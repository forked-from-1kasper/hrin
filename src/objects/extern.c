#include <stdio.h>

#include <common.h>

#include <objects/extern.h>

static void * applyExtern(Region * region, void * value, Array * xs) {
    ExprExtern * expr = value;
    return expr->value(region, xs);
}

static size_t showExtern(char * buf, size_t size, void * value) {
    if (size <= 26) return ellipsis(buf);

    ExprExtern * expr = value;
    return snprintf(buf, size, "<#NATIVE %016lx>", (uintptr_t) expr->value);
}

static void deleteExtern(void * value) {
    UNUSED(value);
}

static void * moveExtern(Region * dest, Region * src, void * value) {
    UNUSED(dest); UNUSED(src); UNUSED(value);

    return value;
}

static ExprTagImpl exprExternImpl = {
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
