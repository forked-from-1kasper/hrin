#include <stdlib.h>
#include <ctype.h>

#include <io.h>
#include <lexer.h>

static int lookahead(int recv) {
    if (isspace(recv)) return TokenError;

    switch (recv) {
        case EOF: return TokenEof;
        case '(': return TokenLparen;
        case ')': return TokenRparen;
        case '[': return TokenLbracket;
        case ']': return TokenRbracket;
        case ';': return TokenSemicolon;
        case ',': return TokenComma;
        case '"': return TokenLiteral;
        default:  return -1;
    }
}

int lexerNextToken(FILE * stream, char * outbuf[]) {
    int recv; do recv = fgetc(stream); while (isspace(recv));

    int token = lookahead(recv);

    if (token < 0) {
        fpush(recv);

        while (lookahead(recv) < 0)
            recv = fnext(stream);

        ungetc(recv, stream);

        *outbuf = fdup(); return TokenIdent;
    } else if (token == TokenLiteral) {
        for (;;) switch (fnext(stream)) {
            case EOF: fdrop(); return TokenError;
            case '"': *outbuf = fdup(); return TokenLiteral;
        }
    } else return token;
}

static int bufferedToken = -1;
static char * lexerBuffer = NULL;

char * getLexerBuffer()
{ return lexerBuffer; }

void pushToken(int token) {
    if (bufferedToken < 0) bufferedToken = token;
}

int popToken(FILE * stream) {
    if (bufferedToken < 0) return lexerNextToken(stream, &lexerBuffer);

    int retval = bufferedToken; bufferedToken = -1; return retval;
}
