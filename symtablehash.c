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

/* Note: For the sake of modularity This file uses all the functions 
   from symtablelist.c to implement all the linkedlist in the symtable. 
   Skip to SymTable_hash for all the Symtable function.*/

/* auBucketCounts contains the different dimensions in size_t that
   the hash table could have */
static const size_t auBucketCounts[] = {509, 1021, 2039, 4093, 8191, 
        16381, 32749, 65521};

/* LinkedList_T is a pointer a LinkedList */
typedef struct LinkedList *LinkedList_T;

/* The Node Struct is used in the LinkedList in 
   SymTable and contains
   a void* pvItem, string psKey, and next node psNext */
struct Node
{
   /* pvItem is the value stored in the Node */
   const void *pvItem;
   /* psKey is the string that stores the identity of the Node */
   char* pvKey;
   /* psNext is a pointer that points to the next Node in the 
      linked List */
   struct Node *psNext;
};

/* LinkedList is the same as SymbolTable in symtablelist.c file.
   It is all the same functions as symboltablelist.c with the
   name changed to LinkedList */
struct LinkedList
{
   /* psFirst is a pointer that points to the first node in the 
      linkedlist */
   struct Node *psFirst;
   /* length is a size_t that stores how many bindings are in 
      the table */
   size_t length;
};

/* SymTable is a hashtable with dimension maxbucket (size of the
   array), number of elements length, bucketnum and LinkedList
   array psArray */
struct SymTable {
   /* maxbucket is a size_t that stores the dimension of the hash
      table */
    size_t maxbucket;
    /* length is a size_t that stores the number of elements in
      SymTable */
    size_t length;
    /* bucketnum is a size_t that stores with index value of
      auBucketCounts that stores what is the maxbucket size and
      the next maxbucket sizes */
    size_t bucketnum;
    /* psArray stores an array of LinkedList_T that represent the
      hashtable */
    LinkedList_T* psArray;
};


/* LinkedList_new returns a new LinkedList_T */
static LinkedList_T LinkedList_new(void) {
   LinkedList_T oLinkedList;
   oLinkedList = (LinkedList_T) malloc(sizeof(struct LinkedList));
   if (oLinkedList == NULL) {
      return NULL;
   }
   oLinkedList->psFirst = NULL;
   oLinkedList->length = 0;
   return oLinkedList;
}

/* LinkedList_getLength gets a oLinkedList and returns the
   number of elements in the LinkedList_T. */
static size_t LinkedList_getLength(LinkedList_T oLinkedList) {
   return oLinkedList->length;
}

/* LinkedList_put gets a oLinkedList, pcKey, and pvItem. Tries to
   put the binding into the linkedlist. Returns 1 if successful, 
   otherwise return 0. */
static int LinkedList_put(LinkedList_T oLinkedList, const char *pcKey, 
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
   if (NewNode == NULL) {
      return 0;
   }
   NewNode->pvItem = pvItem;
   copyKey = malloc(strlen(pcKey) + 1);
   strcpy(copyKey, pcKey);
   NewNode->pvKey = copyKey;
   NewNode->psNext = oLinkedList->psFirst;
   oLinkedList->psFirst = NewNode;
   oLinkedList->length += 1;
   return 1;
}

/* LinkedList_contains gets a oLinkedList and pcKey, and returns 1 if the
   key binding exist. Otherwise returns 0. */
static int LinkedList_contains(LinkedList_T oLinkedList, const char *pcKey) {
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

/* LinkedList_gets gets a oLinkedList and pcKey, and returns the value 
   if the key binding exist. Otherwise returns NULL. */
static void* LinkedList_get(LinkedList_T oLinkedList, const char *pcKey) {
   struct Node *psCurr;
   assert( oLinkedList != NULL);
   assert(pcKey != NULL);
   psCurr = oLinkedList->psFirst;
   while(psCurr != NULL && strcmp(psCurr->pvKey, pcKey) != 0){
      psCurr = psCurr->psNext;
   }
   if (psCurr == NULL) {
      return NULL;
   }
   return (void*) psCurr->pvItem;
}

/* LinkedList_replace gets a oLinkedList, pcKey and pvValue, and returns 
   the replaces the oldValue related with the key with the new value. It
   returns the oldValue if successful, otherwise return NULL. */
static void* LinkedList_replace(LinkedList_T oLinkedList, const char *pcKey, 
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
      return NULL;
   }
   outItem = (void*) psCurr->pvItem;
   psCurr->pvItem = pvValue;
   return (void*) outItem;
}

/* LinkedList_remove takes in a oLinkedList and a string pcKey. If the
    string key is in the oLinkedList, remove the binding from the
    oLinkedList and return the value. Otherwise return NULL. */
static void *LinkedList_remove(LinkedList_T oLinkedList, 
   const char *pcKey) {
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

/* LinkedList_free takes in a oLinkedList and free it and all of it's 
    contents */
static void LinkedList_free(LinkedList_T oLinkedList) {
   struct Node* curr;
   struct Node* next;
   assert(oLinkedList != NULL);

   for (curr = oLinkedList->psFirst; curr != NULL; 
      curr = next) {
      next = curr->psNext;
      free(curr->pvKey);
      free(curr);
   }
   free(oLinkedList);
}

/* LinkedList_map takes in a oLinkedList, function pfApply with parameters
    pcKey, pvValue, and pvExtra, and pvExtra. It applys the function
    pfApply with its paramters on all the bindings using the binding's
    key as pcKey, binding's value as pvValue and pvExtra as pvExtra.*/
static void LinkedList_map(LinkedList_T oLinkedList,
    void (*pfApply)(const char *pcKey, void *pvValue, void *pvExtra),
    const void *pvExtra) {
   struct Node *psCurr;

   assert(oLinkedList != NULL);
   assert(pfApply != NULL);

   for (psCurr = oLinkedList->psFirst;
        psCurr != NULL;
        psCurr = psCurr->psNext)
      (*pfApply)((void*)psCurr->pvKey, (void *)psCurr->pvItem, (void*)pvExtra);
}

/* Actual New Code Starts here. All the code above is from the symtablelist.c
   file */


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

static int SymTable_putNode(SymTable_T oSymTable, struct Node* pNode) {
   size_t hashval = SymTable_hash(pNode->pvKey, oSymTable->maxbucket);
   printf("hi");
   LinkedList_T oLinkedList = oSymTable->psArray[hashval];
   pNode->psNext = oLinkedList->psFirst;
   oLinkedList->psFirst = pNode;
   oLinkedList->length += 1;
   return 1;
}

SymTable_T SymTable_new(void) {
   SymTable_T oSymTable = (SymTable_T) malloc(sizeof(struct SymTable));
   if (oSymTable == NULL) {
      return NULL;
   }
   oSymTable->length = 0;
   oSymTable->maxbucket = auBucketCounts[0];
   oSymTable->psArray = (LinkedList_T*) calloc(sizeof(LinkedList_T),
        (oSymTable->maxbucket)); 
   oSymTable->bucketnum = 0;
   return oSymTable;
}


size_t SymTable_getLength(SymTable_T oSymTable) {
   return oSymTable->length;
}

/* SymTable_put hashes the pcKey and puts the binding pair into
   the oSymTable. If the SymTable length is equal to maxbucket, it
   resizes. */
int SymTable_put(SymTable_T oSymTable, const char *pcKey, 
    const void *pvValue) {
    size_t hashval;
    int output;
    assert(oSymTable != NULL);
    assert(pcKey != NULL);
    /* this portion hashes the string pcKey based on max bucket and
      puts the binding pair into the oSymTable using LinkedList */
    hashval = SymTable_hash(pcKey, oSymTable->maxbucket);
    if (oSymTable->psArray[hashval] == NULL) {
       oSymTable->psArray[hashval] = LinkedList_new();
    }
    output = LinkedList_put(oSymTable->psArray[hashval], pcKey, pvValue);
    if (output) {
        oSymTable->length += 1;
    }
    /* this if statement contains the resizing of the oSymTable if the
      SymTable length is equal to the maxbucket length */
   if (oSymTable->length == oSymTable->maxbucket && oSymTable->length == 
      auBucketCounts[oSymTable->bucketnum] && oSymTable->bucketnum < 
         sizeof(auBucketCounts)/sizeof(auBucketCounts[0]) - 1) {
        LinkedList_T* oldArray = oSymTable->psArray;
        size_t bucketLen;
        size_t i = 0;
        struct Node* head;
        struct Node* next;
        bucketLen = oSymTable->maxbucket;
        oSymTable->bucketnum += 1;
        oSymTable->maxbucket = auBucketCounts[oSymTable->bucketnum];
        oSymTable->psArray = (LinkedList_T*) calloc(sizeof(LinkedList_T),
        (oSymTable->maxbucket));
        oSymTable->length = 0;
        /* puts all the old bindings in the new Symtable */
        while(i < bucketLen) {
            head = NULL;
            if (oldArray[i] != NULL) {
            head = oldArray[i]->psFirst;
            }
            while (head != NULL) {
               next = head->psNext;
               SymTable_putNode(oSymTable, head);
               head = next;
            }
        }
        /* frees all the old linkedlist */
        free(oldArray);
    }
    return output;
    }

int SymTable_contains(SymTable_T oSymTable, const char *pcKey) {
    size_t hashval;
    assert(oSymTable != NULL);
    assert(pcKey != NULL);
    hashval = SymTable_hash(pcKey, oSymTable->maxbucket);
    if (oSymTable->psArray[hashval] == NULL) {
       return 0;
    }
    return LinkedList_contains(oSymTable->psArray[hashval], pcKey);
    }

void* SymTable_get(SymTable_T oSymTable, const char *pcKey) {
    size_t hashval;
    assert(oSymTable != NULL);
    assert(pcKey != NULL);
    hashval = SymTable_hash(pcKey, oSymTable->maxbucket);
    if (oSymTable->psArray[hashval] == NULL) {
       return NULL;
    }
    return LinkedList_get(oSymTable->psArray[hashval], pcKey);
    }

void* SymTable_replace(SymTable_T oSymTable, const char *pcKey, 
    const void *pvValue) {
    size_t hashval;
    assert(oSymTable != NULL);
    assert(pcKey != NULL);
    hashval = SymTable_hash(pcKey, oSymTable->maxbucket);
    if (oSymTable->psArray[hashval] == NULL) {
       return NULL;
    }
    return LinkedList_replace(oSymTable->psArray[hashval], pcKey,
    pvValue);
    }

void* SymTable_remove(SymTable_T oSymTable, const char *pcKey) {
    size_t hashval;
    void* output;
    size_t prevlen;
    assert(oSymTable != NULL);
    assert(pcKey != NULL);
    hashval = SymTable_hash(pcKey, oSymTable->maxbucket);
    if (oSymTable->psArray[hashval] == NULL) {
       return NULL;
    }
    prevlen = LinkedList_getLength(oSymTable->psArray[hashval]);
    output = LinkedList_remove(oSymTable->psArray[hashval], pcKey);
    if (prevlen > LinkedList_getLength(oSymTable->psArray[hashval])) {
        oSymTable->length -= 1;
    }
    return output;
    }

void SymTable_free(SymTable_T oSymTable) {
    size_t bucketLen;
    size_t i = 0;
    assert(oSymTable != NULL);
    bucketLen = oSymTable->maxbucket;
    while(i < bucketLen) {
      if (oSymTable->psArray[i] != NULL) {
      LinkedList_free(oSymTable->psArray[i]);
      }
      i++;
    }
    free(oSymTable->psArray);
    free(oSymTable);
}

void SymTable_map(SymTable_T oSymTable,
    void (*pfApply)(const char *pcKey, void *pvValue, void *pvExtra),
    const void *pvExtra) {
    size_t bucketLen;
    size_t i = 0;
    assert(oSymTable != NULL);
    bucketLen = oSymTable->maxbucket;
    while(i < bucketLen) {
      if (oSymTable->psArray[i] != NULL) {
      LinkedList_map(oSymTable->psArray[i], pfApply, pvExtra);
      }
      i++;
    }
}

