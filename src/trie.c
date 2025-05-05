#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>

#include <trie.h>

#define NEXT(n, b) { n = n->next[b & 1]; if (n == NULL) return NULL; }

static inline BinaryTreeValue * nextValueTree(BinaryTreeValue * n, uint8_t k) {
    BinaryTreeNode * curr = &n->node;

    NEXT(curr, k >> 7);
    NEXT(curr, k >> 6);
    NEXT(curr, k >> 5);
    NEXT(curr, k >> 4);
    NEXT(curr, k >> 3);
    NEXT(curr, k >> 2);
    NEXT(curr, k >> 1);

    return curr->next[k & 1];
}

static void * findBinaryTree(BinaryTreeValue * n, const char * key) {
    for (;;) {
        if (n == NULL) return NULL;
        if (*key == '\0') return n->value;

        n = nextValueTree(n, *(key++));
    }
}

#define NEW(n, b) { if (n->next[b & 1] == NULL) n->next[b & 1] = calloc(1, sizeof(BinaryTreeNode)); n = n->next[b & 1]; }

static inline BinaryTreeValue * newValueTree(BinaryTreeValue * n, uint8_t k) {
    BinaryTreeNode * curr = &n->node;

    NEW(curr, k >> 7);
    NEW(curr, k >> 6);
    NEW(curr, k >> 5);
    NEW(curr, k >> 4);
    NEW(curr, k >> 3);
    NEW(curr, k >> 2);
    NEW(curr, k >> 1);

    if (curr->next[k & 1] == NULL) curr->next[k & 1] = calloc(1, sizeof(BinaryTreeValue));

    return curr->next[k & 1];
}

static void setBinaryTree(BinaryTreeValue * n, const char * key, void * value) {
    while (*key != '\0') n = newValueTree(n, *(key++));

    n->value = value;
}

static void freeBST(BinaryTreeNode * n) {
    if (n == NULL) return;

    freeBST(n->next[0]);
    freeBST(n->next[1]);
    free(n);
}

static void * copyTree(void * destAddr, const void * srcAddr, int nbit) {
    destAddr = destAddr == NULL ? calloc(1, nbit == 0 ? sizeof(BinaryTreeValue) : sizeof(BinaryTreeNode)) : destAddr;

    if (nbit == 0) {
        BinaryTreeValue * dest = destAddr;
        const BinaryTreeValue * src = srcAddr;

        if (dest->value == NULL) dest->value = src->value;
        nbit = 8;
    }

    BinaryTreeNode * dest = destAddr;
    const BinaryTreeNode * src = srcAddr;

    if (src->next[0] != NULL) dest->next[0] = copyTree(dest->next[0], src->next[0], nbit - 1);
    if (src->next[1] != NULL) dest->next[1] = copyTree(dest->next[1], src->next[1], nbit - 1);

    return destAddr;
}

Trie newTrie(void) {
    return (Trie) {.root = {.node = {.next = {NULL, NULL}}, .value = NULL}};
}

void * findTrie(Trie * T, const char * key) {
    return findBinaryTree(&T->root, key);
}

void setTrie(Trie * T, const char * key, void * value) {
    setBinaryTree(&T->root, key, value);
}

void freeTrie(Trie * T) {
    BinaryTreeNode * n = &T->root.node;

    freeBST(n->next[0]);
    freeBST(n->next[1]);
}

void copyTrie(Trie * dest, const Trie * src) {
    copyTree(&dest->root, &src->root, 0);
}
