#ifndef __FLOPPER_H__
#define __FLOPPER_H__


// #define MONOPHONIC
#ifdef MONOPHONIC
#define UNISONO 4
#endif


#define TIMER_RESOLUTION  40    // microseconds
#define MIDI_NOTES        128   // Number of existing midi notes
#define POSITION_MAX      158   // tracks x 2 (each track is two stepper motor steps wide)

#ifdef MONOPHONIC
#include "LinkedNoteList.h"
LinkedNoteList *pNoteStack = NULL;
#ifdef UNISONO
#define DRIVES            UNISONO  // Play single note on UNISONO drives
#else
#define DRIVES            1     // Number of connected drives
#endif
#else
#define DRIVES            4    // Number of connected drives
#endif

#define DIR_UP            0     // Upward direction
#define DIR_DOWN          1     // Downward direction
#define LED_PIN           11    // Teensy led pin


/**
 * Note length in microseconds
 */
const int noteLength[MIDI_NOTES] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  30578, 28861, 27242, 25713, 24270, 22909, 21622, 20409, 19263, 18182, 17161, 16198,   // C1 - B1
  15289, 14436, 13621, 12856, 12135, 11454, 10811, 10205, 9632, 9091, 8581, 8099,       // C2 - B2
  7645, 7218, 6811, 6428, 6068, 5727, 5406, 5103, 4816, 4546, 4291, 4050,               // C3 - B3
  3823, 3609, 3406, 3214, 3034, 2864, 2703, 2552, 2408, 2273, 2146, 2025,               // C4 - B4
  1912, 1805, 1703, 1607, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0
};

/**
 * Note length in ticks. Is calculated from TIMER_RESOLUTION 
 * and noteLength[] on statup.
 */
int noteTicks[MIDI_NOTES];

/**
 * Note of the playing drives.
 */
volatile byte currentNote[DRIVES];

/**
 * Elapsed ticks of playing drives.
 */
int currentTicks[DRIVES];

/**
 * Current head position.
 */
int currentPos[DRIVES];

/**
 * Number of 74HC595s
 */
const byte uRegisters = (((DRIVES * 2) - 1) / 8) + 1;

/**
 * Current shift register state.
 */
byte registerState[uRegisters];

/**
 * Next register state.
 */
byte nextRegState[uRegisters];

/**
 * Current direction (see also DIR_UP and DIR_DOWN)
 */
byte currentDir[DRIVES];

/**
 * 
 */
struct DrivePinInfo {
  byte uReg;      // 0-based register index the drive is connected to
  byte uDirPin;   // number of direction pin
  byte uStepPin;  // number of motor pin
};

DrivePinInfo drivePinInf[DRIVES];

/**
 * Next drive. Needed to cycle through all connected drives when playing polyphonically.
 */
byte uNextDrive = 0;


void _NoteOn(byte channel, byte note, byte velocity);
void _NoteOff(byte channel, byte note, byte velocity);
void VelocityChange(byte channel, byte note, byte velocity);
void _ControlChange(byte channel, byte control, byte value);
void _ProgramChange(byte channel, byte program);
void AfterTouch(byte channel, byte pressure);
void PitchChange(uint8_t channel, uint16_t pitch);
  
void updateRegisters();
void initDrivePinInf();
void resetAllPositions();
void timerInt();
void playNote(byte uNote);
void stopNote(byte uNote);


#ifndef MONOPHONIC

byte findNextIdleDrive();
inline int findDrivePlayingNote(byte uNote);

#endif

#endif // __FLOPPER_H__
