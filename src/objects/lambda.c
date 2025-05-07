#include <stdio.h>

#include <common.h>
#include <array.h>
#include <trie.h>

#include <objects/lambda.h>
#include <objects/extern.h>
#include <objects/atom.h>

static void * applyMacro(Region * region, void * value, Array * xs) {
    ExprLexical * expr = value;

    ARITY(expr->vars.size, xs->size);

    Region * nested = newRegion(region); IFNRET(nested);
    nested->scope = newScope(expr->scope); // TODO

    void * retval = NULL;

    for (size_t i = 0; i < xs->size; i++)
        setVar(nested->scope, getArray(&expr->vars, i), getArray(xs, i));

    void * o = eval(nested, expr->value);
    if (o == NULL) goto finally;

    retval = eval(region, o);

    if (retval == NULL) goto finally;
    if (move(region, retval) == NULL) retval = NULL;

    finally: deleteScope(nested->scope); deleteRegion(nested);

    return retval;
}

static void * applyLambda(Region * region, void * value, Array * xs) {
    ExprLexical * expr = value;

    ARITY(expr->vars.size, xs->size);

    Region * nested = newRegion(region); IFNRET(nested);
    nested->scope = newScope(expr->scope); // TODO

    void * retval = NULL;

    for (size_t i = 0; i < xs->size; i++) {
        void * o = eval(region, getArray(xs, i));
        if (o == NULL) goto finally;

        setVar(nested->scope, getArray(&expr->vars, i), o);
    }

    retval = eval(nested, expr->value);

    if (retval == NULL) goto finally;
    if (move(region, retval) == NULL) retval = NULL;

    finally: deleteScope(nested->scope); deleteRegion(nested);

    return retval;
}

static size_t showLambda(char * buf, size_t size, void * value) {
    //ExprLexical * expr = value;

    if (size <= 26) return ellipsis(buf);
    return snprintf(buf, size, "<#LAMBDA %016lx>", (uintptr_t) value);
}

static size_t showMacro(char * buf, size_t size, void * value) {
    //ExprLexical * expr = value;

    if (size <= 25) return ellipsis(buf);
    return snprintf(buf, size, "<#MACRO %016lx>", (uintptr_t) value);
}

static void deleteLexical(void * value) {
    ExprLexical * expr = value;

    if (expr->scope != NULL)
        deleteScope(expr->scope);

    for (size_t i = 0; i < expr->vars.size; i++)
        free(getArray(&expr->vars, i));

    freeArray(&expr->vars);
}

static void * moveTree(Region * region, void * n, int nbit) {
    void * retptr = n;

    if (nbit == 0) {
        BinaryTreeValue * btv = n;
        if (btv->value != NULL && move(region, btv->value) == NULL)
            retptr = NULL;

        nbit = 8;
    }

    BinaryTreeNode * btn = n;

    if (btn->next[0] != NULL && moveTree(region, btn->next[0], nbit - 1) == NULL)
        retptr = NULL;

    if (btn->next[1] != NULL && moveTree(region, btn->next[1], nbit - 1) == NULL)
        retptr = NULL;

    return retptr;
}

static inline void * moveTrie(Region * region, Trie * T)
{ return moveTree(region, &T->root, 0); }

static void * moveLexical(Region * dest, Region * src, void * value) {
    UNUSED(src);

    ExprLexical * expr = value; void * retptr = value;

    if (moveTrie(dest, &expr->scope->context) == NULL)
        retptr = NULL;

    if (move(dest, expr->value) == NULL)
        retptr = NULL;

    return retptr;
}

ExprTag exprLambdaTag = {
    .eval   = evalNf,
    .apply  = applyLambda,
    .show   = showLambda,
    .delete = deleteLexical,
    .move   = moveLexical,
    .equal  = equalByRef,
    .size   = sizeof(ExprLexical)
};

ExprTag exprMacroTag = {
    .eval   = evalNf,
    .apply  = applyMacro,
    .show   = showMacro,
    .delete = deleteLexical,
    .move   = moveLexical,
    .equal  = equalByRef,
    .size   = sizeof(ExprLexical)
};

Scope * global = NULL; // TODO

void * newLexical(ExprTag * tag, Region * region, Array vars, void * value) {
    ExprLexical * retval = newExpr(region, tag);
    retval->scope = newScope(global); // TODO
    retval->vars  = vars;
    retval->value = value;

    Scope * curr = region->scope;
    while (curr != NULL) {
        if (curr->lexical)
            copyScope(retval->scope, curr);

        curr = curr->next;
    }

    return retval;
}

void * externLexical(ExprTag * tag, Region * region, Array * xs) {
    if (xs->size <= 0) return throw(TypeErrorTag, "no arguments were given");

    for (size_t i = 0; i < xs->size - 1; i++) {
        if (tagof(getArray(xs, i)) != &exprAtomTag)
            return throw(TypeErrorTag, "%s expected to be an atom", showExpr(getArray(xs, i)));
    }

    Array vars = newArray(xs->size - 1);

    for (size_t j = 0; j < xs->size - 1; j++) {
        ExprAtom * i = getArray(xs, j);
        setArray(&vars, j, dup(i->value));
    }

    return newLexical(tag, region, vars, getArray(xs, xs->size - 1));
}

void * externLambda(Region * region, Array * xs) {
    return externLexical(&exprLambdaTag, region, xs);
}

void * externMacro(Region * region, Array * xs) {
    return externLexical(&exprMacroTag, region, xs);
}

void * externExpand(Region * region, Array * xs) {
    ARITY(1, xs->size);

    ExprLexical * o = eval(region, getArray(xs, 0));
    if (o == NULL) return NULL;

    if (tagof(o) != &exprLambdaTag && tagof(o) != &exprMacroTag)
        return throw(TypeErrorTag, "%s expected to be a closure", showExpr(o));

    return o->value;
}

void initLexicalTags(Region * region) {
    global = region->scope;

    newExprImmortal(&exprTag, &exprLambdaTag, &exprMacroTag, NULL);

    setVar(region->scope, "λ",      newExtern(region, externLambda));
    setVar(region->scope, "Λ",      newExtern(region, externMacro));
    setVar(region->scope, "expand", newExtern(region, externExpand));
}
