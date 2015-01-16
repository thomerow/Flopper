#include "LinkedNoteList.h"


inline LinkedNoteList* NoteStack_findElem(LinkedNoteList *pStack, byte uNote)
{
  LinkedNoteList* pElem = pStack;
  
  while (pElem) {
    if (pElem->uNote == uNote) break;
    pElem = pElem->pNext; 
  }
  
  return pElem;
} // NoteStack_findElem


void NoteStack_erase(LinkedNoteList **ppStack, byte uNote)
{
  LinkedNoteList *pElem = NoteStack_findElem(*ppStack, uNote);
  if (!pElem) return;

  if (pElem->pPrev) pElem->pPrev->pNext = pElem->pNext;
  if (pElem->pNext) pElem->pNext->pPrev = pElem->pPrev;
  
  // Change root pointer if necessary
  if (pElem == *ppStack) *ppStack = pElem->pNext;
  
  free(pElem);
} // NoteStack_removeNote


inline LinkedNoteList* NoteStack_lastElem(LinkedNoteList *pStack)
{
  if (!pStack) return NULL;
  
  LinkedNoteList* pLast = pStack;
  while (pLast->pNext) pLast = pLast->pNext;
  return pLast;
} // NoteStack_lastElem


void NoteStack_push(LinkedNoteList **ppStack, byte uNote, byte uVelocity)
{  
  if (!uNote) return;
  
  LinkedNoteList* pElemNew = (LinkedNoteList*) malloc(sizeof(LinkedNoteList));
  pElemNew->uNote = uNote;
  pElemNew->uVelocity = uVelocity;
  pElemNew->pPrev = NoteStack_lastElem(*ppStack);
  if (pElemNew->pPrev) pElemNew->pPrev->pNext = pElemNew;
  pElemNew->pNext = NULL;
  
  // If list was empty, let root pointer point to new element
  if (!*ppStack) *ppStack = pElemNew;
} // NoteStack_addNote


byte NoteStack_last(LinkedNoteList *pStack)
{
  return NoteStack_lastElem(pStack)->uNote;
} // NoteStack_last


void NoteStack_popLast(LinkedNoteList **ppStack, byte &uNote, byte &uVelocity)
{
  uNote = uVelocity = 0;
  if (!*ppStack) return;
  
  LinkedNoteList* pLast = NoteStack_lastElem(*ppStack);
  uNote = pLast->uNote;
  uVelocity = pLast->uVelocity;
    
  // Forget and delete element
  if (pLast->pPrev) pLast->pPrev->pNext = NULL;
  if (*ppStack == pLast) *ppStack = NULL;  
  free(pLast);
} // NoteStack_popLast

