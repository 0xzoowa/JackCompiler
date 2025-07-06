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
bool expect(tokenType expectedType, const char *expectedValue);

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
    /**
     * class: 'class' className '{' classVarDec* subroutineDec* '}'
     * classVarDec: ('static' | 'field') type varName (',' varName)* ';'
     * subroutineDec: ('constructor' 'function' 'method') ('void'|type) subroutineName
     * '(' parameterList ')' subroutineBody
     */
    fprintf(out, "<class>\n");

    if (currentTokenType() == KEYWORD && strcmp(currentTokenValue(), "class") == 0)
    {
        expect(KEYWORD, NULL);
    } // else handle/log error: error handling??

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

void compile_class_var_dec(void)
{
    /**
     * ('static' | 'field') type varName (',' varName)* ';'
     */
}

void compile_subroutine(void)
{

    /**
     * ('constructor' 'function' 'method') ('void'|type) subroutineName
     * '(' parameterList ')' subroutineBody
     *
     *  type:  |'int' |'char' | 'boolean'| className
     *
     *  ('void'|type): 'void' |'int' |'char' | 'boolean'| className
     *
     * subroutineName : identifier
     *
     * parameterList: ( (type varName) (',' type varName) *)?
     *
     * subroutineBody: '{' varDec* statements '}'
     *
     *
     */

    fprintf(out, "<subroutineDec>\n");

    //('constructor' 'function' 'method')
    if (currentTokenType() == KEYWORD && (strcmp(currentTokenValue(), "constructor") == 0 ||
                                          strcmp(currentTokenValue(), "function") == 0 ||
                                          strcmp(currentTokenValue(), "method") == 0))
    {
        expect(KEYWORD, NULL);
    }
    //('void'|type): 'void' |'int' |'char' | 'boolean'| className: identifier

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

    // subroutineName : identifier

    expect(IDENTIFIER, NULL);

    expect(SYMBOL, "(");

    compile_parameter_list();

    expect(SYMBOL, ")");

    compile_subroutine_body();

    fprintf(out, "</subroutineDec>\n");
}

void compile_parameter_list(void)
{

    /**
     *  parameterList: ( (type varName) (',' type varName) *)?
     *
     */

    tokenType ttype = currentTokenType();
    const char *tvalue = currentTokenValue();

    fprintf(out, "<ParameterList>\n");

    expect(ttype, "(");

    //(type):'int' |'char' | 'boolean'| className: identifier
    if (ttype == KEYWORD && (strcmp(tvalue, "int") == 0 ||
                             strcmp(tvalue, "char") == 0 ||
                             strcmp(tvalue, "boolean") == 0))
    {
        expect(KEYWORD, NULL);
        expect(IDENTIFIER, NULL); // var name
    }

    else if (ttype == IDENTIFIER)
    {
        expect(IDENTIFIER, NULL); // class name
        expect(IDENTIFIER, NULL); // var name
    }

    while (ttype == SYMBOL && strcmp(tvalue, ",") == 0)
    {
        expect(SYMBOL, ",");

        ttype = currentTokenType();
        tvalue = currentTokenValue();

        //(type):'int' |'char' | 'boolean'| className: identifier
        if (ttype == KEYWORD && (strcmp(tvalue, "int") == 0 ||
                                 strcmp(tvalue, "char") == 0 ||
                                 strcmp(tvalue, "boolean") == 0))
        {
            expect(KEYWORD, NULL);    // type
            expect(IDENTIFIER, NULL); // var name
        }

        else if (ttype == IDENTIFIER)
        {
            expect(IDENTIFIER, NULL); // class name
            expect(IDENTIFIER, NULL); // var name
        }
    }

    expect(SYMBOL, ")");

    fprintf(out, "</ParameterList>");
}

void compile_subroutine_body(void)
{
    /**
     * subroutineBody: '{' varDec* statements '}'
     *
     * varDec: 'var' type varName (',' varName) * ';'
     * statements: statement*
     * statement: letStatement| ifStatement| whileStatement | doStatement | returnStatement
     * letStatement: 'let' varName ('[' expression ']')? '=' expression
     * ifStatement: 'if' '('expression ')' '{' statements '}' ('else' '{' statements '}')?
     * whileStatement: 'while' '('expression ')' '{' statements '}'
     * doStatement: 'do' subroutineCall ';'
     * returnStatement: 'return' expression? ';'
     */
}

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
    else if (strncmp(token_string, "<integerConstant>", 17) == 0)
    {
        return INT_CONST;
    }
    else if (strncmp(token_string, "<stringConstant>", 16) == 0)
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

bool expect(tokenType expectedType, const char *expectedValue)
{
    tokenType actualType = currentTokenType();
    const char *actualValue = currentTokenValue();

    if (actualType != expectedType)
    {
        fprintf(stderr, "Syntax Error: Expected token type %d, got %d\n", expectedType, actualType);
        return false;
    }

    if (expectedValue != NULL && strcmp(expectedValue, actualValue) != 0)
    {
        fprintf(stderr, "Syntax Error: Expected token value '%s', got '%s'\n", expectedValue, actualValue);
        return false;
    }

    write_xml(true, actualType, out, actualValue);

    advanceToken();
    return true;
}
