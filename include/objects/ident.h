#ifndef IDENT_H
#define IDENT_H

#include <expr.h>

typedef struct { Expr _expr; char * value; } ExprIdent;

extern ExprTag exprIdentTag;
void initIdentTag(Region *);

static inline void * newIdent(Region * region, char * buf) {
    ExprIdent * retval = newExpr(region, exprIdentTag);
    if (retval == NULL) return NULL;

    retval->value = buf;

    return retval;
}


#endif