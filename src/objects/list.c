#include <string.h>
#include <stdio.h>

#include <common.h>
#include <array.h>

#include <objects/extern.h>
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


void * externList(Region * region, Array * xs) {
    void * retval = &exprNil;

    for (size_t i = 1; i <= xs->size; i++) {
        Expr * o = eval(region, getArray(xs, xs->size - i));
        if (o == NULL) return NULL;

        retval = newList(region, o, retval);
    }

    return retval;
}

void * externCar(Region * region, Array * xs) {
    if (xs->size != 1) return throw(TypeErrorTag, "expected 1 argument but %zu were given", xs->size);

    void * o = eval(region, getArray(xs, 0));
    if (o == NULL) return NULL;

    if (tagof(o) != exprListTag) {
        char * buf = show(o);
        throw(TypeErrorTag, "%s expected to be a list", buf);
        free(buf);

        return NULL;
    }

    ExprList * val = o; return val->car;
}

void * externCdr(Region * region, Array * xs) {
    if (xs->size != 1) return throw(TypeErrorTag, "expected 1 argument but %zu were given", xs->size);

    void * o = eval(region, getArray(xs, 0));
    if (o == NULL) return NULL;

    if (tagof(o) != exprListTag) {
        char * buf = show(o);
        throw(TypeErrorTag, "%s expected to be a list", buf);
        free(buf);

        return NULL;
    }

    ExprList * val = o; return val->cdr;
}

void * externCons(Region * region, Array * xs) {
    if (xs->size != 2) return throw(TypeErrorTag, "expected 2 arguments but %zu were given", xs->size);

    void * car = eval(region, getArray(xs, 0));
    if (car == NULL) return NULL;

    void * cdr = eval(region, getArray(xs, 1));
    if (cdr == NULL) return NULL;

    return newList(region, car, cdr);
}

void initListTag(Region * region) {
    exprListTag = newExprTag(exprListImpl);

    setVar(region->scope, "list", newExtern(region, externList));
    setVar(region->scope, "car",  newExtern(region, externCar));
    setVar(region->scope, "cdr",  newExtern(region, externCdr));
    setVar(region->scope, "cons", newExtern(region, externCons));
}