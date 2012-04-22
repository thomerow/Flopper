#include <SPI.h>
#include "Flopper.h"
#include "TimerOne.h"


#define TIMER_RESOLUTION  40    // microseconds
#define MIDI_NOTES        128   // Number of existing midi notes
#define POSITION_MAX      158   // tracks x 2 (each track is two stepper motor steps wide)
#define DRIVES            16    // Number of connected drives
#define DIR_UP            0     // Upward direction
#define DIR_DOWN          1     // Downward direction
#define RANDOM_NUMBERS    30    // Size of random drive number table
#define LED_PIN           11    // Teensy led pin

/**
 * @brief  Note length in microseconds
 */
const int noteLength[MIDI_NOTES] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  30578, 28861, 27242, 25713, 24270, 22909, 21622, 20409, 19263, 18182, 17161, 16198,   // C1 - B1
  15289, 14436, 13621, 12856, 12135, 11454, 10811, 10205, 9632, 9091, 8581, 8099,       // C2 - B2
  7645, 7218, 6811, 6428, 6068, 5727, 5406, 5103, 4816, 4546, 4291, 4050,               // C3 - B3
  3823, 3609, 3406, 3214, 3034, 2864, 2703, 2552, 2408, 2273, 2146, 2025,               // C4 - B4
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0
};

/**
 * @brief  Note length in ticks. Is calculated from TIMER_RESOLUTION 
 * and noteLength[] on statup.
 */
int noteTicks[MIDI_NOTES];

/**
 * Note of the playing drives.
 */
byte currentNote[DRIVES];

/**
 * Elapsed ticks of playing drives.
 */
int currentTicks[DRIVES];

/**
 * Current direction (see also DIR_UP and DIR_DOWN)
 */
byte currentDir[DRIVES];

/**
 * Current head position.
 */
int currentPos[DRIVES];

const byte uRegisters = (((DRIVES * 2) - 1) / 8) + 1;

/**
 * Shift register state.
 */
byte registerState[uRegisters];

struct DrivePinInfo {
  byte uReg;      // 0 - based register index the drive is connected to
  byte uDirPin;   // number of direction pin
  byte uStepPin;  // number of motor pin
};

DrivePinInfo drivePinInf[DRIVES];

/**
 * Next drive. Needed to cycle through all connected drives when playing polyphonically.
 */
byte uNextDrive = 0;

void setup()
{
  int i;
  
  pinMode(LED_PIN, OUTPUT);
  
  pinMode(SS, OUTPUT);
  digitalWrite(SS, HIGH);
  SPI.setClockDivider(SPI_CLOCK_DIV2);
  SPI.begin();

  // Init registers
  for (i = 0; i < uRegisters; ++i) registerState[i] = 0;
  updateRegisters();

  // Calculate ticks per note
  for (i = 0; i < MIDI_NOTES; ++i) {     
    noteTicks[i] = round((double) noteLength[i] / TIMER_RESOLUTION);
  }

  // Init arrays
  for (i = 0; i < DRIVES; ++i) {
    currentNote[i] = 0;
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
    drivePinInf[i].uReg = ((i * 2) - 1) / 8;
    drivePinInf[i].uDirPin = (i * 2) % 8;
    drivePinInf[i].uStepPin = drivePinInf[i].uDirPin + 1;
  }
} // initDrivePinInf

/**
 * Sets register state so drive nDrive makes one step on updateRegisters().
 */
inline void prepareStep(byte uDrive)
{ 
  byte uReg, uDirPin, uStepPin, uStepPinState;
  
  if (!currentPos[uDrive]) currentDir[uDrive] = DIR_UP;
  else if (currentPos[uDrive] == POSITION_MAX) currentDir[uDrive] = DIR_DOWN;

  uReg = drivePinInf[uDrive].uReg;
  uStepPin = drivePinInf[uDrive].uStepPin;
  uDirPin = drivePinInf[uDrive].uDirPin;

  // Clear direction pin state
  registerState[uReg] &= ~(1 << uDirPin);
  
  // Set new pin states
  registerState[uReg] ^= (1 << uStepPin);                    // Toggle motor pin
  registerState[uReg] |= (currentDir[uDrive] << uDirPin);    // Set direction pin
  
  if (DIR_UP) ++currentPos[uDrive]; else --currentPos[uDrive];
} // prepareStep

inline void tick(byte uDrive) 
{
  if (!currentNote[uDrive]) return;
  ++currentTicks[uDrive];
  if (currentTicks[uDrive] != noteTicks[currentNote[uDrive]]) return;
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
  if (velocity) playNote(note);
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
  // ToDo: implement.
} // PitchChange

inline void updateRegisters()
{
  byte i;
  
  digitalWrite(SS, LOW);
  for (i = 0; i < uRegisters; ++i) SPI.transfer(registerState[i]);
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

byte findNextIdleDrive()
{ 
  byte uResult = uNextDrive;    // Return uNextDrive if polyphony is used up
  for (int i = 0; i < DRIVES; ++i) {
    if (!currentNote[uNextDrive]) {
      uResult = uNextDrive++;
      if (uNextDrive == DRIVES) uNextDrive = 0;
      break;
    }
  }
  return uResult;
} // findFirstIdleDrive

inline int findDrivePlayingNote(byte uNote)
{
  for (int i = 0; i < DRIVES; ++i) if (currentNote[i] == uNote) return i;
} // findDrivePlayingNote

void playNote(byte uNote)
{  
  currentNote[findNextIdleDrive()] = uNote;
} // playNote

void stopNote(byte uNote)
{
  currentNote[findDrivePlayingNote(uNote)] = 0;
} // stopNote


