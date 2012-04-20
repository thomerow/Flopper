#include <SPI.h>
#include "Flopper.h"
#include "TimerOne.h"


#define TIMER_RESOLUTION  40    // microseconds
#define POSITION_MAX      158   // tracks x 2 (each track is two stepper motor steps wide)


void setup()
{
  pinMode(SS, OUTPUT);
  digitalWrite(SS, HIGH);
  SPI.setClockDivider(SPI_CLOCK_DIV2);
  SPI.begin();

  // Register MIDI callback functions
  usbMIDI.setHandleNoteOff(_NoteOff);
  usbMIDI.setHandleNoteOn(_NoteOn);
  usbMIDI.setHandleVelocityChange(VelocityChange);
  usbMIDI.setHandleControlChange(_ControlChange);
  usbMIDI.setHandleProgramChange(_ProgramChange);
  usbMIDI.setHandleAfterTouch(AfterTouch);
  usbMIDI.setHandlePitchChange(PitchChange);
  
  // Init timer
  Timer1.initialize(RESOLUTION); // Set up a timer at the defined resolution
  Timer1.attachInterrupt(tick); // Attach the tick function
} // setup

void loop()
{
  // Read MIDI messages
  usbMIDI.read();
} // loop

void tick()
{
  
} // tick

void _NoteOn(byte channel, byte note, byte velocity)
{

} // NoteOn

void _NoteOff(byte channel, byte note, byte velocity)
{

} // NoteOff

void VelocityChange(byte channel, byte note, byte velocity)
{

} // VelocityChange

void _ControlChange(byte channel, byte control, byte value)
{

} // ControlChange

void _ProgramChange(byte channel, byte program)
{

} // ProgramChange

void AfterTouch(byte channel, byte pressure)
{

} // AfterTouch

void PitchChange(uint8_t channel, uint16_t pitch)
{

} // PitchChange



