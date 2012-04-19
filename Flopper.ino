#include <SPI.h>
#include "Flopper.h"


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
} // setup

void loop()
{

} // loop

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

void PitchChange(byte channel, int pitch)
{

} // PitchChange



