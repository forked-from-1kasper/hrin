#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>

enum {
    TokenEof,
    TokenError,
    TokenLparen,
    TokenRparen,
    TokenLbracket,
    TokenRbracket,
    TokenSemicolon,
    TokenComma,
    TokenIdent,
    TokenLiteral
};

char * getLexerBuffer(void);

void pushToken(int token);
int popToken(FILE *);

#endif
