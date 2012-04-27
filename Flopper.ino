#include <SPI.h>
#include "Flopper.h"
#include "TimerOne.h"


void setup()
{
  int i;
  
  pinMode(LED_PIN, OUTPUT);
  
  pinMode(SS, OUTPUT);
  digitalWrite(SS, HIGH);
  SPI.setClockDivider(SPI_CLOCK_DIV2);
  SPI.begin();

  // Init registers
  for (i = 0; i < uRegisters; ++i) {
    registerState[i] = 0;
    nextRegState[i] = 0;
  }
  updateRegisters();

  // Calculate ticks per note
  for (i = 0; i < MIDI_NOTES; ++i) {     
    noteTicks[i] = round((double) noteLength[i] / TIMER_RESOLUTION);
  }

  // Init arrays
  for (i = 0; i < DRIVES; ++i) {
    currentNote[i].uNote = currentNote[i].uVelocity = 0;
    currentTicks[i] = 0;
    
    // Init drive position to max and direction to down so
    // resetting is easyer
    currentDir[i] = DIR_DOWN;
    currentPos[i] = POSITION_MAX;
  }
  
  // Register MIDI callback functions
  usbMIDI.setHandleNoteOff(_NoteOff);
  usbMIDI.setHandleNoteOn(_NoteOn);
  usbMIDI.setHandleVelocityChange(VelocityChange);
  usbMIDI.setHandleControlChange(_ControlChange);
  usbMIDI.setHandleProgramChange(_ProgramChange);
  usbMIDI.setHandleAfterTouch(AfterTouch);
  usbMIDI.setHandlePitchChange(PitchChange);  
  
  initDrivePinInf();

  // Init drive positions
  resetAllPositions();
  
  // Init timer
  Timer1.initialize(TIMER_RESOLUTION);  // Set up a timer at the defined resolution
  Timer1.attachInterrupt(timerInt);     // Attach the timer function
} // setup


void loop()
{
  // Read MIDI messages
  usbMIDI.read();
} // loop


void initDrivePinInf()
{
  for (int i = 0; i < DRIVES; ++i) {
    drivePinInf[i].uReg = uRegisters - ((i * 2) - 1) / 8 - 1;  // Backwards so register order is correct
    drivePinInf[i].uDirPin = (i * 2) % 8;
    drivePinInf[i].uStepPin = drivePinInf[i].uDirPin + 1;
  }
} // initDrivePinInf


/**
 * Sets register state so drive nDrive makes one step on updateRegisters().
 */
inline void prepareStep(byte uDrive)
{ 
  if (!currentPos[uDrive]) currentDir[uDrive] = DIR_UP;
  else if (currentPos[uDrive] == POSITION_MAX) currentDir[uDrive] = DIR_DOWN;

  if (currentDir[uDrive] == DIR_UP) ++currentPos[uDrive]; else --currentPos[uDrive];
  
  DrivePinInfo& pinInf = drivePinInf[uDrive];
  byte& regState = nextRegState[pinInf.uReg];

  // Clear direction pin state
  regState &= ~(1 << pinInf.uDirPin);
  
  // Set new pin states
  regState ^= (1 << pinInf.uStepPin);                    // Toggle motor pin
  regState |= (currentDir[uDrive] << pinInf.uDirPin);    // Set direction pin
} // prepareStep


inline void tick(byte uDrive) 
{
  if (!currentNote[uDrive].uNote) return;
  ++currentTicks[uDrive];
  if (currentTicks[uDrive] < noteTicks[currentNote[uDrive].uNote]) return;
  prepareStep(uDrive);
  currentTicks[uDrive] = 0;
} // tick


void timerInt()
{
  for (int i = 0; i < DRIVES; ++i) tick(i);  
  updateRegisters();
} // timerInt


void _NoteOn(byte channel, byte note, byte velocity)
{
  if (velocity) playNote(note, velocity);
  else stopNote(note);
} // NoteOn


void _NoteOff(byte channel, byte note, byte velocity)
{
  stopNote(note);
} // NoteOff


void VelocityChange(byte channel, byte note, byte velocity)
{
  // ToDo: implement.
} // VelocityChange


void _ControlChange(byte channel, byte control, byte value)
{
  // ToDo: implement.
} // ControlChange


void _ProgramChange(byte channel, byte program)
{
  // ToDo: implement.
} // ProgramChange


void AfterTouch(byte channel, byte pressure)
{
  // ToDo: implement.
} // AfterTouch


void PitchChange(uint8_t channel, uint16_t pitch)
{
  // ToDo: implement
} // PitchChange


inline void updateRegisters()
{
  byte i;
  
  // First update direction pins:
  
  digitalWrite(SS, LOW);
  for (i = 0; i < uRegisters; ++i) {
    SPI.transfer((nextRegState[i] & B01010101) | (registerState[i] & B10101010));
  }
  digitalWrite(SS, HIGH);
  
  // Then update stepper motor pins:
  
  digitalWrite(SS, LOW);
  for (i = 0; i < uRegisters; ++i) {
    SPI.transfer(nextRegState[i]);
    registerState[i] = nextRegState[i]; // Copy next to current state    
  }
  digitalWrite(SS, HIGH);  
} // updateRegisters


/**
 * Advances stepper motor positions of all drives until
 * Every drive reaches position 0.
 */
void resetAllPositions()
{
  for (int i = 0; i < POSITION_MAX; ++i) {
    for (int j = 0; j < DRIVES; ++j) {
      if (currentPos[j]) prepareStep(j);
    }    
    updateRegisters();
    delay(10);
  }
} // resetAllPositions


#ifndef MONOPHONIC


byte findNextIdleDrive()
{ 
  byte uResult = uNextDrive;    // Return uNextDrive if polyphony is used up
  for (int i = 0; i < DRIVES; ++i) {
    if (!currentNote[uNextDrive].uNote) {
      uResult = uNextDrive;
      if (++uNextDrive == DRIVES) uNextDrive = 0;
      break;
    }
    if (++uNextDrive == DRIVES) uNextDrive = 0;
  }
  return uResult;
} // findFirstIdleDrive


inline int findDrivePlayingNote(byte uNote)
{
  for (int i = 0; i < DRIVES; ++i) if (currentNote[i].uNote == uNote) return i;
} // findDrivePlayingNote


void playNote(byte uNote, byte uVelocity)
{  
  if (!noteTicks[uNote]) return;
  
  int nDrive = findNextIdleDrive();
  currentNote[nDrive].uNote = uNote;
  currentNote[nDrive].uVelocity = uVelocity;
} // playNote


void stopNote(byte uNote)
{
  if (!noteTicks[uNote]) return;
  
  int nDrive = findDrivePlayingNote(uNote);
  currentNote[nDrive].uNote = 
  currentNote[nDrive].uVelocity = 0;
} // stopNote


#else // #ifndef MONOPHONIC


#ifdef UNISONO

void addUnisonoVoices(MIDINote &note)
{
  byte uVelocity = round(((double) UNISONO / 127) * note.uVelocity);
  for (int i = 1; i < UNISONO; ++i) {
    if (i >= uVelocity) currentNote[i].uNote = currentNote[i].uVelocity = 0;
    else currentNote[i] = note;
  }  
} // addUnisonoVoices

#endif


void playNote(byte uNote, byte uVelocity)
{
  if (!noteTicks[uNote]) return;
  
  // Push currently playing note onto the stack
  if (currentNote[0].uNote) NoteStack_push(&pNoteStack, currentNote[0].uNote, currentNote[0].uVelocity);  
  currentNote[0].uNote = uNote;
  currentNote[0].uVelocity = uVelocity;
  
#ifdef UNISONO
  addUnisonoVoices(currentNote[0]);
#endif
} // playNote


void stopNote(byte uNote)
{
  if (!noteTicks[uNote]) return;
  
  // Play previously played note.
  if (uNote != currentNote[0].uNote) NoteStack_erase(&pNoteStack, uNote);  
  else NoteStack_popLast(&pNoteStack, currentNote[0].uNote, currentNote[0].uVelocity);
  
#ifdef UNISONO
  addUnisonoVoices(currentNote[0]);
#endif
} // stopNote


#endif // #ifndef MONOPHONIC

