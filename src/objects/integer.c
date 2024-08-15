#include <stdio.h>

#include <common.h>

#include <objects/integer.h>
#include <objects/extern.h>

static char * showInteger(void * value) {
    ExprInteger * expr = value;

    char * retbuf = malloc(12);
    sprintf(retbuf, "%lld", expr->value);

    return retbuf;
}

static void deleteInteger(void * value) {
    UNUSED(value);
}

static void moveInteger(Region * dest, Region * src, void * value) {
    UNUSED(dest); UNUSED(src); UNUSED(value);
}

ExprTagImpl exprIntegerImpl = {
    .eval   = evalNf,
    .apply  = applyThrowError,
    .show   = showInteger,
    .delete = deleteInteger,
    .move   = moveInteger,
    .size   = sizeof(ExprInteger)
};

ExprTag exprIntegerTag;

void * externAdd(Region * region, Array * xs) {
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

void initIntegerTag(Region * region) {
    exprIntegerTag = newExprTag(exprIntegerImpl);

    setVar(region->scope, "add", newExtern(region, externAdd));
}