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

Array * expandList(void * value) {
    if (tagof(value) == exprNilTag)
        return emptyArray();

    if (tagof(value) != exprListTag)
        return NULL;

    size_t size = lengthList(value);

    Array * retval = emptyArray();
    extendArray(retval, size);

    ExprList * curr = (ExprList *) value;
    for (size_t i = 0; i < size; i++) {
        setArray(retval, i, curr->car);
        curr = (ExprList *) curr->cdr;
    }

    if (tagof(curr) == exprNilTag) return retval;

    freeArray(retval);
    free(retval);
    return NULL;
}

static void * evalList(Region * region, void * value) {
    ExprList * expr = value;

    Expr * headval = eval(region, expr->car);
    if (headval == NULL) return NULL;

    Array * argbuf = expandList(expr->cdr);

    if (argbuf == NULL) return throw(TypeErrorTag, "%s expected to be an argument list", showExpr(expr->cdr));

    void * retval = apply(region, headval, argbuf);
    freeArray(argbuf); free(argbuf); return retval;
}

static size_t showList(char * buf, size_t size, void * value) {
    if (size < sizeof("(XXX . YYY)")) return ellipsis(buf);

    size_t rem = size;

    *(buf++) = '('; rem--;

    ExprList * expr = value;

    size_t carlen = show(buf, rem - sizeof(". YYY)"), expr->car);
    buf += carlen; rem -= carlen;

    *(buf++) = ' '; rem--;
    *(buf++) = '.'; rem--;
    *(buf++) = ' '; rem--;

    size_t cdrlen = show(buf, rem - 1, expr->cdr);
    buf += cdrlen; rem -= cdrlen;

    *(buf++) = ')'; rem--;

    *buf = '\0'; return size - rem;
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

static bool equalList(void * value1, void * value2) {
    ExprList * expr1 = value1, * expr2 = value2;
    return value1 == value2 || (equal(expr1->car, expr2->car) && equal(expr2->cdr, expr2->cdr));
}

static ExprTagImpl exprListImpl = {
    .eval   = evalList,
    .apply  = applyThrowError,
    .show   = showList,
    .delete = deleteList,
    .move   = moveList,
    .equal  = equalList,
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

static inline void * evalEnsureList(Region * region, void * value) {
    void * o = eval(region, value);
    if (o == NULL) return NULL;

    if (tagof(o) != exprListTag) return throw(TypeErrorTag, "%s expected to be a list", showExpr(o));

    return o;
}

void * externCar(Region * region, Array * xs) {
    if (xs->size != 1) return throw(TypeErrorTag, "expected 1 argument but %zu were given", xs->size);

    ExprList * listval = evalEnsureList(region, getArray(xs, 0));
    if (listval == NULL) return NULL;

    return listval->car;
}

void * externCdr(Region * region, Array * xs) {
    if (xs->size != 1) return throw(TypeErrorTag, "expected 1 argument but %zu were given", xs->size);

    ExprList * listval = evalEnsureList(region, getArray(xs, 0));
    if (listval == NULL) return NULL;

    return listval->cdr;
}

void * externCons(Region * region, Array * xs) {
    if (xs->size != 2) return throw(TypeErrorTag, "expected 2 arguments but %zu were given", xs->size);

    void * car = eval(region, getArray(xs, 0));
    if (car == NULL) return NULL;

    void * cdr = eval(region, getArray(xs, 1));
    if (cdr == NULL) return NULL;

    return newList(region, car, cdr);
}

void * externSetcar(Region * region, Array * xs) {
    if (xs->size != 2) return throw(TypeErrorTag, "expected 2 arguments but %zu were given", xs->size);

    ExprList * listval = evalEnsureList(region, getArray(xs, 0));
    if (listval == NULL) return NULL;

    void * car = eval(region, getArray(xs, 1));
    if (car == NULL) return NULL;

    move(ownerof(listval), car);
    listval->car = car;
    return &exprNil;
}

void * externSetcdr(Region * region, Array * xs) {
    if (xs->size != 2) return throw(TypeErrorTag, "expected 2 arguments but %zu were given", xs->size);

    ExprList * listval = evalEnsureList(region, getArray(xs, 0));
    if (listval == NULL) return NULL;

    void * cdr = eval(region, getArray(xs, 1));
    if (cdr == NULL) return NULL;

    move(ownerof(listval), cdr);
    listval->cdr = cdr;
    return &exprNil;
}

void initListTag(Region * region) {
    exprListTag = newExprTag(exprListImpl);

    setVar(region->scope, "list",    newExtern(region, externList));
    setVar(region->scope, "car",     newExtern(region, externCar));
    setVar(region->scope, "cdr",     newExtern(region, externCdr));
    setVar(region->scope, "cons",    newExtern(region, externCons));
    setVar(region->scope, "setcar!", newExtern(region, externSetcar));
    setVar(region->scope, "setcdr!", newExtern(region, externSetcdr));
}