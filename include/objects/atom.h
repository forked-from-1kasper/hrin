#ifndef ATOM_H
#define ATOM_H

#include <expr.h>

typedef struct { Expr _expr; char * value; } ExprAtom;

#define ATOM(o) ((ExprAtom *) (o))

extern ExprTag exprAtomTag;
void initAtomTag(Region *);

static inline void * newAtom(Region * region, char * buf) {
    ExprAtom * retval = newExpr(region, &exprAtomTag); IFNRET(retval);

    retval->value = buf;

    return retval;
}

#endif
