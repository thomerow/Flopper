#ifndef __FLOPPER_H__
#define __FLOPPER_H__

void _NoteOn(byte channel, byte note, byte velocity);
void _NoteOff(byte channel, byte note, byte velocity);
void VelocityChange(byte channel, byte note, byte velocity);
void _ControlChange(byte channel, byte control, byte value);
void _ProgramChange(byte channel, byte program);
void AfterTouch(byte channel, byte pressure);
void PitchChange(uint8_t channel, uint16_t pitch);
  
void updateRegisters();
  
#endif // __FLOPPER_H__
