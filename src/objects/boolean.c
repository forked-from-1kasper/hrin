#include <common.h>

#include <objects/boolean.h>
#include <objects/extern.h>

static char * showBoolean(void * value) {
    ExprBoolean * expr = value;
    return dup(expr->value ? "true" : "false");
}

static void deleteBoolean(void * value) {
    UNUSED(value);
}

static void moveBoolean(Region * dest, Region * src, void * value) {
    UNUSED(dest); UNUSED(src); UNUSED(value);
}

ExprTagImpl exprBooleanImpl = {
    .eval   = evalNf,
    .apply  = applyThrowError,
    .show   = showBoolean,
    .delete = deleteBoolean,
    .move   = moveBoolean,
    .size   = sizeof(ExprBoolean)
};

ExprTag exprBooleanTag;
ExprBoolean exprTrue = {.value = true}, exprFalse = {.value = false};

void initBooleanTag(Region * region) {
    exprBooleanTag = newExprTag(exprBooleanImpl);

    newExprImmortal(exprBooleanTag, &exprTrue, &exprFalse, NULL);

    setVar(region->scope, "true",  &exprTrue);
    setVar(region->scope, "false", &exprFalse);
}
