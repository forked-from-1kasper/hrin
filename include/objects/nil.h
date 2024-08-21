#ifndef NIL_H
#define NIL_H

#include <expr.h>

typedef Expr ExprNil;

extern ExprNil exprNil;

extern ExprTag exprNilTag;
void initNilTag(Region *);

#endif