#include <common.h>

#include <objects/nil.h>

static char * showNil(void * value) {
    UNUSED(value);
    return dup("nil");
}

static void deleteNil(void * value) {
    UNUSED(value);
}

static void moveNil(Region * dest, Region * src, void * value) {
    UNUSED(dest); UNUSED(src); UNUSED(value);
}

ExprTagImpl exprNilImpl = {
    .eval   = evalNf,
    .apply  = applyThrowError,
    .show   = showNil,
    .delete = deleteNil,
    .move   = moveNil,
    .size   = sizeof(ExprNil)
};

ExprTag exprNilTag;
ExprNil exprNil;