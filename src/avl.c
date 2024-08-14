#include <stddef.h>
#include <stdlib.h>

#include <common.h>
#include <avl.h>

inline static int height(TreeNode * n)
{ return n == NULL ? 0 : n->height; }

inline static void updateHeightTreeNode(TreeNode * n)
{ n->height = max(height(n->left), height(n->right)) + 1; }

static TreeNode * rotrr(TreeNode * n) {
    TreeNode * nl  = n->left;
    TreeNode * nlr = nl->right;

    nl->right = n;
    n->left   = nlr;

    updateHeightTreeNode(nl);
    updateHeightTreeNode(n);

    return nl;
}

static TreeNode * rotll(TreeNode * n) {
    TreeNode * nr  = n->right;
    TreeNode * nrl = nr->left;

    nr->left = n;
    n->right = nrl;

    updateHeightTreeNode(nr);
    updateHeightTreeNode(n);

    return nr;
}

inline static TreeNode * rotlr(TreeNode * n) {
    n->left = rotll(n->left);
    return rotrr(n);
}

inline static TreeNode * rotrl(TreeNode * n) {
    n->right = rotrr(n->right);
    return rotll(n);
}

inline static int bf(TreeNode * n)
{ return n == NULL ? 0 : height(n->left) - height(n->right); }

inline static TreeNode * rebalance(TreeNode * n) {
    if (n == NULL) return NULL;
    updateHeightTreeNode(n);

    int balance = bf(n);

    if (balance > 1) return bf(n->left) > 0 ? rotrr(n) : rotlr(n);

    if (balance < -1) return bf(n->right) > 0 ? rotrl(n) : rotll(n);

    return n;
}

inline static TreeNode * newTreeNode(void * value) {
    TreeNode * n = malloc(sizeof(TreeNode));

    n->value  = value;
    n->left   = NULL;
    n->right  = NULL;
    n->height = 1;

    return n;
}

static TreeNode * insertTreeNode(TreeNode * n, void * value) {
    if (n == NULL) return newTreeNode(value);

    if (value < n->value)
        n->left = insertTreeNode(n->left, value);
    else if (value > n->value)
        n->right = insertTreeNode(n->right, value);
    else
        return n;

    return rebalance(n);
}

inline static TreeNode * minTreeNode(TreeNode * n) {
    while (n->left != NULL)
        n = n->left;

    return n;
}

static TreeNode * deleteTreeNode(TreeNode * n, void * value) {
    if (n == NULL) return NULL;

    if (value < n->value)
        n->left = deleteTreeNode(n->left, value);
    else if (value > n->value)
        n->right = deleteTreeNode(n->right, value);
    else if (n->left == NULL) {
        TreeNode * nr = n->right;
        free(n); return nr;
    } else if (n->right == NULL) {
        TreeNode * nl = n->left;
        free(n); return nl;
    } else {
        TreeNode * nsucc = minTreeNode(n->right);

        n->value = nsucc->value;
        n->right = deleteTreeNode(n->right, nsucc->value);
    }

    return rebalance(n);
}

static bool findTreeNode(TreeNode * n, void * value) {
    if (n == NULL) return false;

    if (value < n->value)
        return findTreeNode(n->left, value);
    else if (value > n->value)
        return findTreeNode(n->right, value);
    else
        return true;
}

AVLTree newAVLTree() {
    return (AVLTree) {.root = NULL};
}

bool findAVLTree(AVLTree * T, void * value) {
    return findTreeNode(T->root, value);
}

void insertAVLTree(AVLTree * T, void * value) {
    T->root = insertTreeNode(T->root, value);
}

void deleteAVLTree(AVLTree * T, void * value) {
    T->root = deleteTreeNode(T->root, value);
}
