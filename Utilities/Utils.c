#include "Utils.h"
#include "../JackTokenizer/JackTokenizer.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

bool validate_input_file(const char *filename)
{

    // check that the file is a valid file

    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        fprintf(stderr, "Error opening file %s\n", filename);
        return false;
    }
    fclose(file);

    // check if valid extension

    const char *dot = strrchr(filename, '.'); // returns substring starting at position of the found character
    if (!dot || strcmp(dot, ".jack") != 0)
    {
        fprintf(stderr, "Invalid file extension (expected .jack): %s\n", filename);
        return false;
    }
    return true;
}

const char *remove_extension(const char *filename)
{
    char *result;

    char *dot = strrchr(filename, '.');

    if (!dot)
    {
        return filename;
    }
    size_t len = dot - filename;

    result = (char *)malloc(len + 1);

    if (result)
    {
        strncpy(result, filename, len);
        result[len] = '\0';

        return result;
    }
    return NULL;
}

const char *get_filename_without_extension(const char *filename)
{
    char *result;

    char *dot = strrchr(filename, '.');

    char *slash = strrchr(filename, '/'); // unix
    if (!slash)
    {
        slash = strrchr(filename, '\\'); // windows
    }
    const char *base = (slash) ? slash + 1 : filename; // sets base to just after the slash eg /out => (slash == /) and (slash + 1 == o); pointer arithmetic

    if (!dot || (dot < base))
    {
        return base;
    }

    size_t len = dot - base;

    result = (char *)malloc(len + 1);

    if (result)
    {
        strncpy(result, base, len);
        result[len] = '\0';

        return result;
    }
    return NULL;
}

// writexml based on the token type
void write_xml(bool flag, tokenType type, FILE *out, const char *value)
{
    if (type == KEYWORD)
    {

        if (flag && value != NULL)
        {
            fprintf(out, "<keyword>%s</keyword>\n", value);
        }
        else
        {
            fprintf(out, "<keyword>%s</keyword>\n", keyword_type_to_string(keyword()));
        }
    }
    else if (type == SYMBOL)
    {

        const char *sym;

        if (flag && value != NULL)
        {
            sym = value;
        }
        else
        {
            sym = symbol();
        }

        if (strcmp(sym, "<") == 0)
            fprintf(out, "<symbol>&lt;</symbol>\n");
        else if (strcmp(sym, ">") == 0)
            fprintf(out, "<symbol>&gt;</symbol>\n");
        else if (strcmp(sym, "&") == 0)
            fprintf(out, "<symbol>&amp;</symbol>\n");
        else
            fprintf(out, "<symbol>%s</symbol>\n", sym);
    }
    else if (type == IDENTIFIER)
    {
        if (flag && value != NULL)
        {
            fprintf(out, "<identifier>%s</identifier>\n", value);
        }
        else
        {
            fprintf(out, "<identifier>%s</identifier>\n", identifier());
        }
    }
    else if (type == INT_CONST)
    {
        if (flag && value != NULL)
        {
            fprintf(out, "<integerConstant>%s</integerConstant>\n", value);
        }
        else
        {
            fprintf(out, "<integerConstant>%d</integerConstant>\n", int_val());
        }
    }
    else if (type == STRING_CONST)
    {
        if (flag && value != NULL)
        {
            fprintf(out, "<stringConstant>%s</stringConstant>\n", value);
        }
        else
        {
            fprintf(out, "<stringConstant>%s</stringConstant>\n", string_val());
        }
    }
    else
    {
        fprintf(stderr, "write_xml: unknown token type %d\n", type);
    }
}

char *keyword_type_to_string(keywordType kw)
{
    switch (kw)
    {
    case CLASS:
        return "class";
    case METHOD:
        return "method";
    case FUNCTION:
        return "function";
    case CONSTRUCTOR:
        return "constructor";
    case INT:
        return "int";
    case BOOLEAN:
        return "boolean";
    case CHAR:
        return "char";
    case VOID:
        return "void";
    case VAR:
        return "var";
    case STATIC:
        return "static";
    case FIELD:
        return "field";
    case LET:
        return "let";
    case DO:
        return "do";
    case IF:
        return "if";
    case ELSE:
        return "else";
    case WHILE:
        return "while";
    case RETURN:
        return "return";
    case TRUE:
        return "true";
    case FALSE:
        return "false";
    case JACK_NULL:
        return "null";
    case THIS:
        return "this";
    default:
        return "unknown_keyword";
    }
}
