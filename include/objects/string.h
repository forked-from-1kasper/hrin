#ifndef STRING_H
#define STRING_H

#include <expr.h>

typedef struct { Expr _expr; char * value; } ExprString;

#define STRING(o) ((ExprString *) (o))

extern ExprTag exprStringTag;
void initStringTag(Region *);

#endif
