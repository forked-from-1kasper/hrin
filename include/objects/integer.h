#ifndef INTEGER_H
#define INTEGER_H

#include <expr.h>

typedef long long int Integer;

typedef struct { Expr _expr; Integer value; } ExprInteger;

extern ExprTag     exprIntegerTag;
extern ExprTagImpl exprIntegerImpl;

static inline void * newInteger(Region * region, Integer value) {
    ExprInteger * retval = newExpr(region, exprIntegerTag);
    if (retval == NULL) return NULL;

    retval->value = value;

    return retval;
}

void initIntegerTag(Region *);

#endif