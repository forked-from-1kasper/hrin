#ifndef STRING_H
#define STRING_H

#include <expr.h>

typedef struct { Expr _expr; char * value; } ExprString;

extern ExprTag     exprStringTag;
extern ExprTagImpl exprStringImpl;

void initStringTag(Region *);

#endif
