#ifndef AVL_H
#define AVL_H
#include <stdlib.h>
#include <time.h>

int generateRandomId();

struct User {
    int userId;
    char username[50];
    char password[50];
    char role[20]; 
};

struct Assignment {
    int assignmentId;
    int userId; 
    char title[100];
    char description[500];
    char status[20]; 
};


struct AVLNode {
    User user;
    AVLNode *left, *right;
    int height;
};

struct AssignmentNode {
    Assignment assignment;
    AssignmentNode *left, *right;
    int height;
};

AVLNode* insert(AVLNode* root, int userId, const char* username, const char* password, const char* role);
AVLNode* search(AVLNode* root, const char* username);
AVLNode* searchById(AVLNode* root, int userId);
int getHeight(AVLNode* node);

AssignmentNode* createAssignment(AssignmentNode* root, int assignmentId, int userId, const char* title, const char* description);
AssignmentNode* updateAssignmentStatus(AssignmentNode* root, int assignmentId, const char* status);
AssignmentNode* deleteAssignment(AssignmentNode* root, int assignmentId);
AssignmentNode* searchAssignment(AssignmentNode* root, int assignmentId);
void displayAllAssignments(AssignmentNode* root);
void displayUserAssignments(AssignmentNode* root, int userId);
int getAssignmentHeight(AssignmentNode* node);

void saveUsersToFile(AVLNode* root, const char* filename);
AVLNode* loadUsersFromFile(const char* filename);
void saveAssignmentsToFile(AssignmentNode* root, const char* filename);
AssignmentNode* loadAssignmentsFromFile(const char* filename);

void saveUserInorder(AVLNode* root, FILE* file);
void saveAssignmentInorder(AssignmentNode* root, FILE* file);

#endif
