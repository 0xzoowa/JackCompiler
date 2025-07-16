#include "CompilationEngine.h"
#include "../JackTokenizer/JackTokenizer.h"
#include "../Utilities/Utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

static FILE *in = NULL;
static FILE *out = NULL;
static char token_string[MAX_CHAR];

bool advance_token();
tokenType currentTokenType();
const char *currentTokenValue();
bool expect(tokenType expectedType, const char *expectedValue);
void compile_subroutine_call();
void peek_next_token(tokenType *next_type, const char **next_val);
void compile_type(void);

void create_engine(char *input, char *output)
{
    in = fopen(input, "r");
    if (in == NULL)
    {
        fprintf(stderr, "Error: Cannot open file %s\n", input);
        exit(EXIT_FAILURE);
    }

    out = fopen(output, "w");
    if (out == NULL)
    {
        fclose(in);
        fprintf(stderr, "Error: Could not create output file %s\n", output);
        exit(EXIT_FAILURE);
    }
    advance_token();
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
        expect(KEYWORD, "class");
    }
    else
    {
        fprintf(stderr, "Error: Expected token \"class\" to start compilation.\n");
        return;
    }

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

    expect(SYMBOL, "}");

    fprintf(out, "</class>\n");
}

void compile_class_var_dec(void)
{
    /**
     * ('static' | 'field') type varName (',' varName)* ';'
     */
    fprintf(out, "<classVarDec>\n");

    if (currentTokenType() == KEYWORD && (strcmp(currentTokenValue(), "static") == 0 ||
                                          strcmp(currentTokenValue(), "field") == 0))
    {
        expect(KEYWORD, NULL);
    }

    if (currentTokenType() == KEYWORD && (strcmp(currentTokenValue(), "int") == 0 ||
                                          strcmp(currentTokenValue(), "char") == 0 ||
                                          strcmp(currentTokenValue(), "boolean") == 0))
    {
        expect(KEYWORD, NULL); // primitive types
    }
    else if (currentTokenType() == IDENTIFIER)
    {
        expect(IDENTIFIER, NULL); // class type
    }

    expect(IDENTIFIER, NULL); // var name

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
        expect(IDENTIFIER, NULL); // class type
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

    fprintf(out, "<parameterList>\n");

    tokenType type = currentTokenType();
    const char *val = currentTokenValue();

    if ((type == KEYWORD && (strcmp(val, "int") == 0 ||
                             strcmp(val, "char") == 0 ||
                             strcmp(val, "boolean") == 0)) ||
        type == IDENTIFIER)
    {
        compile_type();
        expect(IDENTIFIER, NULL); // varName

        while (currentTokenType() == SYMBOL && strcmp(currentTokenValue(), ",") == 0)
        {
            expect(SYMBOL, ",");
            compile_type();
            expect(IDENTIFIER, NULL);
        }
    }

    fprintf(out, "</parameterList>\n");
}

void compile_subroutine_body(void)
{
    /**
     * subroutineBody: '{' varDec* statements '}'
     *
     * varDec: 'var' type varName (',' varName) * ';'
     * statements: statement*
     * statement: letStatement| ifStatement| whileStatement | doStatement | returnStatement
     * letStatement: 'let' varName ('[' expression ']')? '=' expression ';'
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

    while (currentTokenType() == KEYWORD && strcmp(currentTokenValue(), "var") == 0)
    {
        fprintf(out, "<varDec>\n");
        expect(KEYWORD, "var");

        if (currentTokenType() == KEYWORD && (strcmp(currentTokenValue(), "int") == 0 ||
                                              strcmp(currentTokenValue(), "char") == 0 ||
                                              strcmp(currentTokenValue(), "boolean") == 0))
        {
            expect(KEYWORD, NULL);
        }
        else if (currentTokenType() == IDENTIFIER)
        {
            expect(IDENTIFIER, NULL); // class name
        }
        else
        {
            fprintf(stderr, "Error: expected type int, char, boolean or classname, got %s instead", currentTokenValue());
        }

        expect(IDENTIFIER, NULL); // varname

        while (currentTokenType() == SYMBOL && strcmp(currentTokenValue(), ",") == 0)
        {
            expect(SYMBOL, ",");
            expect(IDENTIFIER, NULL);
        }

        expect(SYMBOL, ";");
        fprintf(out, "</varDec>\n");
    }
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

    fprintf(out, "<statements>\n");

    while (currentTokenType() == KEYWORD)
    {
        const char *tval = currentTokenValue();

        if (strcmp(tval, "let") == 0)
        {
            compile_let();
        }
        else if (strcmp(tval, "if") == 0)
        {
            compile_if();
        }
        else if (strcmp(tval, "while") == 0)
        {
            compile_while();
        }
        else if (strcmp(tval, "do") == 0)
        {
            compile_do();
        }
        else if (strcmp(tval, "return") == 0)
        {
            compile_return();
        }
        else
        {
            break;
        }
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

    expect(SYMBOL, ";");

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

    /**
     * expressionList: (expression (',' expression) *)?
     */

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

    if (currentTokenType() != IDENTIFIER)
    {
        fprintf(stderr, "Error: Expected identifier to start subroutine call.\n");
        return;
    }

    expect(IDENTIFIER, NULL);

    if (currentTokenType() == SYMBOL && strcmp(currentTokenValue(), "(") == 0)
    {
        expect(SYMBOL, "(");
        compile_expression_list();
        expect(SYMBOL, ")");
    }
    else if (currentTokenType() == SYMBOL && strcmp(currentTokenValue(), ".") == 0)
    {
        expect(SYMBOL, ".");
        expect(IDENTIFIER, NULL);
        expect(SYMBOL, "(");
        compile_expression_list();
        expect(SYMBOL, ")");
    }
    else
    {
        fprintf(stderr, "Error: Invalid subroutine call syntax.\n");
    }
}

bool advance_token()
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

    // Decode XML entities
    if (strcmp(value, "&lt;") == 0)
    {
        strcpy(value, "<");
    }
    else if (strcmp(value, "&gt;") == 0)
    {
        strcpy(value, ">");
    }
    else if (strcmp(value, "&amp;") == 0)
    {
        strcpy(value, "&");
    }
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

    advance_token();
    return true;
}

void peek_next_token(tokenType *next_type, const char **next_val)
{
    long pos = ftell(in); // save current file position

    char saved_token[MAX_TOKEN_LENGTH];
    strcpy(saved_token, token_string); // save current token string

    advance_token(); // advance to next token
    *next_type = currentTokenType();
    *next_val = currentTokenValue();

    fseek(in, pos, SEEK_SET);          // rewind to saved file position
    strcpy(token_string, saved_token); // restore token
}

void compile_type(void)
{
    tokenType type = currentTokenType();
    const char *val = currentTokenValue();

    if (type == KEYWORD && (strcmp(val, "int") == 0 ||
                            strcmp(val, "char") == 0 ||
                            strcmp(val, "boolean") == 0))
    {
        expect(KEYWORD, NULL);
    }
    else if (type == IDENTIFIER)
    {
        expect(IDENTIFIER, NULL);
    }
    else
    {
        fprintf(stderr, "Error: Expected type but got '%s'\n", val);
    }
}
