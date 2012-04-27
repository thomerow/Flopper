#ifndef __LINKEDNOTELIST_H__
#define __LINKEDNOTELIST_H__

#include "WProgram.h"


struct LinkedNoteList {
  byte             uNote;
  byte             uVelocity;
  LinkedNoteList*  pPrev;
  LinkedNoteList*  pNext;  
}; // struct LinkedNoteList


void NoteStack_erase(LinkedNoteList **ppStack, byte uNote);
void NoteStack_push(LinkedNoteList **ppStack, byte uNote, byte uVelocity);
byte NoteStack_last(LinkedNoteList *pStack);
void NoteStack_popLast(LinkedNoteList **ppStack, byte &uNote, byte &uVelocity);
bool NoteStack_isEmpty(LinkedNoteList *pStack);


#endif
