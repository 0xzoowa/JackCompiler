#include <stdio.h>
#include <string.h>
#include <stdalign.h>
#include <stdlib.h>
#include "SymbolTable.h"

static Symbol *lookup(const char *name);

static const char *KINDS[] = {"static",
                              "field",
                              "arg",
                              "var"};
static SymbolTable *symbol_table = NULL;

void symbol_table_init()
{

    symbol_table = malloc(sizeof(SymbolTable));

    if (!symbol_table)
    {

        fprintf(stderr, "Error initializing symbol table");
        exit(EXIT_FAILURE);
    }

    symbol_table->argCount = 0;
    symbol_table->fieldCount = 0;
    symbol_table->staticCount = 0;
    symbol_table->varCount = 0;

    memset(symbol_table->classScope, 0, sizeof(symbol_table->classScope));
    memset(symbol_table->subroutineScope, 0, sizeof(symbol_table->subroutineScope));
}

void reset()
{
    symbol_table->argCount = 0;
    symbol_table->varCount = 0;
    memset(symbol_table->subroutineScope, 0, sizeof(symbol_table->subroutineScope));
}

void deallocate_symbol_table()
{
    if (symbol_table != NULL)
    {
        free(symbol_table);
        symbol_table = NULL;
    }
}

void define(const char *name, const char *type, const char *kind)
{
    Symbol symbol;

    strncpy(symbol.name, name, MAX_NAME_LENGTH);
    strncpy(symbol.type, type, MAX_TYPE_LENGTH);
    strncpy(symbol.kind, kind, MAX_KIND_LENGTH);

    if (strcmp(kind, "static") == 0)
    {
        symbol.index = symbol_table->staticCount++;
        symbol_table->classScope[symbol.index] = symbol;
    }
    else if (strcmp(kind, "field") == 0)
    {
        symbol.index = symbol_table->fieldCount++;
        symbol_table->classScope[symbol_table->staticCount + symbol.index] = symbol;
    }
    else if (strcmp(kind, "arg") == 0)
    {
        symbol.index = symbol_table->argCount++;
        symbol_table->subroutineScope[symbol.index] = symbol;
    }
    else if (strcmp(kind, "var") == 0)
    {
        symbol.index = symbol_table->varCount++;
        symbol_table->subroutineScope[symbol_table->argCount + symbol.index] = symbol;
    }
}

int varCount(const char *kind)
{
    if (strcmp(kind, "static") == 0)
        return symbol_table->staticCount;
    if (strcmp(kind, "field") == 0)
        return symbol_table->fieldCount;
    if (strcmp(kind, "arg") == 0)
        return symbol_table->argCount;
    if (strcmp(kind, "var") == 0)
        return symbol_table->varCount;
    return 0;
}

const char *kindOf(const char *name)
{
    Symbol *symbol = lookup(name);
    return symbol ? symbol->kind : "none";
}

const char *typeOf(const char *name)
{
    Symbol *symbol = lookup(name);
    return symbol ? symbol->type : NULL;
}

int indexOf(const char *name)
{
    Symbol *symbol = lookup(name);
    return symbol ? symbol->index : -1;
}

void print_symbol()
{
    if (symbol_table == NULL)
    {
        printf("Empty symbol table\n");
        return;
    }

    printf("\n--- CLASS SCOPE ---\n");
    for (int i = 0; i < symbol_table->staticCount + symbol_table->fieldCount; i++)
    {
        printf("name: %-10s  type: %-10s  kind: %-10s  index: %d\n",
               symbol_table->classScope[i].name,
               symbol_table->classScope[i].type,
               symbol_table->classScope[i].kind,
               symbol_table->classScope[i].index);
    }

    printf("\n--- SUBROUTINE SCOPE ---\n");
    for (int i = 0; i < symbol_table->argCount + symbol_table->varCount; i++)
    {
        printf("name: %-10s  type: %-10s  kind: %-10s  index: %d\n",
               symbol_table->subroutineScope[i].name,
               symbol_table->subroutineScope[i].type,
               symbol_table->subroutineScope[i].kind,
               symbol_table->subroutineScope[i].index);
    }
}

static Symbol *lookup(const char *name)
{
    // check subroutine scope first
    for (int i = 0; i < symbol_table->argCount + symbol_table->varCount; i++)
    {
        if (strcmp(symbol_table->subroutineScope[i].name, name) == 0)
        {
            return &symbol_table->subroutineScope[i];
        }
    }
    // check class scope
    for (int i = 0; i < symbol_table->staticCount + symbol_table->fieldCount; i++)
    {
        if (strcmp(symbol_table->classScope[i].name, name) == 0)
        {
            return &symbol_table->classScope[i];
        }
    }
    return NULL;
}