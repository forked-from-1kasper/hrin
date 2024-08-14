#ifndef NIL_H
#define NIL_H

#include <expr.h>

typedef struct { Expr _expr; } ExprNil;

extern ExprNil     exprNil;
extern ExprTag     exprNilTag;
extern ExprTagImpl exprNilImpl;

#endif