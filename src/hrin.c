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
#include <objects/atom.h>
#include <objects/nil.h>
#include <objects/cc.h>

#include <lexer.h>
#include <parser.h>

Region * rootRegion = NULL;

void * externDefine(Region * region, Array * xs) {
    if (xs->size != 2) return throw(TypeErrorTag, "expected 2 arguments but %zu were given", xs->size);
    ExprAtom * i = getArray(xs, 0);

    if (tagof(i) != exprAtomTag) return throw(TypeErrorTag, "%s expected to be an atom", showExpr(i));

    Expr * o = eval(region, getArray(xs, 1));
    if (o == NULL) return NULL;

    move(rootRegion, o);
    setVar(rootRegion->scope, i->value, o);

    return &exprNil;
}

void * externDeflocal(Region * region, Array * xs) {
    if (xs->size != 2) return throw(TypeErrorTag, "expected 2 arguments but %zu were given", xs->size);
    ExprAtom * i = getArray(xs, 0);

    if (tagof(i) != exprAtomTag) return throw(TypeErrorTag, "%s expected to be an atom", showExpr(i));

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

void * externPrint(Region * region, Array * xs) {
    for (size_t i = 0; i < xs->size; i++) {
        void * o = eval(region, getArray(xs, i));
        if (o == NULL) return NULL;

        printf("%s", tagof(o) == exprStringTag ? ((ExprString *) o)->value : showExpr(o));
    }

    return &exprNil;
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

    char buf[1024];

    show(buf, sizeof(buf), e1);
    printf(">>> %s\n", buf);

    Expr * e2 = eval(region, e1);
    if (e2 == NULL) { retval = printError(); goto finally; }

    show(buf, sizeof(buf), e2);
    printf("<<< %s\n", buf);

    finally:
    deleteScope(region->scope);
    deleteRegion(region);

    return retval;
}

int main(int argc, char * argv[]) {
    Scope * globalScope = newScope(NULL);
    globalScope->lexical = false;

    rootRegion = newRegion(NULL);
    rootRegion->scope = globalScope;

    initExternTag(rootRegion); // Everything below can rely on `newExtern`.

    initNilTag(rootRegion);
    initCCTag(rootRegion);
    initBooleanTag(rootRegion);
    initAtomTag(rootRegion);
    initIntegerTag(rootRegion);
    initStringTag(rootRegion);
    initLexicalTags(rootRegion);

    setVar(rootRegion->scope, "define",   newExtern(rootRegion, externDefine));
    setVar(rootRegion->scope, "progn",    newExtern(rootRegion, externProgn));
    setVar(rootRegion->scope, "deflocal", newExtern(rootRegion, externDeflocal));
    setVar(rootRegion->scope, "quote",    newExtern(rootRegion, externQuote));
    setVar(rootRegion->scope, "eval",     newExtern(rootRegion, externEval));
    setVar(rootRegion->scope, "print!",   newExtern(rootRegion, externPrint));

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
