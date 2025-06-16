#ifndef JACKTOKENIZER_H
#define JACKTOKENIZER_H

#include <stdbool.h>

typedef enum typeOfToken
{
    KEYWORD = 1,
    SYMBOL,
    IDENTIFIER,
    INT_CONST,
    STRING_CONST
} tokenType;

typedef enum typeOfKeyword
{

    CLASS = 1,
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
    THIS
} keywordType;

bool has_more_tokens();
void advance();
tokenType token_type();
keywordType keyword();
char symbol();
char *identifier();
int int_val();
char *string_val();

#endif