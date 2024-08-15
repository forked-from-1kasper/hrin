#include <stdio.h>

#include <common.h>

#include <objects/integer.h>
#include <objects/extern.h>

static char * showInteger(void * value) {
    ExprInteger * expr = value;

    char * retbuf = malloc(21);
    sprintf(retbuf, "%lld", expr->value);

    return retbuf;
}

static void deleteInteger(void * value) {
    UNUSED(value);
}

static void moveInteger(Region * dest, Region * src, void * value) {
    UNUSED(dest); UNUSED(src); UNUSED(value);
}

static bool equalInteger(void * value1, void * value2) {
    ExprInteger * expr1 = value1, * expr2 = value2;
    return expr1->value == expr2->value;
}

ExprTagImpl exprIntegerImpl = {
    .eval   = evalNf,
    .apply  = applyThrowError,
    .show   = showInteger,
    .delete = deleteInteger,
    .move   = moveInteger,
    .equal  = equalInteger,
    .size   = sizeof(ExprInteger)
};

ExprTag exprIntegerTag;

void * externAddi(Region * region, Array * xs) {
    Integer intval = 0;

    for (size_t i = 0; i < xs->size; i++) {
        Expr * argval = eval(region, getArray(xs, i));
        if (argval == NULL) return NULL;

        if (tagof(argval) == exprIntegerTag)
            intval += ((ExprInteger *) argval)->value;
        else {
            char * argvalbuf = show(argval);
            throw(TypeErrorTag, "%s expected to be an integer", argvalbuf);
            free(argvalbuf);

            return NULL;
        }
    }

    return newInteger(region, intval);
}

void * externMuli(Region * region, Array * xs) {
    Integer intval = 1;

    for (size_t i = 0; i < xs->size; i++) {
        Expr * argval = eval(region, getArray(xs, i));
        if (argval == NULL) return NULL;

        if (tagof(argval) == exprIntegerTag)
            intval *= ((ExprInteger *) argval)->value;
        else {
            char * argvalbuf = show(argval);
            throw(TypeErrorTag, "%s expected to be an integer", argvalbuf);
            free(argvalbuf);

            return NULL;
        }
    }

    return newInteger(region, intval);
}

void * externNegi(Region * region, Array * xs) {
    if (xs->size != 1) return throw(TypeErrorTag, "expected 1 argument but %zu were given", xs->size);

    void * o = eval(region, getArray(xs, 0));
    if (tagof(o) == exprIntegerTag) {
        return newInteger(region, -((ExprInteger *) o)->value);
    } else {
        char * buf = show(o);
        throw(TypeErrorTag, "%s expected to be an integer", buf);
        free(buf);

        return NULL;
    }
}

void initIntegerTag(Region * region) {
    exprIntegerTag = newExprTag(exprIntegerImpl);

    setVar(region->scope, "addi", newExtern(region, externAddi));
    setVar(region->scope, "muli", newExtern(region, externMuli));
    setVar(region->scope, "negi", newExtern(region, externNegi));
}