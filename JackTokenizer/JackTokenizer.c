#include "JackTokenizer.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_TOKEN_LENGTH 256
#define KEYWORDS 21
static FILE *input = NULL;
static char current_token;
static char *input_string = NULL;
static int input_index = 0;

void tokenizer_create(const char *filename)
{
    input = fopen(filename, "r");
    if (input == NULL)
    {
        fprintf(stderr, "Error: Cannot open file %s\n", filename);
        exit(EXIT_FAILURE);
    }
}
void tokenizer_destroy(void)
{
    if (input != NULL)
    {
        fclose(input);
        input = NULL;
    }
}
bool has_more_tokens()
{

    int ch = fgetc(input);
    if (ch == EOF)
        return false;
    ungetc(ch, input);
    return true;
}

void advance()
{
    do
    {
        int ch;
        ch = fgetc(input);
        if (ch == EOF)
        {
            current_token = '\0';
            return;
        }

        if (!isspace(ch))
        {
            current_token = (char)ch;
        }
        else
        {
            current_token = '\0';
        }
        current_token = (char)ch;

    } while (current_token == '\0');
}

static void build_input_string()
{
    if (input_index < MAX_TOKEN_LENGTH - 1)
    {
        input_string[input_index++] = current_token;
        input_string[input_index] = '\0';
    }
    else
    {
        fprintf(stderr, "Token too long. Truncating.\n");
    }
}

void reset_input_string()
{
    input_index = 0;
    input_string[0] = '\0';
}

static bool is_jack_symbol()
{
    char jack_symbols[] = {
        '{', '}', '(', ')', '[', ']', '.', ',', ';',
        '+', '-', '*', '/', '&', '|', '<', '>', '=', '~',
        '\0'};

    for (int i = 0; jack_symbols[i] != '\0'; i++)
    {
        if (jack_symbols[i] == current_token)
            return true;
    }
    return false;
}

static bool is_jack_keyword(char *key)
{

    const char *jack_keywords[KEYWORDS] = {"class",
                                           "method",
                                           "function",
                                           "constructor",
                                           "int",
                                           "boolean",
                                           "char",
                                           "void",
                                           "var",
                                           "static",
                                           "field",
                                           "let",
                                           "do",
                                           "if",
                                           "else",
                                           "while",
                                           "return",
                                           "true",
                                           "false",
                                           "null",
                                           "this"};

    for (int i = 0; i < KEYWORDS; i++)
    {
        if (strcmp(jack_keywords[i], key) == 0)
        {
            return true;
        }
    }
    return false;
}
static bool is_jack_identifier() {}
static bool is_jack_string_constant()
{
}
static bool is_jack_integer_constant()
{
}

tokenType token_type()
{

    /**
     * while(true) {
     * let or var car = "benz";
     * }
     *
     * int_const => 1,2,3,4,5,6,7,8,9,0
     */

    if (is_jack_symbol(current_token))
    {
        return SYMBOL;
    }
    int jack_int[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    for (int j = 0; current_token != '\0' && j < sizeof(jack_int) / sizeof(jack_int[0]); j++)
    {
        if (jack_int[j] == (int)current_token)
            return INT_CONST;
    }
    if (strcmp(current_token, "\"") == 0)
        return STRING_CONST;

    // todo: identifier and keyword
}