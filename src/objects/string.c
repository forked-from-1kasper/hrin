#include <stdio.h>

#include <common.h>

#include <objects/integer.h>
#include <objects/string.h>
#include <objects/byte.h>

static void * applyString(Region * region, void * o, Array * xs) {
    ARITY(1, xs->size);

    ExprInteger * i = evalEnsureInteger(region, getArray(xs, 0)); IFNRET(i);
    return newByte(region, STRING(o)->value[i->value]); // TODO: bounds check
}

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

static bool equalString(void * value1, void * value2) {
    ExprString * expr1 = value1, * expr2 = value2;
    return strcmp(expr1->value, expr2->value) == 0;
}

ExprTag exprStringTag = {
    .eval   = trivEval,
    .apply  = applyString,
    .show   = showString,
    .delete = deleteString,
    .move   = trivMove,
    .equal  = equalString,
    .size   = sizeof(ExprString)
};

void initStringTag(Region * region) {
    UNUSED(region);

    newExprImmortal(&exprTag, &exprStringTag, NULL);
    setVar(region->scope, "string", &exprStringTag);
}
