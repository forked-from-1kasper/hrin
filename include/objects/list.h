#ifndef LIST_H
#define LIST_H

#include <expr.h>

typedef struct { Expr _expr; void * car, * cdr; } ExprList;

extern ExprTag     exprListTag;
extern ExprTagImpl exprListImpl;

static inline void * newList(Region * region, void * car, void * cdr) {
    ExprList * retval = newExpr(region, exprListTag);
    if (retval == NULL) return NULL;

    retval->car = car;
    retval->cdr = cdr;

    return retval;
}

size_t lengthList(void *);

void initListTag(Region *);

#endif