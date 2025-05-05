#include <common.h>

#include <objects/atom.h>

static void * evalAtom(Region * region, void * value) {
    ExprAtom * expr = value;

    Expr * o = getVar(region->scope, expr->value);
    return o == NULL ? throw(NameErrorTag, "%s is undefined", expr->value) : o;
}

static size_t showAtom(char * buf, size_t size, void * value) {
    ExprAtom * expr = value;

    size_t length = strlen(expr->value);
    if (size <= length) return ellipsis(buf);

    strcpy(buf, expr->value); return length;
}

static void deleteAtom(void * value) {
    ExprAtom * expr = value;
    free(expr->value);
}

static void * moveAtom(Region * dest, Region * src, void * value) {
    UNUSED(dest); UNUSED(src); UNUSED(value);

    return value;
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
