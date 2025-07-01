#include "CompilationEngine.h"
#include "../JackTokenizer/JackTokenizer.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

static FILE *in = NULL;
static FILE *out = NULL;
static token_string[MAX_CHAR];

bool advanceToken();
tokenType currentTokenType();
const char *currentTokenValue();

void create_engine(char *in, char *out)
{
    in = fopen(in, "r");
    if (in == NULL)
    {
        fprintf(stderr, "Error: Cannot open file %s\n", in);
        exit(EXIT_FAILURE);
    }

    out = fopen(out, "w");
    if (out == NULL)
    {
        fclose(in);
        fprintf(stderr, "Error: Could not create output file %s\n", out);
        exit(EXIT_FAILURE);
    }
    advanceToken();
    compile_class();
}
void destroy_engine(void)
{
    if (in != NULL)
    {
        fclose(in);
        in = NULL;
    }
    if (out != NULL)
    {
        fclose(out);
        out = NULL;
    }
}
void compile_class(void)
{
    fprintf(out, "<class>\n");

    fprintf(out, "</class>\n");
}
void compile_class_var_dec(void) {}
void compile_subroutine(void) {}
void compile_parameter_list(void) {}
void compile_subroutine_body(void) {}
void compile_var_dec(void) {}
void compile_statements(void) {}
void compile_let(void) {}
void compile_if(void) {}
void compile_while(void) {}
void compile_do(void) {}
void compile_return(void) {}
void compile_expression(void) {}
void compile_term(void) {}
int compile_expression_list(void) {}

bool advanceToken()
{
    while (fgets(token, MAX_CHAR, in))
    {
        token_string[strcspn(token, "\n")] = '\0';

        if (strcmp(token, "<tokens>") == 0 || strcmp(token, "</tokens>") == 0)
        {
            continue;
        }
        return true;
    }
    token_string[0] = '\0';
    return false;
}

tokenType currentTokenType()
{
    if (strncmp(token, "<keyword>", 9) == 0)
    {
        return KEYWORD;
    }
    else if (strncmp(token, "<symbol>", 8) == 0)
    {
        return SYMBOL;
    }
    else if (strncmp(token, "<identifier>", 12) == 0)
    {
        return IDENTIFIER;
    }
    else if (strncmp(token, "<integerConstant>", 18) == 0)
    {
        return INT_CONST;
    }
    else if (strncmp(token, "<stringConstant>", 17) == 0)
    {
        return STRING_CONST;
    }
    else
    {
        return INVALID_TYPE;
    }
}

const char *currentTokenValue()
{
    static char value[MAX_CHAR];
    char *start = strchr(token, '>') + 1;
    char *end = strrchr(token, '<');

    if (!start || !end || end <= start)
    {
        return NULL;
    }

    size_t len = end - start;
    strncpy(value, start, len);
    value[len] = '\0';
    return value;
}
