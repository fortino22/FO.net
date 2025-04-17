#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "avl.h"

int max(int a, int b) {
    return (a > b) ? a : b;
}

int getHeight(AVLNode *node) {
    return node ? node->height : 0;
}

int getBalance(AVLNode *node) {
    return node ? getHeight(node->left) - getHeight(node->right) : 0;
}

AVLNode *createNode(const char *username, const char *password, const char *role) {
    AVLNode *newNode = (AVLNode *)malloc(sizeof(AVLNode));
    strcpy(newNode->user.username, username);
    strcpy(newNode->user.password, password);
    strcpy(newNode->user.role, role);
    newNode->left = newNode->right = NULL;
    newNode->height = 1;
    return newNode;
}

AVLNode *rightRotate(AVLNode *y) {
    AVLNode *x = y->left;
    AVLNode *T2 = x->right;

    x->right = y;
    y->left = T2;

    y->height = 1 + max(getHeight(y->left), getHeight(y->right));
    x->height = 1 + max(getHeight(x->left), getHeight(x->right));

    return x;
}

AVLNode *leftRotate(AVLNode *x) {
    AVLNode *y = x->right;
    AVLNode *T2 = y->left;

    y->left = x;
    x->right = T2;

    x->height = 1 + max(getHeight(x->left), getHeight(x->right));
    y->height = 1 + max(getHeight(y->left), getHeight(y->right));

    return y;
}

AVLNode *insert(AVLNode *node, const char *username, const char *password, const char *role) {
    if (!node)
        return createNode(username, password, role);

    if (strcmp(username, node->user.username) < 0)
        node->left = insert(node->left, username, password, role);
    else if (strcmp(username, node->user.username) > 0)
        node->right = insert(node->right, username, password, role);
    else {
        printf("Username already exists!\n");
        return node;
    }

    node->height = 1 + max(getHeight(node->left), getHeight(node->right));
    int balance = getBalance(node);

    if (balance > 1 && strcmp(username, node->left->user.username) < 0)
        return rightRotate(node);

    if (balance < -1 && strcmp(username, node->right->user.username) > 0)
        return leftRotate(node);

    if (balance > 1 && strcmp(username, node->left->user.username) > 0) {
        node->left = leftRotate(node->left);
        return rightRotate(node);
    }

    if (balance < -1 && strcmp(username, node->right->user.username) < 0) {
        node->right = rightRotate(node->right);
        return leftRotate(node);
    }

    return node;
}

AVLNode *search(AVLNode *root, const char *username) {
    if (!root) return NULL;

    if (strcmp(username, root->user.username) == 0)
        return root;
    else if (strcmp(username, root->user.username) < 0)
        return search(root->left, username);
    else
        return search(root->right, username);
}

