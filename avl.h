#ifndef AVL_H
#define AVL_H

struct User {
    char username[50];
    char password[50];
    char role[20]; 
};

struct AVLNode {
    User user;
    AVLNode *left, *right;
    int height;
};

AVLNode* insert(AVLNode* root, const char* username, const char* password, const char* role);
AVLNode* search(AVLNode* root, const char* username);
int getHeight(AVLNode* node);

#endif

