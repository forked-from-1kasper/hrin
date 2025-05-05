#include <stdarg.h>
#include <stdio.h>

#include <common.h>
#include <expr.h>

void * evalNf(Region * region, void * value) {
    UNUSED(region);

    return value;
}

bool equalByRef(void * value1, void * value2) {
    return value1 == value2;
}

void * applyThrowError(Region * region, void * x, Array * xs) {
    UNUSED(region); UNUSED(xs);

    return throw(ApplyErrorTag, "%s is not callable", showExpr(x));
}

static size_t showErrval(char * buf, size_t size, void * value) {
    UNUSED(value);

    if (size <= 7) return ellipsis(buf);
    strcpy(buf, "#ERRVAL"); return 7;
}

static void deleteErrval(void * value) {
    UNUSED(value);
}

static void * moveErrval(Region * dest, Region * src, void * value) {
    UNUSED(dest); UNUSED(src); UNUSED(value);

    return value;
}

static ExprTagImpl exprErrvalImpl = {
    .eval   = evalNf,
    .apply  = applyThrowError,
    .show   = showErrval,
    .delete = deleteErrval,
    .move   = moveErrval,
    .equal  = equalByRef,
    .size   = 0
};

ExprTag exprErrvalTag;

void setVar(Scope * scope, const char * x, void * o) {
    setTrie(&scope->context, x, o);
}

Expr * getVar(Scope * scope, const char * x) {
    while (scope != NULL) {
        Expr * retval = findTrie(&scope->context, x);
        if (retval != NULL) return retval;

        scope = scope->next;
    }

    return NULL;
}

static ExprTag availableExprTag = 0;
static ExprTagImpl * exprTagImpl = NULL;

ExprTag newExprTag(ExprTagImpl impl) {
    ExprTag newTag = availableExprTag;

    availableExprTag++;

    exprTagImpl = realloc(exprTagImpl, availableExprTag * sizeof(ExprTagImpl));
    exprTagImpl[newTag] = impl;

    return newTag;
}

void initExpr(void) {
    exprErrvalTag = newExprTag(exprErrvalImpl);
}

void deinitExpr(void) {
    if (exprTagImpl != NULL)
        free(exprTagImpl);
}

static inline void takeOwnership(Region * region, Expr * o) {
    o->owner = region;
    insertAVLTree(&region->pool, o);
}

void * newExpr(Region * region, ExprTag tag) {
    Expr * o = malloc(exprTagImpl[tag].size);
    if (o == NULL) return throw(OOMErrorTag, NULL);

    o->tag      = tag;
    o->lifetime = -1;

    takeOwnership(region, o);

    return o;
}

void newExprImmortal(ExprTag tag, ...) {
    va_list argv;

    va_start(argv, tag);

    for (;;) {
        Expr * o = va_arg(argv, void *);

        if (o == NULL) break;

        o->tag   = tag;
        o->owner = NULL;
    }

    va_end(argv);
}

void * apply(Region * region, void * f, Array * xs) {
    Expr * expr = f;
    return exprTagImpl[expr->tag].apply(region, f, xs);
}

void * eval(Region * region, void * value) {
    Expr * o = exprTagImpl[tagof(value)].eval(region, value);
    if (o == NULL) fprintf(stderr, "↳ %s\n", showExpr(value));

    return o;
}

size_t show(char * buf, size_t size, void * value) {
    Expr * expr = value;
    return exprTagImpl[expr->tag].show(buf, size, value);
}

void delete(void * value) {
    Expr * expr = value;

    exprTagImpl[expr->tag].delete(value);
    free(value);
}

void invalidate(void * value) {
    Expr * expr = value;

    exprTagImpl[expr->tag].delete(value);
    expr->tag = exprErrvalTag;
}

bool equal(void * value1, void * value2) {
    if (tagof(value1) != tagof(value2)) return false;
    return exprTagImpl[tagof(value1)].equal(value1, value2);
}

void * move(Region * dest, Expr * o) {
    Region * src = o->owner;

    if (src == NULL || src->index <= dest->index)
        return o;

    deleteAVLTree(&src->pool, o);
    takeOwnership(dest, o);

    void * retptr = o;

    if (exprTagImpl[o->tag].move(dest, src, o) == NULL)
        retptr = NULL;

    if (dest->index < o->lifetime) {
        throw(RegionErrorTag, "attempt to move value beyond its lifetime: %s", showExpr(o));
        invalidate(o);
        retptr = NULL;
    }

    return retptr;
}

Scope * newScope(Scope * next) {
    Scope * scope = malloc(sizeof(Scope));
    if (scope == NULL) return throw(OOMErrorTag, NULL);

    scope->lexical = true;
    scope->context = newTrie();
    scope->next    = next;

    return scope;
}

void deleteScope(Scope * scope) {
    freeTrie(&scope->context);
    free(scope);
}

Region * newRegion(Region * parent) {
    if (indexof(parent) == INDEX_MAX)
        return throw(OOMErrorTag, "maximum region depth exceeded");

    Region * region = malloc(sizeof(Region));
    if (region == NULL) return throw(OOMErrorTag, NULL);

    region->index  = indexof(parent) + 1;
    region->scope  = NULL;
    region->pool   = newAVLTree();
    region->parent = parent;

    return region;
}

static void freeTreePool(TreeNode * n) {
    if (n != NULL) {
        freeTreePool(n->left);
        freeTreePool(n->right);
        delete(n->value);
        free(n);
    }
}

void deleteRegion(Region * region) {
    freeTreePool(region->pool.root);
    free(region);
}

const char * showExpr(void * value) {
    static char buf[512];

    show(buf, sizeof(buf), value);
    return buf;
}
