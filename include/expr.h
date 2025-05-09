#ifndef EXPR_H
#define EXPR_H

#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <limits.h>

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

#define INDEX_MAX INT_MAX

struct _Region {
    int index;
    Scope * scope;
    AVLTree pool;
    Region * parent;
};

typedef struct _ExprTag ExprTag;

typedef struct {
    ExprTag * tag;
    Region * owner;
    int lifetime;
} Expr;

struct _ExprTag {
    Expr _expr;
    void * (* move)(Region * dest, Region * src, void *);
    void * (* apply)(Region *, void *, Array *);
    void * (* eval)(Region *, void *);
    bool (* equal)(void *, void *);
    size_t (* show)(char *, size_t, void *);
    void (* delete)(void *);
    size_t size;
};

extern ExprTag exprTag;

void setVar(Scope *, const char *, void *);
Expr * getVar(Scope *, const char *);

void * newExpr(Region *, ExprTag *);
void newExprImmortal(ExprTag *, ...);

void * move(Region *, Expr *);
void invalidate(void *);
void delete(void *);

void * apply(Region *, void *, Array *);
void * eval(Region *, void *);

size_t show(char *, size_t, void *);
bool equal(void *, void *);

void initExpr(void);

Scope * newScope(Scope *);
void deleteScope(Scope *);

Region * newRegion(Region *);
void deleteRegion(Region *);

static inline int indexof(Region * region)
{ return region == NULL ? -1 : region->index; }

static inline ExprTag * tagof(void * value)
{ Expr * expr = value; return expr->tag; }

static inline Region * ownerof(void * value)
{ Expr * expr = value; return expr->owner; }

static inline void copyScope(Scope * dest, Scope * src)
{ copyTrie(&dest->context, &src->context); }

const char * showExpr(void *);

bool equalByRef(void *, void *);
void * applyThrowError(Region *, void *, Array *);
void * trivEval(Region *, void *);
void trivDelete(void *);
void * trivMove(Region * dest, Region * src, void *);

static inline size_t ellipsis(char * buf)
{ strcpy(buf, "..."); return 3; }

#define IFNRET(x) if ((x) == NULL) return NULL;

#endif
