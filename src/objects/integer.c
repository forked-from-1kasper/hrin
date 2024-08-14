#include <stdio.h>

#include <common.h>

#include <objects/integer.h>

static char * showInteger(void * value) {
    ExprInteger * expr = value;

    char * retbuf = malloc(12);
    sprintf(retbuf, "%d", expr->value);

    return retbuf;
}

static void deleteInteger(void * value) {
    UNUSED(value);
}

static void moveInteger(Region * dest, Region * src, void * value) {
    UNUSED(dest); UNUSED(src); UNUSED(value);
}

ExprTagImpl exprIntegerImpl = {
    .eval   = evalNf,
    .apply  = applyThrowError,
    .show   = showInteger,
    .delete = deleteInteger,
    .move   = moveInteger,
    .size   = sizeof(ExprInteger)
};

ExprTag exprIntegerTag;
