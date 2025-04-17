#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "avl.h"

generateRandomId() {
    static int initialized = 0;
    if (!initialized) {
        srand(time(NULL));
        initialized = 1;
    }
    return 1000 + rand() % 9000;
}

int max(int a, int b) {
    return (a > b) ? a : b;
}

int getHeight(AVLNode *node) {
    return node ? node->height : 0;
}

int getBalance(AVLNode *node) {
    return node ? getHeight(node->left) - getHeight(node->right) : 0;
}

AVLNode *createNode(int userId, const char *username, const char *password, const char *role) {
    AVLNode *newNode = (AVLNode *)malloc(sizeof(AVLNode));
    newNode->user.userId = userId;
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

AVLNode *insert(AVLNode *node, int userId, const char *username, const char *password, const char *role) {
    if (!node)
        return createNode(userId, username, password, role);
    if (strcmp(username, node->user.username) < 0)
        node->left = insert(node->left, userId, username, password, role);
    else if (strcmp(username, node->user.username) > 0)
        node->right = insert(node->right, userId, username, password, role);
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

AVLNode *searchById(AVLNode *root, int userId) {
    if (!root) return NULL;
    if (userId == root->user.userId)
        return root;
    else if (userId < root->user.userId)
        return searchById(root->left, userId);
    else
        return searchById(root->right, userId);
}

int getAssignmentHeight(AssignmentNode *node) {
    return node ? node->height : 0;
}

int getAssignmentBalance(AssignmentNode *node) {
    return node ? getAssignmentHeight(node->left) - getAssignmentHeight(node->right) : 0;
}

AssignmentNode *createAssignmentNode(int assignmentId, int userId, const char *title, const char *description) {
    AssignmentNode *newNode = (AssignmentNode *)malloc(sizeof(AssignmentNode));
    newNode->assignment.assignmentId = assignmentId;
    newNode->assignment.userId = userId;
    strcpy(newNode->assignment.title, title);
    strcpy(newNode->assignment.description, description);
    strcpy(newNode->assignment.status, "pending"); // Default status
    newNode->left = newNode->right = NULL;
    newNode->height = 1;
    return newNode;
}

AssignmentNode *rightRotateAssignment(AssignmentNode *y) {
    AssignmentNode *x = y->left;
    AssignmentNode *T2 = x->right;
    x->right = y;
    y->left = T2;
    y->height = 1 + max(getAssignmentHeight(y->left), getAssignmentHeight(y->right));
    x->height = 1 + max(getAssignmentHeight(x->left), getAssignmentHeight(x->right));
    return x;
}

AssignmentNode *leftRotateAssignment(AssignmentNode *x) {
    AssignmentNode *y = x->right;
    AssignmentNode *T2 = y->left;
    y->left = x;
    x->right = T2;
    x->height = 1 + max(getAssignmentHeight(x->left), getAssignmentHeight(x->right));
    y->height = 1 + max(getAssignmentHeight(y->left), getAssignmentHeight(y->right));
    return y;
}

AssignmentNode *createAssignment(AssignmentNode *root, int assignmentId, int userId, const char *title, const char *description) {
    if (!root)
        return createAssignmentNode(assignmentId, userId, title, description);
    
    if (assignmentId < root->assignment.assignmentId)
        root->left = createAssignment(root->left, assignmentId, userId, title, description);
    else if (assignmentId > root->assignment.assignmentId)
        root->right = createAssignment(root->right, assignmentId, userId, title, description);
    else {
        printf("Assignment ID already exists!\n");
        return root;
    }
    
    root->height = 1 + max(getAssignmentHeight(root->left), getAssignmentHeight(root->right));
    int balance = getAssignmentBalance(root);
    
    if (balance > 1 && assignmentId < root->left->assignment.assignmentId)
        return rightRotateAssignment(root);
    
    if (balance < -1 && assignmentId > root->right->assignment.assignmentId)
        return leftRotateAssignment(root);
    
    if (balance > 1 && assignmentId > root->left->assignment.assignmentId) {
        root->left = leftRotateAssignment(root->left);
        return rightRotateAssignment(root);
    }
    
    if (balance < -1 && assignmentId < root->right->assignment.assignmentId) {
        root->right = rightRotateAssignment(root->right);
        return leftRotateAssignment(root);
    }
    
    return root;
}

AssignmentNode *searchAssignment(AssignmentNode *root, int assignmentId) {
    if (!root) return NULL;
    if (assignmentId == root->assignment.assignmentId)
        return root;
    else if (assignmentId < root->assignment.assignmentId)
        return searchAssignment(root->left, assignmentId);
    else
        return searchAssignment(root->right, assignmentId);
}

AssignmentNode *findMinValueNode(AssignmentNode *node) {
    AssignmentNode *current = node;
    while (current && current->left != NULL)
        current = current->left;
    return current;
}

AssignmentNode *deleteAssignment(AssignmentNode *root, int assignmentId) {
    if (!root) return root;
    
    displayAllAssignments(root);
    if (assignmentId < root->assignment.assignmentId)
        root->left = deleteAssignment(root->left, assignmentId);
    else if (assignmentId > root->assignment.assignmentId)
        root->right = deleteAssignment(root->right, assignmentId);
    else {
        if (!root->left || !root->right) {
            AssignmentNode *temp = root->left ? root->left : root->right;
            
            if (!temp) {
                temp = root;
                root = NULL;
            } else { 
                *root = *temp; 
            }
            free(temp);
        } else {
            AssignmentNode *temp = findMinValueNode(root->right);
            
            root->assignment = temp->assignment;
            
            root->right = deleteAssignment(root->right, temp->assignment.assignmentId);
        }
    }
    
    if (!root) return NULL;
    
    root->height = 1 + max(getAssignmentHeight(root->left), getAssignmentHeight(root->right));
    
    int balance = getAssignmentBalance(root);
    
    if (balance > 1 && getAssignmentBalance(root->left) >= 0)
        return rightRotateAssignment(root);
    
    if (balance > 1 && getAssignmentBalance(root->left) < 0) {
        root->left = leftRotateAssignment(root->left);
        return rightRotateAssignment(root);
    }
    
    if (balance < -1 && getAssignmentBalance(root->right) <= 0)
        return leftRotateAssignment(root);
    
    if (balance < -1 && getAssignmentBalance(root->right) > 0) {
        root->right = rightRotateAssignment(root->right);
        return leftRotateAssignment(root);
    }
    
    return root;
}

AssignmentNode *updateAssignmentStatus(AssignmentNode *root, int assignmentId, const char *status) {
    AssignmentNode *node = searchAssignment(root, assignmentId);
    if (node) {
        strcpy(node->assignment.status, status);
        printf("Assignment status updated successfully!\n");
    } else {
        printf("Assignment not found!\n");
    }
    return root;
}

void displayAssignment(Assignment assignment) {
    printf("==================================\n");
    printf("Assignment ID: %d\n", assignment.assignmentId);
    printf("Assigned to User ID: %d\n", assignment.userId);
    printf("Title: %s\n", assignment.title);
    printf("Description: %s\n", assignment.description);
    printf("Status: %s\n", assignment.status);
    printf("==================================\n");
}

void inorderTraversal(AssignmentNode *root) {
    if (root) {
        inorderTraversal(root->left);
        displayAssignment(root->assignment);
        inorderTraversal(root->right);
    }
}

void displayAllAssignments(AssignmentNode *root) {
    if (!root) {
        printf("No assignments found!\n");
        return;
    }
    printf("\n--- All Assignments ---\n");
    inorderTraversal(root);
}

void inorderTraversalByUser(AssignmentNode *root, int userId) {
    if (root) {
        inorderTraversalByUser(root->left, userId);
        if (root->assignment.userId == userId) {
            displayAssignment(root->assignment);
        }
        inorderTraversalByUser(root->right, userId);
    }
}

void displayUserAssignments(AssignmentNode *root, int userId) {
    if (!root) {
        printf("No assignments found!\n");
        return;
    }
    printf("\n--- Assignments for User ID: %d ---\n", userId);
    inorderTraversalByUser(root, userId);
}

void saveUserInorder(AVLNode* root, FILE* file) {
    if (root) {
        saveUserInorder(root->left, file);
        
        fprintf(file, "%d,%s,%s,%s\n", 
                root->user.userId, 
                root->user.username, 
                root->user.password, 
                root->user.role);
        
        saveUserInorder(root->right, file);
    }
}

void saveUsersToFile(AVLNode* root, const char* filename) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        printf("Error opening file for writing: %s\n", filename);
        return;
    }
    
    saveUserInorder(root, file);
    fclose(file);
    printf("User data saved to %s successfully!\n", filename);
}

AVLNode* loadUsersFromFile(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("File not found or cannot be opened: %s\n", filename);
        return NULL;
    }
    
    AVLNode* root = NULL;
    char line[600];
    char username[50], password[50], role[20];
    int userId;
    
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0;
        
        if (sscanf(line, "%d,%[^,],%[^,],%[^,\n]", &userId, username, password, role) == 4) {
            root = insert(root, userId, username, password, role);
        }
    }
    
    fclose(file);
    printf("User data loaded from %s successfully!\n", filename);
    return root;
}

void saveAssignmentInorder(AssignmentNode* root, FILE* file) {
    if (root) {
        saveAssignmentInorder(root->left, file);
        
        fprintf(file, "%d,%d,%s,%s,%s\n", 
                root->assignment.assignmentId, 
                root->assignment.userId, 
                root->assignment.title, 
                root->assignment.description, 
                root->assignment.status);
        
        saveAssignmentInorder(root->right, file);
    }
}

void saveAssignmentsToFile(AssignmentNode* root, const char* filename) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        printf("Error opening file for writing: %s\n", filename);
        return;
    }
    
    saveAssignmentInorder(root, file);
    fclose(file);
    printf("Assignment data saved to %s successfully!\n", filename);
}

AssignmentNode* loadAssignmentsFromFile(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("File not found or cannot be opened: %s\n", filename);
        return NULL;
    }
    
    AssignmentNode* root = NULL;
    char line[1000];
    char title[100], description[500], status[20];
    int assignmentId, userId;
    
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0;
        
        char* token = strtok(line, ",");
        if (token) {
            assignmentId = atoi(token);
            
            token = strtok(NULL, ",");
            if (token) {
                userId = atoi(token);
                
                token = strtok(NULL, ",");
                if (token) {
                    strcpy(title, token);
                    
                    token = strtok(NULL, ",");
                    if (token) {
                        strcpy(description, token);
                        
                        token = strtok(NULL, ",");
                        if (token) {
                            strcpy(status, token);
                            
                            root = createAssignment(root, assignmentId, userId, title, description);
                            
                            if (strcmp(status, "pending") != 0) {
                                root = updateAssignmentStatus(root, assignmentId, status);
                            }
                        }
                    }
                }
            }
        }
    }
    
    fclose(file);
    printf("Assignment data loaded from %s successfully!\n", filename);
    return root;
}
