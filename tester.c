#include "symtable.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <assert.h>

int main(void) {
    SymTable_T oSymTable;
    char acJeter[] = "Jeter";
    SymTable_put(oSymTable,acJeter,3);
    SymTable_free(oSymTable);
    return 0;
}