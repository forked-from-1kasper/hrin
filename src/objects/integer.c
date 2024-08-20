#include <stdio.h>

#include <common.h>

#include <objects/integer.h>
#include <objects/extern.h>

static size_t showInteger(char * buf, size_t size, void * value) {
    ExprInteger * expr = value;

    size_t length = snprintf(NULL, 0, "%lld", expr->value);
    if (size <= length) return ellipsis(buf);

    return snprintf(buf, size, "%lld", expr->value);
}

static void deleteInteger(void * value) {
    UNUSED(value);
}

static void moveInteger(Region * dest, Region * src, void * value) {
    UNUSED(dest); UNUSED(src); UNUSED(value);
}

static bool equalInteger(void * value1, void * value2) {
    ExprInteger * expr1 = value1, * expr2 = value2;
    return expr1->value == expr2->value;
}

static ExprTagImpl exprIntegerImpl = {
    .eval   = evalNf,
    .apply  = applyThrowError,
    .show   = showInteger,
    .delete = deleteInteger,
    .move   = moveInteger,
    .equal  = equalInteger,
    .size   = sizeof(ExprInteger)
};

ExprTag exprIntegerTag;

static inline void * evalEnsureInteger(Region * region, void * value) {
    void * o = eval(region, value);
    if (o == NULL) return NULL;

    if (tagof(o) != exprIntegerTag) return throw(TypeErrorTag, "%s expected to be an integer", showExpr(o));

    return o;
}

void * externAddi(Region * region, Array * xs) {
    Integer intval = 0;

    for (size_t i = 0; i < xs->size; i++) {
        ExprInteger * argval = evalEnsureInteger(region, getArray(xs, i));
        if (argval == NULL) return NULL;

        intval += argval->value;
    }

    return newInteger(region, intval);
}

void * externMuli(Region * region, Array * xs) {
    Integer intval = 1;

    for (size_t i = 0; i < xs->size; i++) {
        ExprInteger * argval = evalEnsureInteger(region, getArray(xs, i));
        if (argval == NULL) return NULL;

        intval *= argval->value;
    }

    return newInteger(region, intval);
}

void * externNegi(Region * region, Array * xs) {
    if (xs->size != 1) return throw(TypeErrorTag, "expected 1 argument but %zu were given", xs->size);

    ExprInteger * argval = evalEnsureInteger(region, getArray(xs, 0));
    if (argval == NULL) return NULL;

    return newInteger(region, -argval->value);
}

void initIntegerTag(Region * region) {
    exprIntegerTag = newExprTag(exprIntegerImpl);

    setVar(region->scope, "addi", newExtern(region, externAddi));
    setVar(region->scope, "muli", newExtern(region, externMuli));
    setVar(region->scope, "negi", newExtern(region, externNegi));
}