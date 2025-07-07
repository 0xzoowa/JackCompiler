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
void compile_subroutine_call();
void peek_next_token(tokenType *next_type, const char **next_val);

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
    fprintf(out, "<classVarDec>\n");

    expect(KEYWORD, NULL);

    if (currentTokenType() == KEYWORD && (strcmp(currentTokenValue(), "int") == 0 ||
                                          strcmp(currentTokenValue(), "char") == 0 ||
                                          strcmp(currentTokenValue(), "boolean") == 0))
    {
        expect(KEYWORD, NULL);
    }

    expect(IDENTIFIER, NULL);

    while (currentTokenType() == SYMBOL && strcmp(currentTokenValue(), ",") == 0)
    {
        expect(SYMBOL, ",");
        expect(IDENTIFIER, NULL);
    }

    expect(SYMBOL, ";");

    fprintf(out, "</classVarDec>\n");
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

    fprintf(out, "<parameterList>\n");

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

    while (currentTokenType() == SYMBOL && strcmp(currentTokenValue(), ",") == 0)
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

    fprintf(out, "</parameterList>");
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

    fprintf(out, "<subroutineBody>\n");

    expect(SYMBOL, "{");
    compile_var_dec();
    compile_statements();
    expect(SYMBOL, "}");

    fprintf(out, "</subroutineBody>\n");
}

void compile_var_dec(void)
{
    /**
     *  varDec: 'var' type varName (',' varName) * ';'
     */

    fprintf(out, "<varDec>\n");

    expect(KEYWORD, "var");

    if (currentTokenType() == KEYWORD && (strcmp(currentTokenValue(), "int") == 0 ||
                                          strcmp(currentTokenValue(), "char") == 0 ||
                                          strcmp(currentTokenValue(), "boolean") == 0))
    {
        expect(KEYWORD, NULL);
    }
    expect(IDENTIFIER, NULL);

    while (currentTokenType() == SYMBOL && strcmp(currentTokenValue(), ",") == 0)
    {
        expect(SYMBOL, ",");
        expect(IDENTIFIER, NULL);
    }

    expect(SYMBOL, ";");

    fprintf(out, "</varDec>");
}

void compile_statements(void)
{
    /**
     * statements: statement*
     * statement: letStatement| ifStatement| whileStatement | doStatement | returnStatement
     * letStatement: 'let' varName ('[' expression ']')? '=' expression
     * ifStatement: 'if' '('expression ')' '{' statements '}' ('else' '{' statements '}')?
     * whileStatement: 'while' '('expression ')' '{' statements '}'
     * doStatement: 'do' subroutineCall ';'
     * returnStatement: 'return' expression? ';'
     */

    tokenType ttype = currentTokenType();

    const char *tval = currentTokenValue();

    fprintf(out, "<statements>\n");

    if (ttype == KEYWORD && strcmp(tval, "let") == 0)
    {
        compile_let();
    }
    else if (ttype == KEYWORD && strcmp(tval, "if") == 0)
    {
        compile_if();
    }
    else if (ttype == KEYWORD && strcmp(tval, "while") == 0)
    {
        compile_while();
    }
    else if (ttype == KEYWORD && strcmp(tval, "do") == 0)
    {
        compile_do();
    }
    else if (ttype == KEYWORD && strcmp(tval, "return") == 0)
    {
        compile_return();
    }
    else
    {
        fprintf(stderr, "Syntax Error: invalid statement\n");
    }

    fprintf(out, "</statements>\n");
}

void compile_let(void)
{
    /**
     *     * letStatement: 'let' varName ('[' expression ']')? '=' expression
     */

    fprintf(out, "<letStatement>\n");

    expect(KEYWORD, "let");
    expect(IDENTIFIER, NULL);
    if (currentTokenType() == SYMBOL && strcmp(currentTokenValue(), "[") == 0)
    {
        expect(SYMBOL, "[");
        compile_expression();
        expect(SYMBOL, "]");
    }

    expect(SYMBOL, "=");
    compile_expression();

    fprintf(out, "</letStatement>\n");
}

void compile_if(void)
{
    /**
     * ifStatement: 'if' '('expression ')' '{' statements '}' ('else' '{' statements '}')?
     */
    fprintf(out, "<ifStatement>\n");

    expect(KEYWORD, "if");

    expect(SYMBOL, "(");

    compile_expression();

    expect(SYMBOL, ")");

    expect(SYMBOL, "{");

    compile_statements();

    expect(SYMBOL, "}");

    if (currentTokenType() == KEYWORD && strcmp(currentTokenValue(), "else") == 0)
    {
        expect(KEYWORD, "else");
        expect(SYMBOL, "{");
        compile_statements();
        expect(SYMBOL, "}");
    }

    fprintf(out, "</ifStatement>\n");
}

void compile_while(void)
{

    /**
     *  whileStatement: 'while' '('expression ')' '{' statements '}'
     */

    fprintf(out, "<whileStatement> \n");

    expect(KEYWORD, "while");
    expect(SYMBOL, "(");
    compile_expression();
    expect(SYMBOL, ")");
    expect(SYMBOL, "{");
    compile_statements();
    expect(SYMBOL, "}");

    fprintf(out, "</whileStatement> \n");
}

void compile_do(void)
{

    /**
     *  doStatement: 'do' subroutineCall ';'
     */

    fprintf(out, "<doStatement>\n");

    expect(KEYWORD, "do");

    compile_subroutine_call();

    expect(SYMBOL, ";");

    fprintf(out, "</doStatement>\n");
}

void compile_return(void)
{

    /**
     * returnStatement: 'return' expression? ';'
     */

    fprintf(out, "<returnStatement>\n");

    expect(KEYWORD, "return");

    if (!(currentTokenType() == SYMBOL && strcmp(currentTokenValue(), ";") == 0))
    {
        compile_expression();
    }

    expect(SYMBOL, ";");

    fprintf(out, "</returnStatement>\n");
}

void compile_expression(void)
{
    /**
     * expression: term (op term) * ✅
     * term: integerConstant | stringConstant | kevwordConstant | varName |
     *  varName '[' expression ']' | '(' expression ')' | (unaryOp term) | subroutineCall ✅
     * subroutineCall: subroutineName '(' expressionList ')' | (className | varName)'.'subroutineName '(' expressionList ')'
     * expressionList: (expression (',' expression) *)? ✅
     * unaryOp: '-' | '~' ✅
     * keywordConstant: 'true' | 'false' | 'null' | 'this' ✅
     * op: '+' | '-' | '*' | '/' | '&' | '|' | '<' | '>' | '=' ✅
     */

    fprintf(out, "<expression>\n");

    compile_term();
    while (
        currentTokenType() == SYMBOL &&
        (strcmp(currentTokenValue(), "+") == 0 ||
         strcmp(currentTokenValue(), "-") == 0 ||
         strcmp(currentTokenValue(), "*") == 0 ||
         strcmp(currentTokenValue(), "/") == 0 ||
         strcmp(currentTokenValue(), "&") == 0 ||
         strcmp(currentTokenValue(), "|") == 0 ||
         strcmp(currentTokenValue(), "<") == 0 ||
         strcmp(currentTokenValue(), ">") == 0 ||
         strcmp(currentTokenValue(), "=") == 0))
    {
        expect(SYMBOL, NULL);
        compile_term();
    }

    fprintf(out, "</expression>\n");
}

void compile_term(void)
{
    tokenType type = currentTokenType();

    const char *val = currentTokenValue();
    /**
     * term: integerConstant | stringConstant | kevwordConstant | varName |
     *  varName '[' expression ']' | '(' expression ')' | (unaryOp term) | subroutineCall
     */

    fprintf(out, "<term>\n");

    if (type == INT_CONST)
    {
        expect(INT_CONST, NULL);
    }
    else if (type == STRING_CONST)
    {
        expect(STRING_CONST, NULL);
    }
    else if (type == KEYWORD && (strcmp(val, "true") == 0 ||
                                 strcmp(val, "false") == 0 ||
                                 strcmp(val, "null") == 0 ||
                                 strcmp(val, "this") == 0))
    {
        expect(KEYWORD, NULL);
    }
    else if (type == IDENTIFIER)
    {
        const char *next_val;
        tokenType next_type;

        peek_next_token(&next_type, &next_val);
        if (next_type == SYMBOL && strcmp(next_val, "[") == 0)
        {
            // varName '[' expression ']'
            expect(IDENTIFIER, NULL); // varName
            expect(SYMBOL, "[");
            compile_expression();
            expect(SYMBOL, "]");
        }
        else if (next_type == SYMBOL && (strcmp(next_val, "(") == 0 || strcmp(next_val, ".") == 0))
        {
            // subroutineCall
            compile_subroutine_call();
        }
        else
        {

            expect(IDENTIFIER, NULL);
        }
    }
    else if (type == SYMBOL && strcmp(val, "(") == 0)
    {
        // '(' expression ')'
        expect(SYMBOL, "(");
        compile_expression();
        expect(SYMBOL, ")");
    }
    else if (type == SYMBOL && (strcmp(val, "-") == 0 || strcmp(val, "~") == 0))
    {
        // unaryOp term
        expect(SYMBOL, NULL); // '-' or '~'
        compile_term();
    }
    else
    {
        fprintf(stderr, "Error: Invalid term.\n");
    }

    fprintf(out, "</term>\n");
}

int compile_expression_list(void)
{

    tokenType type = currentTokenType();
    const char *val = currentTokenValue();

    int xcount = 0;

    fprintf(out, "<expressionList>\n");

    if (type == INT_CONST ||
        type == STRING_CONST ||
        (type == KEYWORD &&
         (strcmp(val, "true") == 0 ||
          strcmp(val, "false") == 0 ||
          strcmp(val, "null") == 0 ||
          strcmp(val, "this") == 0)) ||
        type == IDENTIFIER ||
        (type == SYMBOL && (strcmp(val, "(") == 0 || strcmp(val, "-") == 0 || strcmp(val, "~") == 0)))
    {
        xcount++;
        compile_expression();
        while (currentTokenType() == SYMBOL && strcmp(currentTokenValue(), ",") == 0)
        {
            expect(SYMBOL, ",");
            compile_expression();
            xcount++;
        }
    }

    fprintf(out, "</expressionList>\n");
    return xcount;
}

// helpers

void compile_subroutine_call()
{
    /**
     * subroutineCall: subroutineName '(' expressionList ')' | (className | varName)'.'subroutineName '(' expressionList ')'
     */
}

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

void peek_next_token(tokenType *next_type, const char **next_val)
{
    long pos = ftell(in); // Save current file position

    char saved_token[MAX_TOKEN_LENGTH];
    strcpy(saved_token, token_string); // Save current token string

    advance_token(); // Advance to next token
    *next_type = currentTokenType();
    *next_val = currentTokenValue();

    fseek(in, pos, SEEK_SET);          // Rewind to saved file position
    strcpy(token_string, saved_token); // Restore token
}
