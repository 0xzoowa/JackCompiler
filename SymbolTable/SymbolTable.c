#include <stdio.h>
#include <string.h>
#include <stdalign.h>
#include <stdlib.h>
#include "SymbolTable.h"

static Symbol *lookup(SymbolTable *table, const char *name);

static const char *KINDS[] = {"static",
                              "field",
                              "arg",
                              "var"};
static SymbolTable *table = NULL;

void symbol_table_init(SymbolTable *table)
{

    // table = malloc(sizeof(SymbolTable));

    // if (!table)
    // {

    //     fprintf(stderr, "Error initializing symbol table");
    //     exit(EXIT_FAILURE);
    // }

    table->argCount = 0;
    table->fieldCount = 0;
    table->staticCount = 0;
    table->varCount = 0;
}

void reset(SymbolTable *table)
{
    table->argCount = 0;
    table->varCount = 0;
    memset(table->subroutineScope, 0, sizeof(table->subroutineScope));
}

void deallocate_symbol_table()
{
    if (table != NULL)
    {
        free(table);
        table = NULL;
    }
}

void define(SymbolTable *table, const char *name, const char *type, const char *kind)
{
    Symbol symbol;

    strncpy(symbol.name, name, MAX_NAME_LENGTH);
    strncpy(symbol.type, type, MAX_TYPE_LENGTH);
    strncpy(symbol.kind, kind, MAX_KIND_LENGTH);

    if (strcmp(kind, "static") == 0)
    {
        symbol.index = table->staticCount++;
        table->classScope[symbol.index] = symbol;
    }
    else if (strcmp(kind, "field") == 0)
    {
        symbol.index = table->fieldCount++;
        table->classScope[table->staticCount + symbol.index] = symbol;
    }
    else if (strcmp(kind, "arg") == 0)
    {
        symbol.index = table->argCount++;
        table->subroutineScope[symbol.index] = symbol;
    }
    else if (strcmp(kind, "var") == 0)
    {
        symbol.index = table->varCount++;
        table->subroutineScope[table->argCount + symbol.index] = symbol;
    }
}

int varCount(SymbolTable *table, const char *kind)
{
    if (strcmp(kind, "static") == 0)
        return table->staticCount;
    if (strcmp(kind, "field") == 0)
        return table->fieldCount;
    if (strcmp(kind, "arg") == 0)
        return table->argCount;
    if (strcmp(kind, "var") == 0)
        return table->varCount;
    return 0;
}

const char *kindOf(SymbolTable *table, const char *name)
{
    Symbol *symbol = lookup(table, name);
    return symbol ? symbol->kind : "none";
}

const char *typeOf(SymbolTable *table, const char *name)
{
    Symbol *symbol = lookup(table, name);
    return symbol ? symbol->type : NULL;
}

int indexOf(SymbolTable *table, const char *name)
{
    Symbol *symbol = lookup(table, name);
    return symbol ? symbol->index : -1;
}

static Symbol *lookup(SymbolTable *table, const char *name)
{
    // check subroutine scope first
    for (int i = 0; i < table->argCount + table->varCount; i++)
    {
        if (strcmp(table->subroutineScope[i].name, name) == 0)
        {
            return &table->subroutineScope[i];
        }
    }
    // check class scope
    for (int i = 0; i < table->staticCount + table->fieldCount; i++)
    {
        if (strcmp(table->classScope[i].name, name) == 0)
        {
            return &table->classScope[i];
        }
    }
    return NULL;
}