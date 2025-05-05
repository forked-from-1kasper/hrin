#ifndef BYTE_H
#define BYTE_H

#include <expr.h>

typedef struct { Expr _expr; char value; } ExprByte;

#define BYTE(o) ((ExprByte *) (o))

extern ExprTag exprByteTag;
void initByteTag(Region *);

static inline void * newByte(Region * region, char value) {
    ExprByte * retval = newExpr(region, exprByteTag); IFNRET(retval);

    retval->value = value;

    return retval;
}

#endif
