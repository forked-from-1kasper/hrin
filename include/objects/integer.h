#ifndef INTEGER_H
#define INTEGER_H

#include <limits.h>

#include <expr.h>

typedef long long int Integer;

#define INTEGER_MIN LLONG_MIN
#define INTEGER_MAX LLONG_MAX

typedef struct { Expr _expr; Integer value; } ExprInteger;

extern ExprTag exprIntegerTag;
void initIntegerTag(Region *);

static inline void * newInteger(Region * region, Integer value) {
    ExprInteger * retval = newExpr(region, exprIntegerTag);
    if (retval == NULL) return NULL;

    retval->value = value;

    return retval;
}

static inline void * evalEnsureInteger(Region * region, void * value) {
    void * o = eval(region, value); IFNRET(o);

    if (tagof(o) != exprIntegerTag) return throw(
        TypeErrorTag, "%s expected to be an integer", showExpr(o)
    );

    return o;
}

#endif