#include <string.h>
#include <stdio.h>

#include <common.h>
#include <array.h>

#include <objects/extern.h>
#include <objects/nil.h>
#include <objects/cc.h>

size_t lengthList(void * value) {
    size_t length;

    ExprCC * expr = value;

    for (length = 0; tagof(expr) == exprCCTag; length++)
        expr = expr->cdr;

    return length;
}

Array * expandList(void * value) {
    if (tagof(value) == exprNilTag)
        return emptyArray();

    if (tagof(value) != exprCCTag)
        return NULL;

    size_t size = lengthList(value);

    Array * retval = emptyArray();
    extendArray(retval, size);

    ExprCC * curr = (ExprCC *) value;
    for (size_t i = 0; i < size; i++) {
        setArray(retval, i, curr->car);
        curr = (ExprCC *) curr->cdr;
    }

    if (tagof(curr) == exprNilTag) return retval;

    freeArray(retval);
    free(retval);
    return NULL;
}

static void * evalCC(Region * region, void * value) {
    ExprCC * expr = value;

    Expr * headval = eval(region, expr->car);
    if (headval == NULL) return NULL;

    Array * argbuf = expandList(expr->cdr);

    if (argbuf == NULL) return throw(TypeErrorTag, "%s expected to be an argument list", showExpr(expr->cdr));

    void * retval = apply(region, headval, argbuf);
    freeArray(argbuf); free(argbuf); return retval;
}

static size_t showCC(char * buf, size_t size, void * value) {
    if (size < sizeof("(XXX . YYY)")) return ellipsis(buf);

    size_t rem = size;

    *(buf++) = '('; rem--;

    ExprCC * expr = value;

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

static void deleteCC(void * value) {
    UNUSED(value);
}

static void moveCC(Region * dest, Region * src, void * value) {
    UNUSED(src);

    ExprCC * expr = value;

    move(dest, expr->car);
    move(dest, expr->cdr);
}

static bool equalCC(void * value1, void * value2) {
    ExprCC * expr1 = value1, * expr2 = value2;
    return value1 == value2 || (equal(expr1->car, expr2->car) && equal(expr2->cdr, expr2->cdr));
}

static ExprTagImpl exprCCImpl = {
    .eval   = evalCC,
    .apply  = applyThrowError,
    .show   = showCC,
    .delete = deleteCC,
    .move   = moveCC,
    .equal  = equalCC,
    .size   = sizeof(ExprCC)
};

ExprTag exprCCTag;

void * externList(Region * region, Array * xs) {
    void * retval = &exprNil;

    for (size_t i = 1; i <= xs->size; i++) {
        Expr * o = eval(region, getArray(xs, xs->size - i));
        if (o == NULL) return NULL;

        retval = newCC(region, o, retval);
    }

    return retval;
}

static inline void * evalEnsureCC(Region * region, void * value) {
    void * o = eval(region, value);
    if (o == NULL) return NULL;

    if (tagof(o) != exprCCTag) return throw(TypeErrorTag, "%s expected to be a cons cell", showExpr(o));

    return o;
}

void * externCar(Region * region, Array * xs) {
    ARITY(1, xs->size);

    ExprCC * consval = evalEnsureCC(region, getArray(xs, 0));
    if (consval == NULL) return NULL;

    return consval->car;
}

void * externCdr(Region * region, Array * xs) {
    ARITY(1, xs->size);

    ExprCC * consval = evalEnsureCC(region, getArray(xs, 0));
    if (consval == NULL) return NULL;

    return consval->cdr;
}

void * externCons(Region * region, Array * xs) {
    ARITY(2, xs->size);

    void * car = eval(region, getArray(xs, 0));
    if (car == NULL) return NULL;

    void * cdr = eval(region, getArray(xs, 1));
    if (cdr == NULL) return NULL;

    return newCC(region, car, cdr);
}

void * externSetcar(Region * region, Array * xs) {
    ARITY(2, xs->size);

    ExprCC * consval = evalEnsureCC(region, getArray(xs, 0));
    if (consval == NULL) return NULL;

    void * car = eval(region, getArray(xs, 1));
    if (car == NULL) return NULL;

    move(ownerof(consval), car);
    consval->car = car;
    return &exprNil;
}

void * externSetcdr(Region * region, Array * xs) {
    ARITY(2, xs->size);

    ExprCC * consval = evalEnsureCC(region, getArray(xs, 0));
    if (consval == NULL) return NULL;

    void * cdr = eval(region, getArray(xs, 1));
    if (cdr == NULL) return NULL;

    move(ownerof(consval), cdr);
    consval->cdr = cdr;
    return &exprNil;
}

void initCCTag(Region * region) {
    exprCCTag = newExprTag(exprCCImpl);

    setVar(region->scope, "list",    newExtern(region, externList));
    setVar(region->scope, "car",     newExtern(region, externCar));
    setVar(region->scope, "cdr",     newExtern(region, externCdr));
    setVar(region->scope, "cons",    newExtern(region, externCons));
    setVar(region->scope, "setcar!", newExtern(region, externSetcar));
    setVar(region->scope, "setcdr!", newExtern(region, externSetcdr));
}