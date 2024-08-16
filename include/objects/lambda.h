#ifndef LAMBDA_H
#define LAMBDA_H

#include <expr.h>

typedef struct { Expr _expr; Scope * scope; Array vars; void * value; } ExprLambda;

extern ExprTag exprLambdaTag;
void initLambdaTag(Region *);

#endif