#include <stdio.h>

#include <common.h>

#include <objects/extern.h>

static void * applyExtern(Region * region, void * value, Array * xs) {
    ExprExtern * expr = value;
    return expr->value(region, xs);
}

static char * showExtern(void * value) {
    ExprExtern * expr = value;

    char * retbuf = malloc(27); sprintf(retbuf, "<#NATIVE %016lx>", (uintptr_t) expr->value);

    return retbuf;
}

static void deleteExtern(void * value) {
    UNUSED(value);
}

static void moveExtern(Region * dest, Region * src, void * value) {
    UNUSED(dest); UNUSED(src); UNUSED(value);
}

ExprTagImpl exprExternImpl = {
    .eval   = evalNf,
    .apply  = applyExtern,
    .show   = showExtern,
    .delete = deleteExtern,
    .move   = moveExtern,
    .equal  = equalByRef,
    .size   = sizeof(ExprExtern)
};

ExprTag exprExternTag;

void initExternTag(Region * region) {
    UNUSED(region);

    exprExternTag = newExprTag(exprExternImpl);
}