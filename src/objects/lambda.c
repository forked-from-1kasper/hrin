#include <stdio.h>

#include <common.h>
#include <array.h>
#include <trie.h>

#include <objects/lambda.h>

static void * applyLambda(Region * region, void * value, Array * xs) {
    ExprLambda * expr = value;

    if (xs->size != expr->vars.size)
        return throw(TypeErrorTag, "expected %zu argument(s) but %zu were given", expr->vars.size, xs->size);

    Region * internal = newRegion(region);
    internal->scope = newScope(expr->scope);

    void * retval = NULL;

    for (size_t i = 0; i < xs->size; i++) {
        void * o = eval(region, getArray(xs, i));
        if (o == NULL) goto finally;

        setVar(internal->scope, getArray(&expr->vars, i), o);
    }

    retval = eval(internal, expr->value);
    if (retval != NULL) move(region, retval);

    finally: deleteScope(internal->scope); deleteRegion(internal); return retval;
}

static char * showLambda(void * value) {
    //ExprLambda * expr = value;

    char * retbuf = malloc(27); sprintf(retbuf, "<#LAMBDA %016lx>", (uintptr_t) value);

    return retbuf;
}

static void deleteLambda(void * value) {
    ExprLambda * expr = value;

    if (expr->scope != NULL)
        deleteScope(expr->scope);

    for (size_t i = 0; i < expr->vars.size; i++)
        free(getArray(&expr->vars, i));

    freeArray(&expr->vars);
}

static void moveTree(Region * region, void * n, int nbit) {
    if (nbit == 0) {
        BinaryTreeValue * btv = n;
        if (btv->value != NULL)
            move(region, btv->value);

        nbit = 8;
    }

    BinaryTreeNode * btn = n;

    if (btn->next[0] != NULL) moveTree(region, btn->next[0], nbit - 1);
    if (btn->next[1] != NULL) moveTree(region, btn->next[1], nbit - 1);
}

static inline void moveTrie(Region * region, Trie * T)
{ moveTree(region, &T->root, 0); }

static void moveLambda(Region * dest, Region * src, void * value) {
    UNUSED(src);

    ExprLambda * expr = value;

    moveTrie(dest, &expr->scope->context);
    move(dest, expr->value);
}

ExprTagImpl exprLambdaImpl = {
    .eval   = evalNf,
    .apply  = applyLambda,
    .show   = showLambda,
    .delete = deleteLambda,
    .move   = moveLambda,
    .size   = sizeof(ExprLambda)
};

ExprTag exprLambdaTag;