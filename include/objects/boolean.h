#ifndef BOOLEAN_H
#define BOOLEAN_H

#include <expr.h>

typedef Expr ExprBoolean;

extern ExprBoolean exprTrue, exprFalse;

extern ExprTag exprBooleanTag;
void initBooleanTag(Region *);

static inline void * newBool(bool value)
{ return value ? &exprTrue : &exprFalse; }

#endif
