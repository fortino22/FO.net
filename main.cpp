#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h> 
#include "avl.h"
#include "task_management.h"



int main() {
    HWND console = GetConsoleWindow();
    ShowWindow(console, SW_MAXIMIZE);
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
    char input[10];
    do {
        printf("\n===== Task Management System =====\n");
        printf("1. Register\n");
        printf("2. Login\n");
        printf("0. Exit\n");
        printf("Enter your choice: ");
        fgets(input, sizeof(input), stdin);

        input[strcspn(input, "\n")] = 0;

        if (strlen(input) == 1 && (input[0] == '0' || input[0] == '1' || input[0] == '2')) {
            choice = input[0] - '0';
        } else {
            printf("Invalid choice. Please enter a number from the menu.\n");
            continue;
        }
        
        switch (choice) {
            case 1:
                system("cls");
                registerUser(&userRoot);
                saveUsersToFile(userRoot, CREDENTIALS_FILE); 
                break;
            case 2:
                system("cls");
                loginUser(userRoot, &assignmentRoot);
                break;
            case 0:
                system("cls");
                saveAllData(userRoot, assignmentRoot);
                printf("Thank you for using our system!\n");
                break;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    } while (choice != 0);
    
    return 0;
}
