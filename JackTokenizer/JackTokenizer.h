#ifndef JACKTOKENIZER_H
#define JACKTOKENIZER_H

#include <stdbool.h>

typedef enum typeOfToken
{
    KEYWORD = 0,
    SYMBOL,
    IDENTIFIER,
    INT_CONST,
    STRING_CONST,
    INVALID_TYPE
} tokenType;

typedef enum typeOfKeyword
{

    CLASS = 0,
    METHOD,
    FUNCTION,
    CONSTRUCTOR,
    INT,
    BOOLEAN,
    CHAR,
    VOID,
    VAR,
    STATIC,
    FIELD,
    LET,
    DO,
    IF,
    ELSE,
    WHILE,
    RETURN,
    TRUE,
    FALSE,
    JACK_NULL,
    THIS,
    INVALID_KEYWORD,
    STRING_ERROR
} keywordType;

void tokenizer_create(const char *filename, const char *out);
void tokenizer_destroy(void);
bool has_more_tokens();
void advance();
tokenType token_type();
keywordType keyword();
char *symbol();
char *identifier();
int int_val();
char *string_val();

#endif