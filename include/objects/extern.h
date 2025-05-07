#ifndef EXTERN_H
#define EXTERN_H

#include <expr.h>

typedef void * Extern(Region *, Array *);
typedef struct { Expr _expr; Extern * value; } ExprExtern;

extern ExprTag exprExternTag;
void initExternTag(Region *);

static inline void * newExtern(Region * region, Extern * value) {
    ExprExtern * retval = newExpr(region, &exprExternTag);
    if (retval == NULL) return NULL;

    retval->value = value;

    return retval;
}

#endif
