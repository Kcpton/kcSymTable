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

/* The Node Struct is used in the LinkedList SymTable and contains
   a void* pvItem, string psKey, and next node psNext */
struct Node
{
   /* pvItem is the value stored in the Node */
   const void *pvItem;
   /* psKey is the string that stores the identity of the Node */
   char* psKey;
   /* psNext is a pointer that points to the next Node in the 
      linked List */
   struct Node *psNext;
};

/* The SymTable struct contains the first node of the linkedlist 
   psFirst and contains it's length as a size_t */
struct SymTable
{
   /* psFirst is a pointer that points to the first node in the 
      linkedlist */
   struct Node *psFirst;
   /* length is a size_t that stores how many bindings are in 
      the table */
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
   const void* pvValue) {
   struct Node *NewNode;
   char* copyKey;
   struct Node *psCurr;
   assert(oSymTable != NULL);
   assert(pcKey != NULL);
   psCurr = oSymTable->psFirst;
   while(psCurr != NULL && strcmp(psCurr->psKey, pcKey) != 0){
      psCurr = psCurr->psNext;
   }
   if (psCurr != NULL) {
      return 0;
   }
   NewNode =(struct Node*)malloc(sizeof(struct Node));
   if (NewNode == NULL) {
      return 0;
   }
   NewNode->pvItem = pvValue;
   copyKey = malloc(strlen(pcKey) + 1);
   strcpy(copyKey, pcKey);
   NewNode->psKey = copyKey;
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
   while(psCurr != NULL && strcmp(psCurr->psKey, pcKey) != 0){
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
   while(psCurr != NULL && strcmp(psCurr->psKey, pcKey) != 0){
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
   while(psCurr != NULL && strcmp(psCurr->psKey, pcKey) != 0){
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
      return NULL;
   } 
   if (strcmp(psCurr->psKey, pcKey) == 0) {
      outItem = (void*) psCurr->pvItem;
      removalNode = psCurr;
      oSymTable->psFirst = psCurr->psNext;
      free(removalNode->psKey);
      free(removalNode);
      oSymTable->length -=  1;
      return (void *) outItem;
   }
   while(psCurr->psNext != NULL && 
      strcmp(psCurr->psNext->psKey, pcKey) != 0){
      psCurr = psCurr->psNext;
   }
   if (psCurr->psNext == NULL) {
      return NULL;
   }
   outItem = (void*)psCurr->psNext->pvItem;
   removalNode = psCurr->psNext;
   psCurr->psNext = psCurr->psNext->psNext;
   free(removalNode->psKey);
   free(removalNode);
   oSymTable->length -= 1;
   return (void *) outItem;
}

void SymTable_free(SymTable_T oSymTable) {
   struct Node* curr;
   struct Node* next;
   assert(oSymTable != NULL);

   for (curr = oSymTable->psFirst; curr != NULL; 
      curr = next) {
      next = curr->psNext;
      free(curr->psKey);
      free(curr);
   }
   free(oSymTable);
}
 
void SymTable_map(SymTable_T oSymTable,
    void (*pfApply)(const char *pcKey, void *pvValue, void *pvExtra),
    const void *pvExtra) {
   struct Node *psCurr;

   assert(oSymTable != NULL);
   assert(pfApply != NULL);

   for (psCurr = oSymTable->psFirst;
        psCurr != NULL;
        psCurr = psCurr->psNext)
      (*pfApply)((void*)psCurr->psKey, (void *)psCurr->pvItem, (void*)pvExtra);
}




