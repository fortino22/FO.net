#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "task_management.h"
#include <conio.h>
#include "worldmap.h"
#include "windows.h"
void getInput(char* buffer, int size, const char* prompt) {
    printf("%s", prompt);
    fgets(buffer, size, stdin);
    buffer[strcspn(buffer, "\n")] = 0; 
}

void getPasswordInput(char* buffer, int size, const char* prompt) {
    int valid = 0;
    while (!valid) {
        printf("%s", prompt);
        int i = 0;
        char ch;
        while (1) {
            ch = _getch();
            if (ch == 13) { 
                buffer[i] = '\0';
                break;
            } else if (ch == 8) { 
                if (i > 0) {
                    i--;
                    printf("\b \b");
                }
            } else if (ch == 27) { 
                buffer[0] = '\0';
                break;
            } else if (i < size - 1) {
                buffer[i++] = ch;
                printf("*");
            }
        }
        printf("\n");

        int hasUpper = 0, hasDigit = 0;
        for (int j = 0; buffer[j]; j++) {
            if (buffer[j] >= 'A' && buffer[j] <= 'Z') hasUpper = 1;
            if (buffer[j] >= '0' && buffer[j] <= '9') hasDigit = 1;
        }
        if (!hasUpper || !hasDigit) {
            printf("Password must contain at least one uppercase letter and one number. Please try again.\n");
        } else {
            valid = 1;
        }
    }
}

void getPasswordInputLogin(char* buffer, int size, const char* prompt) {
    int valid = 0;
    while (!valid) {
        printf("%s", prompt);
        int i = 0;
        char ch;
        while (1) {
            ch = _getch();
            if (ch == 13) { 
                buffer[i] = '\0';
                break;
            } else if (ch == 8) { 
                if (i > 0) {
                    i--;
                    printf("\b \b");
                }
            } else if (ch == 27) { 
                buffer[0] = '\0';
                break;
            } else if (i < size - 1) {
                buffer[i++] = ch;
                printf("*");
            }
        }
        printf("\n");

        int hasUpper = 0, hasDigit = 0;
        for (int j = 0; buffer[j]; j++) {
            if (buffer[j] >= 'A' && buffer[j] <= 'Z') hasUpper = 1;
            if (buffer[j] >= '0' && buffer[j] <= '9') hasDigit = 1;
        }
        if (!hasUpper || !hasDigit) {

            printf("Username or password wrong.\n");
            Sleep(100);
            system("cls");
            return; 
        } else {
            valid = 1;
        }
    }
}

void getValidStatus(char* buffer, int size) {
    int validStatus = 0;
    while (!validStatus) {
        getInput(buffer, size, "Enter new status (pending, in-progress, completed): ");
        
        if (strcmp(buffer, "pending") == 0 || 
            strcmp(buffer, "in-progress") == 0 || 
            strcmp(buffer, "completed") == 0) {
            validStatus = 1;
        } else {
            printf("Invalid status. Must be exactly 'pending', 'in-progress', or 'completed'.\n");
        }
    }
}



void collectUserAssignments(AssignmentNode* root, int userId, AssignmentNode* assignments[], int* count) {
    if (!root) return;
    
    collectUserAssignments(root->left, userId, assignments, count);
    
    if (root->assignment.userId == userId) {
        assignments[*count] = root;
        (*count)++;
    }
    
    collectUserAssignments(root->right, userId, assignments, count);
}

void registerUser(AVLNode** root) {
    char username[50], password[50];
    printf("\n===== Registration =====\n");

    while (1) {
        getInput(username, 50, "Enter username (press q to exit): ");
        if (strlen(username) == 1 && (username[0] == 'q' || username[0] == 'Q')) {
            printf("Registration cancelled.\n");
            return;
        }
        if (strlen(username) == 0) {
            printf("Username cannot be empty. Please try again.\n");
        } else if (strchr(username, ',')) {
            printf("Username cannot contain a comma (,). Please try again.\n");
        } else if (search(*root, username)) {
            printf("Username already exists. Please choose another.\n");
        } else {
            break;
        }
    }

    do {
        getPasswordInput(password, 50, "Enter password (press q to exit): ");
        if (strlen(password) == 1 && (password[0] == 'q' || password[0] == 'Q')) {
            printf("Registration cancelled.\n");
            return;
        }
        if (strlen(password) == 0) {
            printf("Password cannot be empty. Please try again.\n");
        } else if (strlen(password) < 8) {
            printf("Password must be at least 8 characters long. Please try again.\n");
        }
    } while (strlen(password) < 8);

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
    int loggedIn = 0;

    while (!loggedIn) {
        printf("\n===== Login =====\n");
        getInput(username, 50, "Enter username (press q to exit): ");
        if (strlen(username) == 1 && (username[0] == 'q' || username[0] == 'Q')) {
            system("cls");
            return;
        }
        getPasswordInputLogin(password, 50, "Enter password (press q to exit): ");
        if (strlen(password) == 1 && (password[0] == 'q' || password[0] == 'Q')) {
            system("cls");
            return;
        }

        AVLNode* user = search(root, username);
        if (user) {
            if (strcmp(user->user.password, password) == 0) {
                system("cls");
                if (strcmp(user->user.role, "manager") == 0) {
                    managerMenu(root, assignmentRoot, user->user.userId);
                } else {
                    workerMenu(root, assignmentRoot, user->user.userId);
                }
                loggedIn = 1;
            } else {
                printf("Username or password wrong.\n");
                Sleep(500);
                system("cls");
            }
        } else {
            printf("Username or password wrong.\n");
            Sleep(500);
            system("cls");
        }
    }
}

void managerMenu(AVLNode* root, AssignmentNode** assignmentRoot, int managerId) {
    char input[10];
    int choice;
    do {
        printf("\n===== Manager Menu =====\n");
        printf("1. Assign Task to Worker\n");
        printf("2. View All Tasks\n");
        printf("3. View All Users\n");
        printf("4. View Workers\n");
        printf("5. Delete Task\n");
        printf("6. Save All Data\n");
        printf("0. Logout (or press q)\n");
        printf("Enter your choice: ");
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = 0;

        if (strlen(input) == 1 && (input[0] == 'q' || input[0] == 'Q' || input[0] == '0')) {
            choice = 0;
        } else if (strlen(input) == 1 && input[0] >= '1' && input[0] <= '6') {
            choice = input[0] - '0';
        } else {
            printf("Invalid choice. Please try again.\n");
            continue;
        }

        switch (choice) {
            case 1:
                system("cls");
                assignTask(root, assignmentRoot, managerId);
                saveAssignmentsToFile(*assignmentRoot, ASSIGNMENTS_FILE);
                break;
            case 2:
                system("cls");
                displayAllAssignments(*assignmentRoot);
                break;
            case 3:
                system("cls");
                viewAllUsers(root);
                break;
            case 4:
                system("cls");
                viewWorkers(root);
                break;
                case 5: {
                    system("cls");
                    printf("\n--- All Available Assignments ---\n");
                    displayAllAssignments(*assignmentRoot);
                
                    char input[32];
                    printf("Enter Assignment ID to delete (or 'q' to cancel): ");
                    fgets(input, sizeof(input), stdin);
                    input[strcspn(input, "\n")] = 0;
                    if (strlen(input) == 1 && (input[0] == 'q' || input[0] == 'Q')) {
                        printf("Operation cancelled.\n");
                        system("cls");
                        break;
                    }
                    int assignmentId = atoi(input);
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
        printf("1. Start Assignment\n");
        printf("2. Update Task Status\n");
        printf("3. View My Tasks\n");
        printf("0. Logout\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        getchar(); 
        
        switch (choice) {
            case 1:
                system("cls");
                startAssignment(assignmentRoot, workerId);
                break;
            case 2:
                system("cls");
                if (updateTaskStatus(assignmentRoot, workerId)) {
                    saveAssignmentsToFile(*assignmentRoot, ASSIGNMENTS_FILE);
                }
                break;
            case 3:
                system("cls");
                displayUserAssignments(*assignmentRoot, workerId);
                break;
            case 0:
                system("cls");
                printf("Logging out...\n");
                break;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    } while (choice != 0);
}

void assignTask(AVLNode* root, AssignmentNode** assignmentRoot, int managerId) {
    int workerId;
    char title[100], description[500], input[50];

    printf("\n===== Assign Task =====\n");
    printf("Press 'q' at any prompt to cancel this operation.\n");

    viewWorkers(root);

    printf("Enter Worker's User ID: ");
    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = 0;
    if (strlen(input) == 1 && (input[0] == 'q' || input[0] == 'Q')) {
        printf("Operation cancelled.\n");
        return;
    }
    workerId = atoi(input);

    AVLNode* worker = searchById(root, workerId);
    if (!worker || strcmp(worker->user.role, "worker") != 0) {
        printf("Invalid worker ID or user is not a worker.\n");
        return;
    }

    getInput(title, 100, "Enter task title: ");
    if (strlen(title) == 1 && (title[0] == 'q' || title[0] == 'Q')) {
        printf("Operation cancelled.\n");
        return;
    }

    getInput(description, 500, "Enter task description: ");
    if (strlen(description) == 1 && (description[0] == 'q' || description[0] == 'Q')) {
        printf("Operation cancelled.\n");
        return;
    }

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

int updateTaskStatus(AssignmentNode** root, int workerId) {
    int assignmentId;
    char status[20];
    char input[32];

    displayUserAssignments(*root, workerId);

    while (1) {
        printf("\nEnter Assignment ID to update: ");
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = 0;

        int valid = 1;
        if (strlen(input) == 0) valid = 0;
        for (int i = 0; input[i]; i++) {
            if (input[i] < '0' || input[i] > '9') valid = 0;
        }
        if (valid) {
            assignmentId = atoi(input);
            break;
        } else {
            printf("Invalid input. Please enter a valid numeric Assignment ID.\n");
        }
    }

    AssignmentNode* assignment = searchAssignment(*root, assignmentId);
    if (!assignment || assignment->assignment.userId != workerId) {
        printf("Invalid assignment ID.\n");
        return 0;
    }

    printf("Current status: %s\n", assignment->assignment.status);

    getValidStatus(status, 20);

    if (strcmp(assignment->assignment.status, status) == 0) {
        printf("Status is already '%s'. No update performed.\n", status);
        return 0;
    }

    *root = updateAssignmentStatus(*root, assignmentId, status);
    printf("Task status updated successfully!\n");
    return 1; 
}

void saveAllData(AVLNode* userRoot, AssignmentNode* assignmentRoot) {
    saveUsersToFile(userRoot, CREDENTIALS_FILE);
    
    saveAssignmentsToFile(assignmentRoot, ASSIGNMENTS_FILE);
    
    printf("All data has been saved successfully!\n");
}
