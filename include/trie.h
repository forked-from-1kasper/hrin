#ifndef TRIE_H
#define TRIE_H

typedef struct {
    void * next[2];
} BinaryTreeNode;

typedef struct {
    BinaryTreeNode node;
    void * value;
} BinaryTreeValue;

typedef struct {
    BinaryTreeValue root;
} Trie;

Trie newTrie();
void * findTrie(Trie *, const char *);
void setTrie(Trie *, const char *, void *);
void freeTrie(Trie *);
void copyTrie(Trie * dest, const Trie * src);

#endif