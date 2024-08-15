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

void * externNot(Region * region, Array * xs) {
    if (xs->size != 1) return throw(TypeErrorTag, "expected 1 argument but %zu were given", xs->size);

    void * o = eval(region, getArray(xs, 0));
    if (o == NULL) return NULL;

    if (o == &exprFalse) return &exprTrue;
    if (o == &exprTrue)  return &exprFalse;

    char * buf = show(o);
    throw(TypeErrorTag, "%s expected to be a boolean", buf);
    free(buf);

    return NULL;
}

void * externAndalso(Region * region, Array * xs) {
    for (size_t i = 0; i < xs->size; i++) {
        void * o = eval(region, getArray(xs, i));
        if (o == NULL)       return NULL;
        if (o == &exprFalse) return &exprFalse;
    }

    return &exprTrue;
}

void * externOrelse(Region * region, Array * xs) {
    for (size_t i = 0; i < xs->size; i++) {
        void * o = eval(region, getArray(xs, i));
        if (o == NULL)      return NULL;
        if (o == &exprTrue) return &exprTrue;
    }

    return &exprFalse;
}

void * externIte(Region * region, Array * xs) {
    if (xs->size != 3) return throw(TypeErrorTag, "expected 3 arguments but %zu were given", xs->size);

    void * b = eval(region, getArray(xs, 0));

    if (b == &exprTrue)  return eval(region, getArray(xs, 1));
    if (b == &exprFalse) return eval(region, getArray(xs, 2));

    char * buf = show(b);
    throw(TypeErrorTag, "%s expected to be a boolean", buf);
    free(buf);

    return NULL;
}

void initBooleanTag(Region * region) {
    exprBooleanTag = newExprTag(exprBooleanImpl);

    newExprImmortal(exprBooleanTag, &exprTrue, &exprFalse, NULL);

    setVar(region->scope, "true",    &exprTrue);
    setVar(region->scope, "false",   &exprFalse);
    setVar(region->scope, "not",     newExtern(region, externNot));
    setVar(region->scope, "andalso", newExtern(region, externAndalso));
    setVar(region->scope, "orelse",  newExtern(region, externOrelse));
    setVar(region->scope, "ite",     newExtern(region, externIte));
}
