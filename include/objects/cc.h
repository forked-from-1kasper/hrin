#ifndef CC_H
#define CC_H

#include <expr.h>

typedef struct { Expr _expr; void * car, * cdr; } ExprCC;

extern ExprTag exprCCTag;
void initCCTag(Region *);

static inline void * newCC(Region * region, void * car, void * cdr) {
    ExprCC * retval = newExpr(region, exprCCTag);
    if (retval == NULL) return NULL;

    retval->car = car;
    retval->cdr = cdr;

    return retval;
}

size_t lengthList(void *);

#endif