#include "JackTokenizer.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define KEYWORDS (sizeof(jack_keywords) / sizeof(jack_keywords[0]))
static FILE *input = NULL;
static FILE *outT = NULL;
static char current_char = '\0';
static char *current_token = NULL;
static char *input_string = NULL;
static int input_index = 0;
static const char *jack_keywords[] = {"class",
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

static int peek_char();
static bool handle_symbol(char ch);
static bool handle_keyword(char *keyword);

void tokenizer_create(const char *filename, FILE *out)
{
    input = fopen(filename, "r");
    if (input == NULL)
    {
        fprintf(stderr, "Error: Cannot open file %s\n", filename);
        exit(EXIT_FAILURE);
    }
    outT = out;
    fprintf(outT, "<tokens>");
}

void tokenizer_destroy(void)
{
    if (input != NULL)
    {
        fclose(input);
        input = NULL;
    }
    fprintf(outT, "</tokens>");
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
    while (1)
    {
        int ch = fgetc(input);
        if (ch == EOF)
        {
            current_char = '\0';
            return;
        }

        // Skip comments
        if (ch == '/')
        {
            int next = peek_char();
            if (next == '/')
            {
                while ((ch = fgetc(input)) != '\n' && ch != EOF)
                    ;
                continue;
            }
            if (next == '*')
            {
                fgetc(input);
                int prev = 0;
                while ((ch = fgetc(input)) != EOF)
                {
                    if (prev == '*' && ch == '/')
                        break;
                    prev = ch;
                }
                continue;
            }
        }

        current_char = (char)ch;
        return;
    }
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

void reset_input_string()
{
    input_index = 0;
    input_string[0] = '\0';
}

static bool handle_symbol(char ch)
{
    char jack_symbols[] = {
        '{', '}', '(', ')', '[', ']', '.', ',', ';',
        '+', '-', '*', '/', '&', '|', '<', '>', '=', '~',
        '\0'};

    for (int i = 0; jack_symbols[i] != '\0'; i++)
    {
        if (jack_symbols[i] == ch)
            return true;
    }
    return false;
}

static bool handle_keyword(char *keyword)
{

    for (unsigned long i = 0; i < KEYWORDS; i++)
    {
        if (strcmp(jack_keywords[i], keyword) == 0)
        {
            return true;
        }
    }
    return false;
}

tokenType token_type()
{

    reset_input_string();

    if (current_char == '"')
    {
        advance();
        while (current_char != '"' && current_char != '\0')
        {
            build_input_string();
            advance();
        }
        if (current_char == '\0')
        {
            // no closing quote
            current_token = strdup(input_string);
            fprintf(stderr, "Error: Unterminated string constant.\n");
            return STRING_ERROR;
        }
        advance(); // Skip closing quote
        current_token = strdup(input_string);
        return STRING_CONST;
    }
    else if (isdigit(current_char))
    {
        while (isdigit(current_char))
        {
            build_input_string();
            advance();
        }
        current_token = strdup(input_string);
        return INT_CONST;
    }
    else if (isalpha(current_char) || current_char == '_')
    {
        while (isalnum(current_char) || current_char == '_')
        {
            build_input_string();
            advance();
        }
        current_token = strdup(input_string);
        return handle_keyword(current_token) ? KEYWORD : IDENTIFIER;
    }
    else if (handle_symbol(current_char))
    {
        build_input_string();
        current_token = strdup(input_string);
        advance();
        return SYMBOL;
    }
    else
    {
        current_token = NULL;
        return INVALID_TYPE;
    }
}

keywordType keyword()
{

    for (unsigned long i = 0; i < KEYWORDS; i++)
    {
        if (strcmp(jack_keywords[i], current_token) == 0)
        {
            return (keywordType)i;
        }
    }
    fprintf(stderr, "Invalid keyword: %s\n", current_token);
    return INVALID_KEYWORD;
}

char *symbol()
{
    return current_token;
}

char *identifier()
{
    return current_token;
}

int int_val()
{
    return atoi(current_token);
}

char *string_val()
{
    return current_token;
}

static int peek_char()
{
    int c = fgetc(input);
    if (c != EOF)
        ungetc(c, input);
    return c;
}