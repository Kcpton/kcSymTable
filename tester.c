#include "symtable.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <assert.h>

int main(void) {
    SymTable_T oSymTable = SymTable_new();
    char acJeter[] = "Jeter";
    int b = 3;
    int * a = &b;
    SymTable_put(oSymTable,acJeter,a);
    SymTable_free(oSymTable);
    return 0;
}