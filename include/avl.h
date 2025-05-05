#ifndef AVL_H
#define AVL_H

#include <stdbool.h>

typedef struct _TreeNode TreeNode;

typedef struct {
    TreeNode * root;
} AVLTree;

struct _TreeNode {
    void * value;
    int height;
    TreeNode * left;
    TreeNode * right;
};

AVLTree newAVLTree();
bool findAVLTree(AVLTree *, void *);
void insertAVLTree(AVLTree *, void *);
void deleteAVLTree(AVLTree *, void *);

#endif
