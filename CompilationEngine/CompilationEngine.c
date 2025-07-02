#include "CompilationEngine.h"
#include "../JackTokenizer/JackTokenizer.h"
#include "../Utilities/Utils.h"
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
void expect(tokenType expectedType, const char *expectedValue);

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

    expect(KEYWORD, "class");
    expect(IDENTIFIER, NULL);
    expect(SYMBOL, "{");

    while (currentTokenType() == KEYWORD && (strcmp(currentTokenValue(), "static") == 0 || strcmp(currentTokenValue(), "field") == 0))
    {
        compile_class_var_dec();
    }
    while (currentTokenType() == KEYWORD && (strcmp(currentTokenValue(), "constructor") == 0 || strcmp(currentTokenValue(), "function") == 0 || strcmp(currentTokenValue(), "method") == 0))
    {
        compile_subroutine();
    }

    fprintf(out, "</class>\n");
}
void compile_class_var_dec(void) {}
void compile_subroutine(void)
{
    fprintf(out, "<subroutineDec>\n");

    // constructor | function | method
    if (currentTokenType() == KEYWORD && (strcmp(currentTokenValue(), "constructor") == 0 ||
                                          strcmp(currentTokenValue(), "function") == 0 ||
                                          strcmp(currentTokenValue(), "method") == 0))
    {
        expect(KEYWORD, NULL);
    }
    // void | type
    if (currentTokenType() == KEYWORD && (strcmp(currentTokenValue(), "int") == 0 ||
                                          strcmp(currentTokenValue(), "char") == 0 ||
                                          strcmp(currentTokenValue(), "boolean") == 0 ||
                                          strcmp(currentTokenValue(), "void") == 0))
    {
        expect(KEYWORD, NULL);
    }
    else if (currentTokenType() == IDENTIFIER)
    {
        expect(IDENTIFIER, NULL); // class type return
    }
    else
    {
        fprintf(stderr, "Expected return type\n");
        return;
    }

    expect(IDENTIFIER, NULL);

    expect(SYMBOL, "(");

    compile_parameter_list();

    expect(SYMBOL, ")");

    compile_subroutine_body();

    fprintf(out, "</subroutineDec>\n");
}
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
    while (fgets(token_string, MAX_CHAR, in))
    {
        token_string[strcspn(token_string, "\n")] = '\0';

        if (strcmp(token_string, "<tokens>") == 0 || strcmp(token_string, "</tokens>") == 0)
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
    if (strncmp(token_string, "<keyword>", 9) == 0)
    {
        return KEYWORD;
    }
    else if (strncmp(token_string, "<symbol>", 8) == 0)
    {
        return SYMBOL;
    }
    else if (strncmp(token_string, "<identifier>", 12) == 0)
    {
        return IDENTIFIER;
    }
    else if (strncmp(token_string, "<integerConstant>", 18) == 0)
    {
        return INT_CONST;
    }
    else if (strncmp(token_string, "<stringConstant>", 17) == 0)
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
    char *start = strchr(token_string, '>') + 1;
    char *end = strrchr(token_string, '<');

    if (!start || !end || end <= start)
    {
        return NULL;
    }

    size_t len = end - start;
    strncpy(value, start, len);
    value[len] = '\0';
    return value;
}

void expect(tokenType expectedType, const char *expectedValue)
{
    tokenType actualType = currentTokenType();
    const char *actualValue = currentTokenValue();

    if (actualType != expectedType)
    {
        fprintf(stderr, "Syntax Error: Expected token type %d, got %d\n", expectedType, actualType);
        return;
    }

    if (expectedValue != NULL && strcmp(expectedValue, actualValue) != 0)
    {
        fprintf(stderr, "Syntax Error: Expected token value '%s', got '%s'\n", expectedValue, actualValue);
        return;
    }

    write_xml(actualType, out);

    advanceToken();
}
