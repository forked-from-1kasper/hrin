#include <common.h>

#include <objects/nil.h>

static size_t showNil(char * buf, size_t size, void * value) {
    UNUSED(value); UNUSED(size);
    strcpy(buf, "nil"); return 3;
}

ExprTag exprNilTag = {
    .eval   = trivEval,
    .apply  = applyThrowError,
    .show   = showNil,
    .delete = trivDelete,
    .move   = trivMove,
    .equal  = equalByRef,
    .size   = 0
};

ExprNil exprNil;

void initNilTag(Region * region) {
    newExprImmortal(&exprTag, &exprNilTag, NULL);
    newExprImmortal(&exprNilTag, &exprNil, NULL);
    setVar(region->scope, "nil", &exprNil);
}
