#include <stdio.h>

#include <common.h>

#include <objects/string.h>

static size_t showString(char * buf, size_t size, void * value) {
    ExprString * expr = value;

    size_t length = strlen(expr->value);

    if (size <= length + 2) return ellipsis(buf);
    else return snprintf(buf, size, "\"%s\"", expr->value);
}

static void deleteString(void * value) {
    ExprString * expr = value;
    free(expr->value);
}

static void * moveString(Region * dest, Region * src, void * value) {
    UNUSED(dest); UNUSED(src); UNUSED(value);

    return value;
}

static bool equalString(void * value1, void * value2) {
    ExprString * expr1 = value1, * expr2 = value2;
    return strcmp(expr1->value, expr2->value) == 0;
}

static ExprTagImpl exprStringImpl = {
    .eval   = evalNf,
    .apply  = applyThrowError,
    .show   = showString,
    .delete = deleteString,
    .move   = moveString,
    .equal  = equalString,
    .size   = sizeof(ExprString)
};

ExprTag exprStringTag;

void initStringTag(Region * region) {
    UNUSED(region);

    exprStringTag = newExprTag(exprStringImpl);
}