#include <SPI.h>
#include "Flopper.h"
#include "TimerOne.h"


#define TIMER_RESOLUTION  40    // microseconds
#define MIDI_NOTES        128   // Number of existing midi notes
#define POSITION_MAX      158   // tracks x 2 (each track is two stepper motor steps wide)
#define DRIVES            20    // Number of connected drives
#define DIR_UP            0     // Upward direction
#define DIR_DOWN          1     // Downward direction

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
 * Note periods in ticks of the playing drives.
 */
int currentPeriod[DRIVES];

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

void setup()
{
  int i;
  
  pinMode(SS, OUTPUT);
  digitalWrite(SS, HIGH);
  SPI.setClockDivider(SPI_CLOCK_DIV2);
  SPI.begin();

  // Calculate ticks per note
  for (i = 0; i < MIDI_NOTES; ++i) {     
    noteTicks[i] = round((double) noteLength[i] / TIMER_RESOLUTION);
  }

  // Init arrays
  for (i = 0; i < DRIVES; ++i) {
    currentPeriod[i] = 0;
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
  
  // Init drive positions
  resetAllPositions();
  
  // Init timer
  Timer1.initialize(TIMER_RESOLUTION); // Set up a timer at the defined resolution
  Timer1.attachInterrupt(tick); // Attach the tick function
} // setup

void loop()
{
  // Read MIDI messages
  usbMIDI.read();
} // loop

inline void tick(int drive) 
{
  
} // tick

void tick()
{
  int i;
  
  for (i = 0; i < DRIVES; ++i) tick(i);
} // tick

void _NoteOn(byte channel, byte note, byte velocity)
{
  // ToDo: implement.
} // NoteOn

void _NoteOff(byte channel, byte note, byte velocity)
{
  // ToDo: implement.
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


void resetAllPositions()
{
  // ToDo: implement
} // resetAllPositions



