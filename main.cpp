#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "avl.h"
#include "task_management.h"


int main() {
    AVLNode* userRoot = NULL;
    AssignmentNode* assignmentRoot = NULL;
    
    userRoot = loadUsersFromFile(CREDENTIALS_FILE);
    assignmentRoot = loadAssignmentsFromFile(ASSIGNMENTS_FILE);
    
    if (!userRoot) {
        int adminId = generateRandomId();
        userRoot = insert(userRoot, adminId, "admin", "admin123", "manager");
        printf("Created default admin user (username: admin, password: admin123)\n");
    }
    
    int choice;
    do {
        printf("\n===== Task Management System =====\n");
        printf("1. Register\n");
        printf("2. Login\n");
        printf("0. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        getchar(); 
        
        switch (choice) {
            case 1:
                registerUser(&userRoot);
                saveUsersToFile(userRoot, CREDENTIALS_FILE); 
                break;
            case 2:
                loginUser(userRoot, &assignmentRoot);
                break;
            case 0:
                saveAllData(userRoot, assignmentRoot);
                printf("Thank you for using our system!\n");
                break;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    } while (choice != 0);
    
    return 0;
}



