#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "task_management.h"

void registerUser(AVLNode** root) {
    char username[50], password[50];
    printf("\n===== Registration =====\n");
    
    printf("Enter username: ");
    fgets(username, 50, stdin);
    username[strcspn(username, "\n")] = 0; 
    
    printf("Enter password: ");
    fgets(password, 50, stdin);
    password[strcspn(password, "\n")] = 0; 
    
    char role[20] = "worker";
    
    if (search(*root, username)) {
        printf("Username already exists. Please choose another.\n");
        return;
    }
    
    int userId = generateRandomId();
    
    *root = insert(*root, userId, username, password, role);
    printf("Registration successful! Your User ID is: %d\n", userId);
    printf("You have been registered as a worker.\n");
}

void loginUser(AVLNode* root, AssignmentNode** assignmentRoot) {
    char username[50], password[50];
    printf("\n===== Login =====\n");
    
    printf("Enter username: ");
    fgets(username, 50, stdin);
    username[strcspn(username, "\n")] = 0; 
    
    printf("Enter password: ");
    fgets(password, 50, stdin);
    password[strcspn(password, "\n")] = 0; 
    
    AVLNode* user = search(root, username);
    if (user && strcmp(user->user.password, password) == 0) {
        printf("Login successful!\n");
        
        if (strcmp(user->user.role, "manager") == 0) {
            managerMenu(root, assignmentRoot, user->user.userId);
        } else {
            workerMenu(root, assignmentRoot, user->user.userId);
        }
    } else {
        printf("Invalid username or password.\n");
    }
}

void managerMenu(AVLNode* root, AssignmentNode** assignmentRoot, int managerId) {
    int choice;
    do {
        printf("\n===== Manager Menu =====\n");
        printf("1. Assign Task to Worker\n");
        printf("2. View All Tasks\n");
        printf("3. View All Users\n");
        printf("4. View Workers\n");
        printf("5. Delete Task\n");
        printf("6. Save All Data\n");
        printf("0. Logout\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        getchar(); 
        
        switch (choice) {
            case 1:
                assignTask(root, assignmentRoot, managerId);
                saveAssignmentsToFile(*assignmentRoot, ASSIGNMENTS_FILE);
                break;
            case 2:
                displayAllAssignments(*assignmentRoot);
                break;
            case 3:
                viewAllUsers(root);
                break;
            case 4:
                viewWorkers(root);
                break;
            case 5: {
                printf("\n--- All Available Assignments ---\n");
                displayAllAssignments(*assignmentRoot);
                int assignmentId;
                printf("Enter Assignment ID to delete: ");
                scanf("%d", &assignmentId);
                getchar(); 
                *assignmentRoot = deleteAssignment(*assignmentRoot, assignmentId);
                printf("Assignment deleted successfully (if it existed).\n");
                saveAssignmentsToFile(*assignmentRoot, ASSIGNMENTS_FILE); 
                break;
            }
            case 6:
                saveAllData(root, *assignmentRoot);
                break;
            case 0:
                printf("Logging out...\n");
                break;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    } while (choice != 0);
}

void workerMenu(AVLNode* root, AssignmentNode** assignmentRoot, int workerId) {
    int choice;
    do {
        printf("\n===== Worker Menu =====\n");
        printf("1. View My Tasks\n");
        printf("2. Update Task Status\n");
        printf("0. Logout\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        getchar(); 
        
        switch (choice) {
            case 1:
                displayUserAssignments(*assignmentRoot, workerId);
                break;
            case 2:
                updateTaskStatus(assignmentRoot, workerId);
                saveAssignmentsToFile(*assignmentRoot, ASSIGNMENTS_FILE);
                break;
            case 0:
                printf("Logging out...\n");
                break;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    } while (choice != 0);
}

void assignTask(AVLNode* root, AssignmentNode** assignmentRoot, int managerId) {
    int workerId;
    char title[100], description[500];
    
    printf("\n===== Assign Task =====\n");
    
    viewWorkers(root);
    
    printf("Enter Worker's User ID: ");
    scanf("%d", &workerId);
    getchar();
    
    AVLNode* worker = searchById(root, workerId);
    if (!worker || strcmp(worker->user.role, "worker") != 0) {
        printf("Invalid worker ID or user is not a worker.\n");
        return;
    }
    
    printf("Enter task title: ");
    fgets(title, 100, stdin);
    title[strcspn(title, "\n")] = 0; 
    
    printf("Enter task description: ");
    fgets(description, 500, stdin);
    description[strcspn(description, "\n")] = 0; 
    
    int assignmentId = generateRandomId();
    
    *assignmentRoot = createAssignment(*assignmentRoot, assignmentId, workerId, title, description);
    printf("Task assigned successfully! Assignment ID: %d\n", assignmentId);
}

void viewAllUsers(AVLNode* root) {
    if (!root) {
        printf("No users found!\n");
        return;
    }
    printf("\n--- All Users ---\n");
    inorderTraversalUsers(root);
}

void inorderTraversalUsers(AVLNode* root) {
    if (root) {
        inorderTraversalUsers(root->left);
        printf("User ID: %d, Username: %s, Role: %s\n", 
               root->user.userId, root->user.username, root->user.role);
        inorderTraversalUsers(root->right);
    }
}

void viewWorkers(AVLNode* root) {
    if (!root) {
        printf("No users found!\n");
        return;
    }
    printf("\n--- All Workers ---\n");
    inorderTraversalWorkers(root);
}

void inorderTraversalWorkers(AVLNode* root) {
    if (root) {
        inorderTraversalWorkers(root->left);
        if (strcmp(root->user.role, "worker") == 0) {
            printf("Worker ID: %d, Username: %s\n", 
                   root->user.userId, root->user.username);
        }
        inorderTraversalWorkers(root->right);
    }
}

void viewAssignedTasks(AssignmentNode* root, int userId) {
    displayUserAssignments(root, userId);
}

void updateTaskStatus(AssignmentNode** root, int workerId) {
    int assignmentId;
    char status[20];
    int validStatus = 0;
    
    displayUserAssignments(*root, workerId);
    
    printf("\nEnter Assignment ID to update: ");
    scanf("%d", &assignmentId);
    getchar(); 
    
    AssignmentNode* assignment = searchAssignment(*root, assignmentId);
    if (!assignment || assignment->assignment.userId != workerId) {
        printf("Invalid assignment ID or you don't have permission to update this task.\n");
        return;
    }
    
    printf("Current status: %s\n", assignment->assignment.status);
    
    while (!validStatus) {
        printf("Enter new status (pending, in-progress, completed): ");
        fgets(status, 20, stdin);
        status[strcspn(status, "\n")] = 0; 
        
        if (strcmp(status, "pending") == 0 || 
            strcmp(status, "in-progress") == 0 || 
            strcmp(status, "completed") == 0) {
            validStatus = 1;
        } else {
            printf("Invalid status. Must be exactly 'pending', 'in-progress', or 'completed'.\n");
        }
    }
    
    *root = updateAssignmentStatus(*root, assignmentId, status);
    printf("Task status updated successfully!\n");
}

void saveAllData(AVLNode* userRoot, AssignmentNode* assignmentRoot) {
    saveUsersToFile(userRoot, CREDENTIALS_FILE);
    
    saveAssignmentsToFile(assignmentRoot, ASSIGNMENTS_FILE);
    
    printf("All data has been saved successfully!\n");
}
