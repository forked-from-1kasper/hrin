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

    for (length = 0; tagof(expr) == &exprCCTag; length++)
        expr = expr->cdr;

    return length;
}

Array * expandList(void * value) {
    if (tagof(value) == &exprNilTag)
        return emptyArray();

    if (tagof(value) != &exprCCTag)
        return NULL;

    size_t size = lengthList(value);

    Array * retval = emptyArray();
    extendArray(retval, size);

    ExprCC * curr = (ExprCC *) value;
    for (size_t i = 0; i < size; i++) {
        setArray(retval, i, curr->car);
        curr = (ExprCC *) curr->cdr;
    }

    if (tagof(curr) == &exprNilTag) return retval;

    freeArray(retval);
    free(retval);
    return NULL;
}

static void * evalCC(Region * region, void * value) {
    ExprCC * expr = value;

    Expr * headval = eval(region, expr->car);
    IFNRET(headval);

    Array * argbuf = expandList(expr->cdr);

    if (argbuf == NULL) return throw(
        TypeErrorTag, "%s expected to be an argument list", showExpr(expr->cdr)
    );

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

static void * moveCC(Region * dest, Region * src, void * value) {
    UNUSED(src);

    ExprCC * expr = value; void * retptr = value;

    if (move(dest, expr->car) == NULL) retptr = NULL;
    if (move(dest, expr->cdr) == NULL) retptr = NULL;

    return retptr;
}

static bool equalCC(void * value1, void * value2) {
    ExprCC * expr1 = value1, * expr2 = value2;
    return value1 == value2 || (equal(expr1->car, expr2->car) && equal(expr2->cdr, expr2->cdr));
}

ExprTag exprCCTag = {
    .eval   = evalCC,
    .apply  = applyThrowError,
    .show   = showCC,
    .delete = deleteCC,
    .move   = moveCC,
    .equal  = equalCC,
    .size   = sizeof(ExprCC)
};

void * externList(Region * region, Array * xs) {
    void * retval = &exprNil;

    for (size_t i = 1; i <= xs->size; i++) {
        Expr * o = eval(region, getArray(xs, xs->size - i)); IFNRET(o);
        retval = newCC(region, o, retval);
    }

    return retval;
}

static inline void * throwTypeErrorCC(void * o)
{ return throw(TypeErrorTag, "%s expected to be a cons cell", showExpr(o)); }

static inline void * evalEnsureCC(Region * region, void * value) {
    void * o = eval(region, value); IFNRET(o);

    if (tagof(o) != &exprCCTag) return throwTypeErrorCC(o);

    return o;
}

void * externCar(Region * region, Array * xs) {
    ARITY(1, xs->size);

    void * o = eval(region, getArray(xs, 0)); IFNRET(o);

    if (tagof(o) == &exprNilTag)
        return &exprNil;
    else if (tagof(o) == &exprCCTag)
        return CC(o)->car;
    else
        return throwTypeErrorCC(o);
}

void * externCdr(Region * region, Array * xs) {
    ARITY(1, xs->size);

    void * o = eval(region, getArray(xs, 0)); IFNRET(o);

    if (tagof(o) == &exprNilTag)
        return &exprNil;
    else if (tagof(o) == &exprCCTag)
        return CC(o)->cdr;
    else
        return throwTypeErrorCC(o);
}

void * externCons(Region * region, Array * xs) {
    ARITY(2, xs->size);

    void * car = eval(region, getArray(xs, 0)); IFNRET(car);
    void * cdr = eval(region, getArray(xs, 1)); IFNRET(cdr);

    return newCC(region, car, cdr);
}

void * externSetcar(Region * region, Array * xs) {
    ARITY(2, xs->size);

    ExprCC * consval = evalEnsureCC(region, getArray(xs, 0));
    IFNRET(consval);

    void * car = eval(region, getArray(xs, 1));
    IFNRET(car);

    IFNRET(move(ownerof(consval), car));

    consval->car = car;
    return &exprNil;
}

void * externSetcdr(Region * region, Array * xs) {
    ARITY(2, xs->size);

    ExprCC * consval = evalEnsureCC(region, getArray(xs, 0));
    IFNRET(consval);

    void * cdr = eval(region, getArray(xs, 1));
    IFNRET(cdr);

    IFNRET(move(ownerof(consval), cdr));

    consval->cdr = cdr;
    return &exprNil;
}

void initCCTag(Region * region) {
    newExprImmortal(&exprTag, &exprCCTag, NULL);

    setVar(region->scope, "cc",      &exprCCTag);
    setVar(region->scope, "list",    newExtern(region, externList));
    setVar(region->scope, "car",     newExtern(region, externCar));
    setVar(region->scope, "cdr",     newExtern(region, externCdr));
    setVar(region->scope, "cons",    newExtern(region, externCons));
    setVar(region->scope, "setcar!", newExtern(region, externSetcar));
    setVar(region->scope, "setcdr!", newExtern(region, externSetcdr));
}
