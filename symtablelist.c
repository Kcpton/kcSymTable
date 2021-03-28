/*--------------------------------------------------------------------*/
/* symtablelist.c                                                     */
/* Author: Kevin Chen                                                 */
/*--------------------------------------------------------------------*/

#include "symtable.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>

struct Node
{
   const void *pvItem;
   char* value;
   struct Node *psNext;
};

struct SymTable
{
   struct Node *psFirst;
   size_t length;
};

SymTable_T SymTable_new(void) {
   SymTable_T oSymTable;
   oSymTable = (SymTable_T) malloc(sizeof(struct SymTable));
   if (oSymTable == NULL) {
      return NULL;
   }
   oSymTable->psFirst = NULL;
   oSymTable->length = 0;
   return oSymTable;
}
size_t SymTable_getLength(SymTable_T oSymTable) {
   return oSymTable->length;
}

int SymTable_put(SymTable_T oSymTable, const char *pcKey, 
   const void* pvItem) {
   struct Node *NewNode;
   char* copyKey;
   struct Node *psCurr;
   assert(oSymTable != NULL);
   assert(pcKey != NULL);
   assert(pvItem != NULL);
   if (NewNode == NULL) {
      return 0;
   }
   while(psCurr && strcmp(psCurr->value, pcKey) != 0){
      psCurr = psCurr->psNext;
   }
   if(psCurr != NULL) {
      return 0;
   }
   NewNode =(struct Node*)malloc(sizeof(struct Node));
   NewNode->pvItem = pvItem;
   char* copy = malloc(strlen(pcKey) + 1);
   strcpy(copyKey, pcKey);
   NewNode->value = copyKey;
   NewNode->psNext = oSymTable->psFirst;
   oSymTable->psFirst = NewNode;
   oSymTable->length += 1;
   return 1;
}


int SymTable_contains(SymTable_T oSymTable, const char *pcKey) {
   struct Node *psCurr;
   assert( oSymTable != NULL);
   assert(pcKey != NULL);
   psCurr = oSymTable->psFirst;
   while(psCurr != NULL && strcmp(psCurr->value, pcKey) != 0){
      psCurr = psCurr->psNext;
   }
   if (psCurr == NULL) {
      return 0;
   }
   return 1;
}

void* SymTable_get(SymTable_T oSymTable, const char *pcKey) {
   struct Node *psCurr;
   assert( oSymTable != NULL);
   assert(pcKey != NULL);
   psCurr = oSymTable->psFirst;
   while(psCurr != NULL && strcmp(psCurr->value, pcKey) != 0){
      psCurr = psCurr->psNext;
   }
   if (psCurr == NULL) {
      return 0;
   }
   return (void*) psCurr->pvItem;
}

void* SymTable_replace(SymTable_T oSymTable, const char *pcKey, 
   const void *pvValue) {
   const void *outItem;
   struct Node *psCurr;
   assert( oSymTable != NULL);
   assert(pcKey != NULL);
   psCurr = oSymTable->psFirst;
   while(psCurr != NULL && strcmp(psCurr->value, pcKey) != 0){
      psCurr = psCurr->psNext;
   }
   if (psCurr == NULL) {
      return 0;
   }
   outItem = (void*) psCurr->pvItem;
   psCurr->pvItem = pvValue;
   return (void*) outItem;
}

void *SymTable_remove(SymTable_T oSymTable, const char *pcKey) {
   struct Node*removalNode;
   const void* outItem;
   struct Node *psCurr;
   assert( oSymTable != NULL);
   assert(pcKey != NULL);
   psCurr = oSymTable->psFirst;
   if (psCurr == NULL) {
      return 0;
   } 
   if (strcmp(psCurr->value, pcKey) == 0) {
      outItem = (void*) psCurr->psNext->pvItem;
      removalNode = psCurr->psNext;
      psCurr->psNext = psCurr->psNext->psNext;
      oSymTable->psFirst = psCurr;
      free(removalNode->value);
      free(removalNode);
      return (void *) outItem;
   }
   while(psCurr->psNext != NULL && 
      strcmp(psCurr->psNext->value, pcKey) != 0){
      psCurr = psCurr->psNext;
   }
   if (psCurr->psNext == NULL) {
      return 0;
   }
   outItem = (void*) psCurr->psNext->pvItem;
   removalNode = psCurr->psNext;
   psCurr->psNext = psCurr->psNext->psNext;
   free(removalNode->value);
   free(removalNode);
   return (void *) outItem;
}

void SymTable_free(SymTable_T oSymTable) {
   struct Node* curr;
   struct Node* next;
   assert(oSymTable != NULL);

   for (curr = oSymTable->psFirst; curr != NULL; 
      curr = next) {
      next = curr->psNext;
      free(curr->value);
      free(curr);
   }
   free(oSymTable);
}
 
void SymTable_map(SymTable_T oSymTable,
               void (*pfApply)(void *pvItem, void *pvExtra),
               const void *pvExtra)
{
   struct Node *psCurr;

   assert(oSymTable != NULL);
   assert(pfApply != NULL);

   for (psCurr = oSymTable->psFirst;
        psCurr != NULL;
        psCurr = psCurr->psNext)
      (*pfApply)((void*)psCurr->pvItem, (void*)pvExtra);
}




