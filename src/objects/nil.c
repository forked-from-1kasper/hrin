#include <common.h>

#include <objects/nil.h>

static size_t showNil(char * buf, size_t size, void * value) {
    UNUSED(value); UNUSED(size);
    strcpy(buf, "nil"); return 3;
}

static void deleteNil(void * value) {
    UNUSED(value);
}

static void * moveNil(Region * dest, Region * src, void * value) {
    UNUSED(dest); UNUSED(src); UNUSED(value);

    return value;
}

static ExprTagImpl exprNilImpl = {
    .eval   = evalNf,
    .apply  = applyThrowError,
    .show   = showNil,
    .delete = deleteNil,
    .move   = moveNil,
    .equal  = equalByRef,
    .size   = sizeof(ExprNil)
};

ExprTag exprNilTag;
ExprNil exprNil;

void initNilTag(Region * region) {
    exprNilTag = newExprTag(exprNilImpl);

    newExprImmortal(exprNilTag, &exprNil, NULL);
    setVar(region->scope, "nil", &exprNil);
}