#include <string.h>
#include <stdio.h>

#include <common.h>
#include <array.h>

#include <objects/list.h>
#include <objects/nil.h>

size_t lengthList(void * value) {
    size_t length;

    ExprList * expr = value;

    for (length = 0; tagof(expr) == exprListTag; length++)
        expr = expr->cdr;

    return length;
}

static void * evalList(Region * region, void * value) {
    ExprList * expr = value;

    Expr * headval = eval(region, expr->car);
    if (headval == NULL) return NULL;

    size_t argn = lengthList(expr->cdr);
    Array argbuf = newArray(argn);

    extendArray(&argbuf, argn);

    ExprList * curr = (ExprList *) expr->cdr;

    for (size_t i = 0; i < argn; i++) {
        setArray(&argbuf, i, curr->car);
        curr = (ExprList *) curr->cdr;
    }

    Expr * retval = NULL;

    if (tagof(curr) != exprNilTag) goto finally;

    Array argarr = sliceArray(&argbuf, 0, argn);
    retval = apply(region, headval, &argarr);

    finally: freeArray(&argbuf); return retval;
}

static char * showList(void * value) {
    ExprList * expr = value;

    char * car = show(expr->car);
    char * cdr = show(expr->cdr);

    size_t length = strlen(car) + strlen(cdr) + 5;
    char * retbuf = malloc(length + 1);
    if (retbuf == NULL) return NULL;

    sprintf(retbuf, "(%s . %s)", car, cdr);

    free(car); free(cdr); return retbuf;
}

static void deleteList(void * value) {
    UNUSED(value);
}

static void moveList(Region * dest, Region * src, void * value) {
    UNUSED(src);

    ExprList * expr = value;

    move(dest, expr->car);
    move(dest, expr->cdr);
}

ExprTagImpl exprListImpl = {
    .eval   = evalList,
    .apply  = applyThrowError,
    .show   = showList,
    .delete = deleteList,
    .move   = moveList,
    .size   = sizeof(ExprList)
};

ExprTag exprListTag;