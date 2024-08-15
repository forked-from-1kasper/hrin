#ifndef BOOLEAN_H
#define BOOLEAN_H

#include <expr.h>

typedef struct { Expr _expr; bool value; } ExprBoolean;

extern ExprBoolean exprTrue, exprFalse;
extern ExprTag     exprBooleanTag;
extern ExprTagImpl exprBooleanImpl;

void initBooleanTag(Region *);

#endif