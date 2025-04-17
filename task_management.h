#ifndef TASK_MANAGEMENT_H
#define TASK_MANAGEMENT_H
#include "avl.h"
#define CREDENTIALS_FILE "credentials.txt"
#define ASSIGNMENTS_FILE "assignments.txt"


void registerUser(AVLNode** root);

void loginUser(AVLNode* root, AssignmentNode** assignmentRoot);

void managerMenu(AVLNode* root, AssignmentNode** assignmentRoot, int managerId);


void workerMenu(AVLNode* root, AssignmentNode** assignmentRoot, int workerId);


void assignTask(AVLNode* root, AssignmentNode** assignmentRoot, int managerId);

void viewAllUsers(AVLNode* root);


void viewWorkers(AVLNode* root);


void viewAssignedTasks(AssignmentNode* root, int userId);

void updateTaskStatus(AssignmentNode** root, int workerId);


void saveAllData(AVLNode* userRoot, AssignmentNode* assignmentRoot);


void inorderTraversalUsers(AVLNode* root);

void inorderTraversalWorkers(AVLNode* root);

#endif 
