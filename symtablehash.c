/*--------------------------------------------------------------------*/
/* symtablehash.c                                                     */
/* Author: Kevin Chen                                                 */
/*--------------------------------------------------------------------*/

#include "symtable.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>

static const size_t auBucketCounts[] = {509, 1021, 2039, 4093, 8191, 
        16381, 32749, 65521};

typedef struct LinkedList *LinkedList_T;
struct Node
{
   const void *pvItem;
   char* pvKey;
   struct Node *psNext;
};

struct LinkedList
{
   struct Node *psFirst;
   size_t length;
};
struct SymTable {
    size_t maxbucket;
    size_t length;
    struct LinkedList* psArray;
};

LinkedList_T LinkedList_new(void) {
   LinkedList_T oLinkedList;
   oLinkedList = (LinkedList_T) malloc(sizeof(struct LinkedList));
   if (oLinkedList == NULL) {
      return NULL;
   }
   oLinkedList->psFirst = NULL;
   oLinkedList->length = 0;
   return oLinkedList;
}

size_t LinkedList_getLength(LinkedList_T oLinkedList) {
   return oLinkedList->length;
}

int LinkedList_put(LinkedList_T oLinkedList, const char *pcKey, 
   const void* pvItem) {
   struct Node *NewNode;
   char* copyKey;
   struct Node *psCurr;
   assert(oLinkedList != NULL);
   assert(pcKey != NULL);
   psCurr = oLinkedList->psFirst;
   while(psCurr != NULL && strcmp(psCurr->pvKey, pcKey) != 0){
      psCurr = psCurr->psNext;
   }
   if (psCurr != NULL) {
      return 0;
   }
   NewNode =(struct Node*)malloc(sizeof(struct Node));
   NewNode->pvItem = pvItem;
   copyKey = malloc(strlen(pcKey) + 1);
   strcpy(copyKey, pcKey);
   NewNode->pvKey = copyKey;
   NewNode->psNext = oLinkedList->psFirst;
   oLinkedList->psFirst = NewNode;
   oLinkedList->length += 1;
   return 1;
}

int LinkedList_contains(LinkedList_T oLinkedList, const char *pcKey) {
   struct Node *psCurr;
   assert( oLinkedList != NULL);
   assert(pcKey != NULL);
   psCurr = oLinkedList->psFirst;
   while(psCurr != NULL && strcmp(psCurr->pvKey, pcKey) != 0){
      psCurr = psCurr->psNext;
   }
   if (psCurr == NULL) {
      return 0;
   }
   return 1;
}

void* LinkedList_get(LinkedList_T oLinkedList, const char *pcKey) {
   struct Node *psCurr;
   assert( oLinkedList != NULL);
   assert(pcKey != NULL);
   psCurr = oLinkedList->psFirst;
   while(psCurr != NULL && strcmp(psCurr->pvKey, pcKey) != 0){
      psCurr = psCurr->psNext;
   }
   if (psCurr == NULL) {
      return 0;
   }
   return (void*) psCurr->pvItem;
}

void* LinkedList_replace(LinkedList_T oLinkedList, const char *pcKey, 
   const void *pvValue) {
   const void *outItem;
   struct Node *psCurr;
   assert( oLinkedList != NULL);
   assert(pcKey != NULL);
   psCurr = oLinkedList->psFirst;
   while(psCurr != NULL && strcmp(psCurr->pvKey, pcKey) != 0){
      psCurr = psCurr->psNext;
   }
   if (psCurr == NULL) {
      return 0;
   }
   outItem = (void*) psCurr->pvItem;
   psCurr->pvItem = pvValue;
   return (void*) outItem;
}

void *LinkedList_remove(LinkedList_T oLinkedList, const char *pcKey) {
   struct Node*removalNode;
   const void* outItem;
   struct Node *psCurr;
   assert( oLinkedList != NULL);
   assert(pcKey != NULL);
   psCurr = oLinkedList->psFirst;
   if (psCurr == NULL) {
      return NULL;
   } 
   if (strcmp(psCurr->pvKey, pcKey) == 0) {
      outItem = (void*) psCurr->pvItem;
      removalNode = psCurr;
      oLinkedList->psFirst = psCurr->psNext;
      free(removalNode->pvKey);
      free(removalNode);
      oLinkedList->length -=  1;
      return (void *) outItem;
   }
   while(psCurr->psNext != NULL && 
      strcmp(psCurr->psNext->pvKey, pcKey) != 0){
      psCurr = psCurr->psNext;
   }
   if (psCurr->psNext == NULL) {
      return NULL;
   }
   outItem = (void*)psCurr->psNext->pvItem;
   removalNode = psCurr->psNext;
   psCurr->psNext = psCurr->psNext->psNext;
   free(removalNode->pvKey);
   free(removalNode);
   oLinkedList->length -= 1;
   return (void *) outItem;
}

void LinkedList_free(LinkedList_T oLinkedList) {
   struct Node* curr;
   struct Node* next;
   assert(oLinkedList != NULL);

   for (curr = oLinkedList->psFirst; curr != NULL; 
      curr = next) {
      next = curr->psNext;
      free(curr->pvKey);
      free(curr);
   }
}

void LinkedList_map(LinkedList_T oLinkedList,
    void (*pfApply)(const char *pcKey, void *pvValue, void *pvExtra),
    const void *pvExtra) {
   struct Node *psCurr;

   assert(oLinkedList != NULL);
   assert(pfApply != NULL);

   for (psCurr = oLinkedList->psFirst;
        psCurr != NULL;
        psCurr = psCurr->psNext)
      (*pfApply)((void*)psCurr->pvItem, (void *)psCurr->pvKey, (void*)pvExtra);
}

/* Return a hash code for pcKey that is between 0 and uBucketCount-1,
        inclusive. */
        
static size_t SymTable_hash(const char *pcKey, size_t uBucketCount) {
    const size_t HASH_MULTIPLIER = 65599;
    size_t u;
    size_t uHash = 0;
    assert(pcKey != NULL);

    for (u = 0; pcKey[u] != '\0'; u++)
    uHash = uHash * HASH_MULTIPLIER + (size_t)pcKey[u];
        
    return uHash % uBucketCount;
    }

SymTable_T SymTable_new_help(size_t maxbucket) {
    SymTable_T oSymTable;
    oSymTable = (SymTable_T) malloc(sizeof(struct SymTable));
    oSymTable->length = 0;
    oSymTable->maxbucket = maxbucket;
    oSymTable->psArray = (struct LinkedList*) malloc(sizeof(struct LinkedList)
        * (maxbucket)); 
    return oSymTable;
}

SymTable_T SymTable_new(void) {
   return SymTable_new_help(auBucketCounts[0]);
}


size_t SymTable_getLength(SymTable_T oSymTable) {
   return oSymTable->length;
}

int SymTable_put(SymTable_T oSymTable, const char *pcKey, 
    const void *pvValue) {
    size_t hashval;
    int output;
    assert(oSymTable != NULL);
    assert(pcKey != NULL);
    hashval = SymTable_hash(pcKey, oSymTable->maxbucket);
    output = LinkedList_put(oSymTable->psArray + hashval, pcKey, pvValue);
    if (output) {
        oSymTable->length += 1;
    }
    return output;
    }

int SymTable_contains(SymTable_T oSymTable, const char *pcKey) {
    size_t hashval;
    assert(oSymTable != NULL);
    assert(pcKey != NULL);
    hashval = SymTable_hash(pcKey, oSymTable->maxbucket);
    return LinkedList_contains(oSymTable->psArray + hashval, pcKey);
    }

void* SymTable_get(SymTable_T oSymTable, const char *pcKey) {
    size_t hashval;
    assert(oSymTable != NULL);
    assert(pcKey != NULL);
    hashval = SymTable_hash(pcKey, oSymTable->maxbucket);
    return LinkedList_get(oSymTable->psArray + hashval, pcKey);
    }

void* SymTable_replace(SymTable_T oSymTable, const char *pcKey, 
    const void *pvValue) {
    size_t hashval;
    assert(oSymTable != NULL);
    assert(pcKey != NULL);
    hashval = SymTable_hash(pcKey, oSymTable->maxbucket);
    return LinkedList_replace(oSymTable->psArray + hashval, pcKey,
    pvValue);
    }

void* SymTable_remove(SymTable_T oSymTable, const char *pcKey) {
    size_t hashval;
    void* output;
    size_t prevlen;
    assert(oSymTable != NULL);
    assert(pcKey != NULL);
    hashval = SymTable_hash(pcKey, oSymTable->maxbucket);
    prevlen = LinkedList_getLength(oSymTable->psArray + hashval);
    output = LinkedList_remove(oSymTable->psArray + hashval, pcKey);
    if (prevlen > LinkedList_getLength(oSymTable->psArray + hashval)) {
        oSymTable->length -= 1;
    }
    return output;
    }

void SymTable_free(SymTable_T oSymTable) {
    /*size_t bucketLen;
    size_t i = 0;
    struct LinkedList** pCurr;
    assert(oSymTable != NULL);
    bucketLen = oSymTable->maxbucket;
    pCurr =  (struct LinkedList*) (oSymTable->psArray);
    i = 0;
    while(i < bucketLen) {
        if(*(pCurr + i) != NULL) {
            LinkedList_free(pCurr + i);
        }
        i += 1;
    }
    free(oSymTable);
    */
}

void SymTable_map(SymTable_T oSymTable,
    void (*pfApply)(const char *pcKey, void *pvValue, void *pvExtra),
    const void *pvExtra) {
    size_t bucketLen;
    size_t i = 0;
    struct LinkedList *pCurr;
    assert(oSymTable != NULL);
    bucketLen = oSymTable->maxbucket;
    pCurr =  oSymTable->psArray;
    while(i < bucketLen) {
        if((pCurr + i) != NULL) {
            LinkedList_map(pCurr + i, &pfApply, pvExtra);
        }
        i += 1;
    }
}
    