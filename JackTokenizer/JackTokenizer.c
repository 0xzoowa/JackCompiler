#include "JackTokenizer.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_TOKEN_LENGTH 256
#define KEYWORDS 21
static FILE *input = NULL;
static char current_char = NULL;
static char current_token = NULL;
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

    /**
     * while(true) {
     * let or var car = "benz";
     * }
     */

    do
    {
        int ch;
        ch = fgetc(input);
        if (ch == EOF)
        {
            current_char = '\0';
            return;
        }
        current_char = (char)ch;
        set_current_token();
        // skip comments : turn comments to null '\0'

    } while (current_char == '\0' && current_token == '\0');
}

static void build_input_string()
{
    if (input_index < MAX_TOKEN_LENGTH - 1)
    {
        input_string[input_index++] = current_char;
        input_string[input_index] = '\0';
    }
    else
    {
        fprintf(stderr, "Token too long. Truncating.\n");
    }
}

static void set_current_token()
{
    reset_input_string();
    while (!isspace(current_char) || !is_jack_symbol(current_char))
    {
        build_input_string();
        advance();
    }
    current_token = input_string; // check that current token is not null
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
        if (jack_symbols[i] == current_char)
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

    // if it is a symbol access via current character
    if (is_jack_symbol(current_char))
    {
        return SYMBOL;
    }
    // else build out string and access via current token
}
