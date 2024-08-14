#ifndef EXPR_H
#define EXPR_H

#include <stdbool.h>
#include <stdint.h>

#include <avl.h>
#include <trie.h>
#include <array.h>

#include <error.h>

typedef struct _Scope Scope;

struct _Scope {
    bool lexical;
    Trie context;
    Scope * next;
};

typedef struct _Region Region;

struct _Region {
    int index;
    Scope * scope;
    AVLTree pool;
    Region * parent;
};

typedef int ExprTag;
typedef struct { ExprTag tag; Region * owner; } Expr;

typedef struct {
    void * (* eval)(Region *, void *);
    void * (* apply)(Region *, void *, Array *);
    char * (* show)(void *);
    void (* move)(Region * dest, Region * src, void *);
    void (* delete)(void *);
    size_t size;
} ExprTagImpl;

void setVar(Scope *, const char *, void *);
Expr * getVar(Scope *, const char *);

ExprTag newExprTag(ExprTagImpl);
void * newExpr(Region *, ExprTag);
void newExprImmortal(ExprTag, ...);

void * apply(Region *, void *, Array *);
void * eval(Region *, void *);
void move(Region *, Expr *);
void delete(void *);
char * show(void *);

void deallocExprBuffers(void);

Scope * newScope(Scope *);
void deleteScope(Scope *);

Region * newRegion(Region *);
void deleteRegion(Region *);

static inline int indexof(Region * region)
{ return region == NULL ? -1 : region->index; }

static inline ExprTag tagof(void * value)
{ Expr * expr = value; return expr->tag; }

static inline Region * ownerof(void * value)
{ Expr * expr = value; return expr->owner; }

static inline void copyScope(Scope * dest, Scope * src)
{ copyTrie(&dest->context, &src->context); }

void * evalNf(Region *, void *);
void * applyThrowError(Region *, void *, Array *);

#endif