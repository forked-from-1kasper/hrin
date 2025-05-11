#include <stdarg.h>
#include <stdio.h>

#include <common.h>
#include <expr.h>

#include <objects/boolean.h>

static size_t showTag(char * buf, size_t size, void * value) {
    if (size <= 23) return ellipsis(buf);

    return snprintf(buf, size, "<#TAG %016lx>", (uintptr_t) value);
}

static void * applyTag(Region * region, void * value, Array * xs) {
    ARITY(1, xs->size);

    Expr * o = eval(region, getArray(xs, 0)); IFNRET(o);
    return newBool(o->tag == value);
}

static size_t showErrval(char * buf, size_t size, void * value) {
    UNUSED(value);

    if (size <= 7) return ellipsis(buf);
    strcpy(buf, "#ERRVAL"); return 7;
}

void * applyThrowError(Region * region, void * x, Array * xs) {
    UNUSED(region); UNUSED(xs);

    return throw(ApplyErrorTag, "%s is not callable", showExpr(x));
}

bool equalByRef(void * value1, void * value2) {
    return value1 == value2;
}

void * trivEval(Region * region, void * value) {
    UNUSED(region);

    return value;
}

void trivDelete(void * value) {
    UNUSED(value);
}

void * trivMove(Region * dest, Region * src, void * value) {
    UNUSED(dest); UNUSED(src); UNUSED(value);

    return value;
}

ExprTag exprTag = {
    .eval   = trivEval,
    .apply  = applyTag,
    .show   = showTag,
    .delete = trivDelete,
    .move   = trivMove,
    .equal  = equalByRef,
    .size   = 0
};

static ExprTag exprErrvalTag = {
    .eval   = trivEval,
    .apply  = applyThrowError,
    .show   = showErrval,
    .delete = trivDelete,
    .move   = trivMove,
    .equal  = equalByRef,
    .size   = 0
};

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

void initExpr(void) {
    newExprImmortal(&exprTag, &exprTag, &exprErrvalTag, NULL);
}

static inline void takeOwnership(Region * region, Expr * o) {
    o->owner = region;
    insertAVLTree(&region->pool, o);
}

void * newExpr(Region * region, ExprTag * tag) {
    if (tag->size == 0) return throw(
        RegionErrorTag, "cannot allocate static type on the heap"
    );

    Expr * o = malloc(tag->size);
    if (o == NULL) return throw(OOMErrorTag, NULL);

    o->tag      = tag;
    o->lifetime = -1;

    takeOwnership(region, o);

    return o;
}

void newExprImmortal(ExprTag * tag, ...) {
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

void * apply(Region * region, void * x, Array * xs) {
    return tagof(x)->apply(region, x, xs);
}

void * eval(Region * region, void * value) {
    Expr * o = tagof(value)->eval(region, value);
    if (o == NULL) fprintf(stderr, "↳ %s\n", showExpr(value));

    return o;
}

size_t show(char * buf, size_t size, void * value) {
    return tagof(value)->show(buf, size, value);
}

void delete(void * value) {
    tagof(value)->delete(value);
    free(value);
}

void invalidate(void * value) {
    Expr * expr = value;

    expr->tag->delete(value);
    expr->tag = &exprErrvalTag;
}

bool equal(void * value1, void * value2) {
    if (tagof(value1) != tagof(value2)) return false;
    return tagof(value1)->equal(value1, value2);
}

void * move(Region * dest, Expr * o) {
    Region * src = o->owner;

    if (src == NULL || src->index <= dest->index)
        return o;

    deleteAVLTree(&src->pool, o);
    takeOwnership(dest, o);

    void * retptr = o;

    if (o->tag->move(dest, src, o) == NULL)
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

void setVars(Scope * scope, ...) {
    va_list argv;

    va_start(argv, scope);

    for (;;) {
        const char * x = va_arg(argv, const char *);
        if (x == NULL) break;

        void * o = va_arg(argv, void *);
        setVar(scope, x, o);
    }

    va_end(argv);
}
