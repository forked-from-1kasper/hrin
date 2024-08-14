#include <stdarg.h>

#include <common.h>
#include <expr.h>

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

void deallocExprBuffers() {
    if (exprTagImpl != NULL)
        free(exprTagImpl);
}

static inline void takeOwnership(Region * region, Expr * o) {
    o->owner = region;
    insertAVLTree(&region->pool, o);
}

void * newExpr(Region * region, ExprTag tag) {
    Expr * o = malloc(exprTagImpl[tag].size);
    if (o == NULL) return NULL;

    o->tag = tag;

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
    Expr * expr = value;
    return exprTagImpl[expr->tag].eval(region, value);
}

char * show(void * value) {
    Expr * expr = value;
    return exprTagImpl[expr->tag].show(value);
}

void delete(void * value) {
    Expr * expr = value;

    exprTagImpl[expr->tag].delete(value);
    free(value);
}

void move(Region * dest, Expr * o) {
    Region * src = o->owner;

    if (src == NULL || src->index <= dest->index) return;
    deleteAVLTree(&src->pool, o);

    takeOwnership(dest, o);
    exprTagImpl[o->tag].move(dest, src, o);
}

Scope * newScope(Scope * next) {
    Scope * scope = malloc(sizeof(Scope));
    if (scope == NULL) return NULL;

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
    Region * region = malloc(sizeof(Region));
    if (region == NULL) return NULL;

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

void * evalNf(Region * region, void * value) {
    UNUSED(region);

    return value;
}

void * applyThrowError(Region * region, void * x, Array * xs) {
    UNUSED(region); UNUSED(xs);

    char * bufobj = show(x);
    throw(ApplyErrorTag, "%s is not callable", bufobj);
    free(bufobj);

    return NULL;
}
