#include <common.h>

#include <objects/atom.h>

static void * evalAtom(Region * region, void * value) {
    ExprAtom * expr = value;

    Expr * o = getVar(region->scope, expr->value);
    return o == NULL ? throw(NameErrorTag, "%s is undefined", expr->value) : o;
}

static char * showAtom(void * value) {
    ExprAtom * expr = value;
    return dup(expr->value);
}

static void deleteAtom(void * value) {
    ExprAtom * expr = value;
    free(expr->value);
}

static void moveAtom(Region * dest, Region * src, void * value) {
    UNUSED(dest); UNUSED(src); UNUSED(value);
}

static bool equalAtom(void * value1, void * value2) {
    ExprAtom * expr1 = value1, * expr2 = value2;
    return strcmp(expr1->value, expr2->value) == 0;
}

static ExprTagImpl exprAtomImpl = {
    .eval   = evalAtom,
    .apply  = applyThrowError,
    .show   = showAtom,
    .delete = deleteAtom,
    .move   = moveAtom,
    .equal  = equalAtom,
    .size   = sizeof(ExprAtom)
};

ExprTag exprAtomTag;

void initAtomTag(Region * region) {
    UNUSED(region);

    exprAtomTag = newExprTag(exprAtomImpl);
}