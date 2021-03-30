/*--------------------------------------------------------------------*/
/* symtable.h                                                       */
/* Author: Kevin Chen                                               */
/*--------------------------------------------------------------------*/

#include <stddef.h>

/* SymTable_T is a pointer to a SymTable */
typedef struct SymTable *SymTable_T;

/* SymTable_new takes in no parameters Creates a new SymTable_T that 
    stores key value binding pairs. Returns the SymTable_T*/
SymTable_T SymTable_new(void);

/* SymTable_free takes in a oSymTable and free it and all of it's 
    contents */
void SymTable_free(SymTable_T oSymTable);

/* SymTable_getLength takes in a oSymTable and return how many
    key value bindings are in the oSymTable as an size_t */
size_t SymTable_getLength(SymTable_T oSymTable);

/* SymTable_put takes in a oSymTable, a string pcKey and a
    generic value pvValue and stores it in the oSymTable. It
    returns 1 if successful and 0 if not. If there is no
    memory for the item, returns 0.*/
int SymTable_put(SymTable_T oSymTable, const char *pcKey, 
    const void *pvValue);

/* SymTable_replace takes in a oSymTable, string pcKey, a generic
    value pvValue. It replaces the original value in the oSymTable
    with the new inputed pvValue and returns the original. */
void *SymTable_replace(SymTable_T oSymTable, const char *pcKey,
    const void *pvValue);

/* SymTable_contains takes in a oSymTable and a string pcKey. If the
    string key is in the oSymTable returns 1, otherwise returns 0 */
int SymTable_contains(SymTable_T oSymTable, const char *pcKey);

/* SymTable_get takes in a oSymTable and a string pcKey. If the
    string key is in the oSymTable returns the value, otherwise
    returns NULL. */
void *SymTable_get(SymTable_T oSymTable, const char *pcKey);

/* SymTable_remove takes in a oSymTable and a string pcKey. If the
    string key is in the oSymTable, remove the binding from the
    oSymTable and return the value. Otherwise return NULL. */
void *SymTable_remove(SymTable_T oSymTable, const char *pcKey);

/* SymTable_map takes in a oSymTable, function pfApply with parameters
    pcKey, pvValue, and pvExtra, and pvExtra. It applys the function
    pfApply with its paramters on all the bindings using the binding's
    key as pcKey, binding's value as pvValue and pvExtra as pvExtra.*/
void SymTable_map(SymTable_T oSymTable,
    void (*pfApply)(const char *pcKey, void *pvValue, void *pvExtra),
    const void *pvExtra);