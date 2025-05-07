#include <stdio.h>

#include <common.h>

#include <objects/boolean.h>
#include <objects/integer.h>
#include <objects/extern.h>

static size_t showInteger(char * buf, size_t size, void * value) {
    ExprInteger * expr = value;

    size_t length = snprintf(NULL, 0, "%lld", expr->value);
    if (size <= length) return ellipsis(buf);

    return snprintf(buf, size, "%lld", expr->value);
}

static void deleteInteger(void * value) {
    UNUSED(value);
}

static void * moveInteger(Region * dest, Region * src, void * value) {
    UNUSED(dest); UNUSED(src); UNUSED(value);

    return value;
}

static bool equalInteger(void * value1, void * value2) {
    ExprInteger * expr1 = value1, * expr2 = value2;
    return expr1->value == expr2->value;
}

ExprTag exprIntegerTag = {
    .eval   = evalNf,
    .apply  = applyThrowError,
    .show   = showInteger,
    .delete = deleteInteger,
    .move   = moveInteger,
    .equal  = equalInteger,
    .size   = sizeof(ExprInteger)
};

void * externAddi(Region * region, Array * xs) {
    Integer retval = 0;

    for (size_t i = 0; i < xs->size; i++) {
        ExprInteger * argval = evalEnsureInteger(region, getArray(xs, i)); IFNRET(argval);

        retval += argval->value;
    }

    return newInteger(region, retval);
}

void * externSubi(Region * region, Array * xs) {
    ARITY(2, xs->size);

    ExprInteger * i1 = evalEnsureInteger(region, getArray(xs, 0)); IFNRET(i1);
    ExprInteger * i2 = evalEnsureInteger(region, getArray(xs, 1)); IFNRET(i2);

    return newInteger(region, i1->value - i2->value);
}

void * externMuli(Region * region, Array * xs) {
    Integer retval = 1;

    for (size_t i = 0; i < xs->size; i++) {
        ExprInteger * argval = evalEnsureInteger(region, getArray(xs, i)); IFNRET(argval);

        retval *= argval->value;
    }

    return newInteger(region, retval);
}

void * externNegi(Region * region, Array * xs) {
    ARITY(1, xs->size);

    ExprInteger * argval = evalEnsureInteger(region, getArray(xs, 0));
    IFNRET(argval);

    return newInteger(region, -argval->value);
}

void * externMaxi(Region * region, Array * xs) {
    Integer retval = INTEGER_MIN;

    for (size_t i = 0; i < xs->size; i++) {
        ExprInteger * argval = evalEnsureInteger(region, getArray(xs, i)); IFNRET(argval);

        retval = max(retval, argval->value);
    }

    return newInteger(region, retval);
}

void * externMini(Region * region, Array * xs) {
    Integer retval = INTEGER_MAX;

    for (size_t i = 0; i < xs->size; i++) {
        ExprInteger * argval = evalEnsureInteger(region, getArray(xs, i)); IFNRET(argval);

        retval = min(retval, argval->value);
    }

    return newInteger(region, retval);
}

void * externDivi(Region * region, Array * xs) {
    ARITY(2, xs->size);

    ExprInteger * i1 = evalEnsureInteger(region, getArray(xs, 0)); IFNRET(i1);
    ExprInteger * i2 = evalEnsureInteger(region, getArray(xs, 1)); IFNRET(i2);

    return newInteger(region, i1->value / i2->value);
}

void * externModi(Region * region, Array * xs) {
    ARITY(2, xs->size);

    ExprInteger * i1 = evalEnsureInteger(region, getArray(xs, 0)); IFNRET(i1);
    ExprInteger * i2 = evalEnsureInteger(region, getArray(xs, 1)); IFNRET(i2);

    return newInteger(region, i1->value % i2->value);
}

void * externAndi(Region * region, Array * xs) {
    ARITY(2, xs->size);

    ExprInteger * i1 = evalEnsureInteger(region, getArray(xs, 0)); IFNRET(i1);
    ExprInteger * i2 = evalEnsureInteger(region, getArray(xs, 1)); IFNRET(i2);

    return newInteger(region, i1->value & i2->value);
}

void * externOri(Region * region, Array * xs) {
    ARITY(2, xs->size);

    ExprInteger * i1 = evalEnsureInteger(region, getArray(xs, 0)); IFNRET(i1);
    ExprInteger * i2 = evalEnsureInteger(region, getArray(xs, 1)); IFNRET(i2);

    return newInteger(region, i1->value | i2->value);
}

void * externXori(Region * region, Array * xs) {
    ARITY(2, xs->size);

    ExprInteger * i1 = evalEnsureInteger(region, getArray(xs, 0)); IFNRET(i1);
    ExprInteger * i2 = evalEnsureInteger(region, getArray(xs, 1)); IFNRET(i2);

    return newInteger(region, i1->value ^ i2->value);
}

void * externLei(Region * region, Array * xs) {
    ARITY(2, xs->size);

    ExprInteger * i1 = evalEnsureInteger(region, getArray(xs, 0)); IFNRET(i1);
    ExprInteger * i2 = evalEnsureInteger(region, getArray(xs, 1)); IFNRET(i2);

    return newBool(i1->value <= i2->value);
}

void * externLti(Region * region, Array * xs) {
    ARITY(2, xs->size);

    ExprInteger * i1 = evalEnsureInteger(region, getArray(xs, 0)); IFNRET(i1);
    ExprInteger * i2 = evalEnsureInteger(region, getArray(xs, 1)); IFNRET(i2);

    return newBool(i1->value < i2->value);
}

void * externGei(Region * region, Array * xs) {
    ARITY(2, xs->size);

    ExprInteger * i1 = evalEnsureInteger(region, getArray(xs, 0)); IFNRET(i1);
    ExprInteger * i2 = evalEnsureInteger(region, getArray(xs, 1)); IFNRET(i2);

    return newBool(i1->value >= i2->value);
}

void * externGti(Region * region, Array * xs) {
    ARITY(2, xs->size);

    ExprInteger * i1 = evalEnsureInteger(region, getArray(xs, 0)); IFNRET(i1);
    ExprInteger * i2 = evalEnsureInteger(region, getArray(xs, 1)); IFNRET(i2);

    return newBool(i1->value > i2->value);
}

void initIntegerTag(Region * region) {
    newExprImmortal(&exprTag, &exprIntegerTag, NULL);

    setVar(region->scope, "addi", newExtern(region, externAddi));
    setVar(region->scope, "subi", newExtern(region, externSubi));
    setVar(region->scope, "maxi", newExtern(region, externMaxi));
    setVar(region->scope, "mini", newExtern(region, externMini));
    setVar(region->scope, "muli", newExtern(region, externMuli));
    setVar(region->scope, "divi", newExtern(region, externDivi));
    setVar(region->scope, "modi", newExtern(region, externModi));
    setVar(region->scope, "negi", newExtern(region, externNegi));
    setVar(region->scope, "andi", newExtern(region, externAndi));
    setVar(region->scope, "ori",  newExtern(region, externOri));
    setVar(region->scope, "xori", newExtern(region, externXori));
    setVar(region->scope, "lei",  newExtern(region, externLei));
    setVar(region->scope, "lti",  newExtern(region, externLti));
    setVar(region->scope, "gei",  newExtern(region, externGei));
    setVar(region->scope, "gti",  newExtern(region, externGti));
}
