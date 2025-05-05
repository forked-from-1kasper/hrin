#ifndef LAMBDA_H
#define LAMBDA_H

#include <expr.h>

typedef struct { Expr _expr; Scope * scope; Array vars; void * value; } ExprLexical;

extern ExprTag exprLambdaTag, exprMacroTag;
void initLexicalTags(Region *);

#endif
