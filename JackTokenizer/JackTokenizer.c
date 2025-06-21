#include "JackTokenizer.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX_TOKEN_LENGTH 256
#define KEYWORDS 21
static FILE *input = NULL;
static FILE *outT = NULL;
static char current_char = NULL;
static char *current_token = NULL;
static char *input_string = NULL;
static int input_index = 0;

static int peek_char();
static void set_current_token();
static bool handle_symbol();
static bool handle_keyword();
static bool handle_identifier();
static bool handle_string_constant();
static bool handle_integer_constant();

void tokenizer_create(const char *filename, const char *out)
{
    input = fopen(filename, "r");
    if (input == NULL)
    {
        fprintf(stderr, "Error: Cannot open file %s\n", filename);
        exit(EXIT_FAILURE);
    }
    outT = fopen(out, "w");
    if (outT == NULL)
    {
        fprintf(stderr, "Error: Could not create output file %s\n", filename);
        exit(EXIT_FAILURE);
    }
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

static void set_current_token()
{
    reset_input_string();
    while (!isspace(current_char) && !is_jack_symbol(current_char))
    {
        build_input_string();
        advance();
    }
    current_token = strdup(input_string); // check that current token is not null
}

void reset_input_string()
{
    input_index = 0;
    input_string[0] = '\0';
}

static bool handle_symbol()
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

static bool handle_keyword()
{
    // build current token
    set_current_token();

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
        if (strcmp(jack_keywords[i], current_token) == 0)
        {
            return true;
        }
    }
    return false;
}
static bool handle_identifier()
{

    if (isalpha((unsigned char)current_char) || current_char == '_')
    {
        reset_input_string();
        build_input_string();
        advance();

        while (isalnum((unsigned char)current_char) || current_char == '_')
        {
            build_input_string();
            advance();
        }
        current_token = strdup(input_string);
        if (handle_keyword(current_token))
        {
            return false;
        }
        return true;
    }
    return false;
}
static bool handle_string_constant()
{

    if (current_char != '"')
        return false;

    reset_input_string();
    advance();

    while (current_char != '"' && current_char != '\0')
    {
        build_input_string();
        advance();
    }

    if (current_char == '"')
    {
        advance();
        current_token = strdup(input_string);
        return true;
    }

    fprintf(stderr, "Unterminated string constant.\n");
    return false;
}

static bool handle_integer_constant()
{

    if (!isdigit((unsigned char)current_char))
    {
        return false;
    }
    set_current_token();
    for (int i = 0; current_token[i] != '\0'; i++)
    {
        if (!isdigit((unsigned char)current_token[i]))
            return false;
    }
    return true;
}

tokenType token_type()
{

    if (handle_symbol())
    {
        return SYMBOL;
    }
    else if (handle_identifier())
    {
        return IDENTIFIER;
    }
    else if (handle_keyword())
    {
        return KEYWORD;
    }
    else if (handle_string_constant())
    {
        return STRING_CONST;
    }
    else if (handle_integer_constant())
    {
        return INT_CONST;
    }
    else
    {
        return INVALID_TYPE;
    }
}
static int peek_char()
{
    int c = fgetc(input);
    if (c != EOF)
        ungetc(c, input);
    return c;
}