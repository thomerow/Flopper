#ifndef __LINKEDNOTELIST_H__
#define __LINKEDNOTELIST_H__

#include "WProgram.h"


struct LinkedNoteList {
  byte             uNote;
  LinkedNoteList*  pPrev;
  LinkedNoteList*  pNext;  
}; // struct LinkedNoteList


void NoteStack_erase(LinkedNoteList **ppStack, byte uNote);
void NoteStack_push(LinkedNoteList **ppStack, byte uNote);
byte NoteStack_last(LinkedNoteList *pStack);
byte NoteStack_popLast(LinkedNoteList **ppStack);
bool NoteStack_isEmpty(LinkedNoteList *pStack);


#endif
