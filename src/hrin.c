#include <stdlib.h>
#include <stdio.h>

#include <common.h>
#include <error.h>
#include <expr.h>

#include <objects/boolean.h>
#include <objects/integer.h>
#include <objects/extern.h>
#include <objects/string.h>
#include <objects/lambda.h>
#include <objects/ident.h>
#include <objects/list.h>
#include <objects/nil.h>

#include <lexer.h>
#include <parser.h>

void * externAdd(Region * region, Array * xs) {
    int intval = 0;

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

Region * rootRegion = NULL;

void * externDefine(Region * region, Array * xs) {
    if (xs->size != 2) return throw(TypeErrorTag, "expected 2 arguments but %zu were given", xs->size);
    ExprIdent * i = getArray(xs, 0);

    if (tagof(i) != exprIdentTag) {
        char * ibuf = show(i);
        throw(TypeErrorTag, "%s expected to be an ident", ibuf);
        free(ibuf);

        return NULL;
    }

    Expr * o = eval(region, getArray(xs, 1));
    if (o == NULL) return NULL;

    move(rootRegion, o);
    setVar(rootRegion->scope, i->value, o);

    return &exprNil;
}

void * externDeflocal(Region * region, Array * xs) {
    if (xs->size != 2) return throw(TypeErrorTag, "expected 2 arguments but %zu were given", xs->size);
    ExprIdent * i = getArray(xs, 0);

    if (tagof(i) != exprIdentTag) {
        char * ibuf = show(i);
        throw(TypeErrorTag, "%s expected to be an ident", ibuf);
        free(ibuf);

        return NULL;
    }

    Expr * o = eval(region, getArray(xs, 1));
    if (o == NULL) return NULL;

    setVar(region->scope, i->value, o);

    return &exprNil;
}

void * externProgn(Region * region, Array * xs) {
    Expr * retval = NULL;

    for (size_t i = 0; i < xs->size; i++) {
        retval = eval(region, getArray(xs, i));
        if (retval == NULL) return NULL;
    }

    return retval;
}

void * externLambda(Region * region, Array * xs) {
    if (xs->size <= 0) return throw(TypeErrorTag, "no arguments were given");

    for (size_t i = 0; i < xs->size - 1; i++) {
        if (tagof(getArray(xs, i)) != exprIdentTag) {
            char * buf = show(getArray(xs, i));
            throw(TypeErrorTag, "%s expected to be an ident", buf);
            free(buf);

            return NULL;
        }
    }

    ExprLambda * retval = newExpr(region, exprLambdaTag);

    Array vars = newArray(xs->size - 1);

    for (size_t j = 0; j < xs->size - 1; j++) {
        ExprIdent * i = getArray(xs, j);
        setArray(&vars, j, dup(i->value));
    }

    retval->scope = newScope(rootRegion->scope); // TODO
    retval->vars  = vars;
    retval->value = getArray(xs, xs->size - 1);

    Scope * curr = region->scope;
    while (curr != NULL) {
        if (curr->lexical)
            copyScope(retval->scope, curr);

        curr = curr->next;
    }

    return retval;
}

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

void * externQuote(Region * region, Array * xs) {
    UNUSED(region);

    if (xs->size != 1) return throw(TypeErrorTag, "expected 1 argument but %zu were given", xs->size);
    return getArray(xs, 0);
}

void * externEval(Region * region, Array * xs) {
    if (xs->size != 1) return throw(TypeErrorTag, "expected 1 argument but %zu were given", xs->size);

    Expr * o = eval(region, getArray(xs, 0));
    if (o == NULL) return NULL;

    return eval(region, o);
}

ErrorTag printError() {
    ErrorTag error = getThrownError();
    if (error == NULL) return NULL;

    const char * errorBuffer = getErrorBuffer();

    if (strlen(errorBuffer) > 0) printf("!!! %s: %s\n", error, errorBuffer);
    else printf("!!! %s\n", error);

    return error;
}

void scanModule(FILE * file) {
    Region * moduleRegion = newRegion(rootRegion);
    moduleRegion->scope = newScope(rootRegion->scope);

    for (;;) {
        Expr * e1 = takeExprToplevel(moduleRegion, file);
        if (e1 == NULL) { printError(); goto finally; }

        Expr * e2 = eval(moduleRegion, e1);
        if (e2 == NULL) { printError(); goto finally; }
    }

    finally:
    deleteScope(moduleRegion->scope);
    deleteRegion(moduleRegion);
}

ErrorTag scanLine(FILE * file) {
    ErrorTag retval = NULL;

    Region * region = newRegion(rootRegion);
    region->scope = newScope(rootRegion->scope);

    Expr * e1 = takeExprToplevel(region, file);
    if (e1 == NULL) { retval = printError(); goto finally; }

    {
        char * buf = show(e1);
        printf(">>> %s\n", buf);
        free(buf);
    }

    Expr * e2 = eval(region, e1);
    if (e2 == NULL) { retval = printError(); goto finally; }

    {
        char * buf = show(e2);
        printf("<<< %s\n", buf);
        free(buf);
    }

    finally:
    deleteScope(region->scope);
    deleteRegion(region);

    return retval;
}

int main(int argc, char * argv[]) {
    exprIdentTag   = newExprTag(exprIdentImpl);
    exprListTag    = newExprTag(exprListImpl);
    exprExternTag  = newExprTag(exprExternImpl);
    exprIntegerTag = newExprTag(exprIntegerImpl);
    exprStringTag  = newExprTag(exprStringImpl);
    exprLambdaTag  = newExprTag(exprLambdaImpl);
    exprNilTag     = newExprTag(exprNilImpl);

    newExprImmortal(exprNilTag, &exprNil, NULL);

    Scope * globalScope = newScope(NULL);
    globalScope->lexical = false;

    rootRegion = newRegion(NULL);
    rootRegion->scope = globalScope;

    setVar(rootRegion->scope, "nil",      &exprNil);
    setVar(rootRegion->scope, "add",      newExtern(rootRegion, externAdd));
    setVar(rootRegion->scope, "define",   newExtern(rootRegion, externDefine));
    setVar(rootRegion->scope, "progn",    newExtern(rootRegion, externProgn));
    setVar(rootRegion->scope, "deflocal", newExtern(rootRegion, externDeflocal));
    setVar(rootRegion->scope, "λ",        newExtern(rootRegion, externLambda));
    setVar(rootRegion->scope, "list",     newExtern(rootRegion, externList));
    setVar(rootRegion->scope, "car",      newExtern(rootRegion, externCar));
    setVar(rootRegion->scope, "cdr",      newExtern(rootRegion, externCdr));
    setVar(rootRegion->scope, "cons",     newExtern(rootRegion, externCons));
    setVar(rootRegion->scope, "quote",    newExtern(rootRegion, externQuote));
    setVar(rootRegion->scope, "eval",     newExtern(rootRegion, externEval));

    initBooleanTag(rootRegion);

    for (int i = 1; i < argc; i++) {
        FILE * fin = fopen(argv[i], "r");
        if (fin == NULL) printf("Cannot open “%s”\n", argv[i]);
        else { scanModule(fin); fclose(fin); }
    }

    while (scanLine(stdin) != EOFErrorTag);

    deleteRegion(rootRegion);
    deleteScope(globalScope);

    deallocExprBuffers();

    return 0;
}
