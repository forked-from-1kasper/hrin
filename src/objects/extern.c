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

ExprTag exprExternTag = {
    .eval   = trivEval,
    .apply  = applyExtern,
    .show   = showExtern,
    .delete = trivDelete,
    .move   = trivMove,
    .equal  = equalByRef,
    .size   = sizeof(ExprExtern)
};

void initExternTag(Region * region) {
    UNUSED(region);

    newExprImmortal(&exprTag, &exprExternTag, NULL);
}
