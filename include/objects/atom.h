#ifndef ATOM_H
#define ATOM_H

#include <expr.h>

typedef struct { Expr _expr; char * value; } ExprAtom;

extern ExprTag exprAtomTag;
void initAtomTag(Region *);

static inline void * newAtom(Region * region, char * buf) {
    ExprAtom * retval = newExpr(region, exprAtomTag);
    if (retval == NULL) return NULL;

    retval->value = buf;

    return retval;
}

#endif