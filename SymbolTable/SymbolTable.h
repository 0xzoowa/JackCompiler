#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#define MAX_SYMBOLS 256
#define MAX_NAME_LENGTH 64
#define MAX_TYPE_LENGTH 64
#define MAX_KIND_LENGTH 16

typedef struct
{
    char name[MAX_NAME_LENGTH];
    char type[MAX_TYPE_LENGTH];
    char kind[MAX_KIND_LENGTH];
    int index;
} Symbol;

typedef struct
{
    Symbol classScope[MAX_SYMBOLS];      // "static" and "field"
    Symbol subroutineScope[MAX_SYMBOLS]; // "arg" and "var"
    int staticCount;                     // static segment
    int fieldCount;                      // this segment
    int argCount;                        // argument segment
    int varCount;                        // local segment
} SymbolTable;

void symbol_table_init();

void deallocate_symbol_table();

void reset();

void define(const char *name, const char *type, const char *kind);

int varCount(const char *kind);

const char *kindOf(const char *name);

const char *typeOf(const char *name);

int indexOf(const char *name);

void print_symbol();

#endif
