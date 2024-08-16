#include <common.h>

#include <objects/ident.h>

static void * evalIdent(Region * region, void * value) {
    ExprIdent * expr = value;

    Expr * o = getVar(region->scope, expr->value);
    return o == NULL ? throw(NameErrorTag, "%s is undefined", expr->value) : o;
}

static char * showIdent(void * value) {
    ExprIdent * expr = value;
    return dup(expr->value);
}

static void deleteIdent(void * value) {
    ExprIdent * expr = value;
    free(expr->value);
}

static void moveIdent(Region * dest, Region * src, void * value) {
    UNUSED(dest); UNUSED(src); UNUSED(value);
}

static bool equalIdent(void * value1, void * value2) {
    ExprIdent * expr1 = value1, * expr2 = value2;
    return strcmp(expr1->value, expr2->value) == 0;
}

static ExprTagImpl exprIdentImpl = {
    .eval   = evalIdent,
    .apply  = applyThrowError,
    .show   = showIdent,
    .delete = deleteIdent,
    .move   = moveIdent,
    .equal  = equalIdent,
    .size   = sizeof(ExprIdent)
};

ExprTag exprIdentTag;

void initIdentTag(Region * region) {
    UNUSED(region);

    exprIdentTag = newExprTag(exprIdentImpl);
}