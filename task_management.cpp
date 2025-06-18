#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "task_management.h"
#include <conio.h>
#include "worldmap.h"

void getInput(char* buffer, int size, const char* prompt) {
    printf("%s", prompt);
    fgets(buffer, size, stdin);
    buffer[strcspn(buffer, "\n")] = 0; 
}

void getPasswordInput(char* buffer, int size, const char* prompt) {
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


void startAssignment(AssignmentNode** root, int workerId) {
    // Initialize the world map
    printf("\033[2J\033[H"); // Clear screen
    int numCountries = 5;
    int validCount = initializeWorld(numCountries);
    
    if (validCount <= 0) {
        printf("Failed to generate countries. Please try again.\n");
        return;
    }
    
    // Get all assignments for the worker
    int assignmentCount = 0;
    AssignmentNode* currentAssignments[10]; // Array to store pointers to assignments
    int selectedAssignment = 0; // Currently selected assignment index
    
    // Collect worker assignments
    collectUserAssignments(*root, workerId, currentAssignments, &assignmentCount);
    
    if (assignmentCount == 0) {
        printf("You don't have any assignments yet.\n");
        printf("Press Enter to return...");
        getchar();
        return;
    }
    
    // Start interactive mode with map and assignments
    char input;
    int running = 1;
    
    while (running) {
        // Print the map on the left side
        printf("\033[H"); // Move cursor to top
        for (int y = 0; y < HEIGHT; y++) {
            for (int x = 0; x < WIDTH/2; x++) { // Use half width for map
                int mapX = x * 2; // Scale x to maintain aspect ratio
                if (showPlayer && mapX == playerX && y == playerY) {
                    printf("\033[31mP\033[0m");
                } else if (mapX < WIDTH && map[y][mapX] == '#') {
                    printf("\033[32m#\033[0m");
                } else if (mapX < WIDTH && map[y][mapX] == '.') {
                    printf("\033[34m.\033[0m");
                } else if (mapX < WIDTH && map[y][mapX] == '+') {
                    printf("\033[33m+\033[0m");
                } else if (mapX < WIDTH && map[y][mapX] == '=') {
                    printf("\033[35m=\033[0m");
                } else {
                    putchar('.');
                }
            }
            
            // Print assignment details on the right side
            if (y == 1) printf("  | \033[1mYOUR ASSIGNMENTS\033[0m");
            else if (y == 2) printf("  | Use up/down arrows to select, u to update status");
            else if (y >= 4 && y < 4 + assignmentCount) {
                int idx = y - 4;
                if (idx == selectedAssignment) {
                    printf("  | \033[7m→ %d: %s (%s)\033[0m", 
                        currentAssignments[idx]->assignment.assignmentId, 
                        currentAssignments[idx]->assignment.title,
                        currentAssignments[idx]->assignment.status);
                } else {
                    printf("  | %d: %s (%s)", 
                        currentAssignments[idx]->assignment.assignmentId, 
                        currentAssignments[idx]->assignment.title,
                        currentAssignments[idx]->assignment.status);
                }
            }
            else if (y == 4 + assignmentCount + 2) printf("  | \033[1mCONTROLS:\033[0m");
            else if (y == 4 + assignmentCount + 3) printf("  | w/a/s/d - Move player");
            else if (y == 4 + assignmentCount + 4) printf("  | u - Update selected task status");
            else if (y == 4 + assignmentCount + 5) printf("  | Up/Down - Select assignment");
            else if (y == 4 + assignmentCount + 6) printf("  | q - Return to menu");
            
            printf("\n");
        }
        
        printf("Action (w/a/s/d to move, u to update, ↑/↓ to select, q to quit): ");
        input = _getch();
        
        if (input == 'q') {
            running = 0;
        } else if (input == 'u' && assignmentCount > 0) {
            // Update status of selected assignment
            char newStatus[20];
            printf("\033[%d;%dH", HEIGHT + 2, 0); // Move cursor below the map
            printf("                                                                              \n"); // Clear line
            printf("                                                                              \n"); // Clear line
            
            getValidStatus(newStatus, 20);
            
            // Update assignment status
            *root = updateAssignmentStatus(*root, currentAssignments[selectedAssignment]->assignment.assignmentId, newStatus);
            
            // Update our local cache
            currentAssignments[selectedAssignment] = searchAssignment(*root, currentAssignments[selectedAssignment]->assignment.assignmentId);
            
            printf("Status updated to: %s\n", newStatus);
            printf("Press any key to continue...");
            _getch();
            
            // Save changes
            saveAssignmentsToFile(*root, ASSIGNMENTS_FILE);
        } else if (input == 'w' || input == 'a' || input == 's' || input == 'd') {
            movePlayer(input);
        } else if (input == 224) { // Arrow key prefix
            input = _getch(); // Get the actual arrow key
            if (input == 72 && selectedAssignment > 0) { // Up arrow
                selectedAssignment--;
            } else if (input == 80 && selectedAssignment < assignmentCount - 1) { // Down arrow
                selectedAssignment++;
            }
        }
    }
    
    printf("\n\033[0mReturning to work menu...\n");
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
    
    getInput(username, 50, "Enter username: ");
    getPasswordInput(password, 50, "Enter password: ");
    
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
    
    getInput(username, 50, "Enter username: ");
    getPasswordInput(password, 50, "Enter password: ");
    
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
        printf("1. Start Assignment\n");
        printf("2. Update Task Status\n");
        printf("3. View My Tasks\n");
        printf("0. Logout\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        getchar(); 
        
        switch (choice) {
            case 1:
                startAssignment(assignmentRoot, workerId);
                break;
            case 2:
                updateTaskStatus(assignmentRoot, workerId);
                saveAssignmentsToFile(*assignmentRoot, ASSIGNMENTS_FILE);
                break;
            case 3:
                displayUserAssignments(*assignmentRoot, workerId);
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
    
    getInput(title, 100, "Enter task title: ");
    getInput(description, 500, "Enter task description: ");
    
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
    
    getValidStatus(status, 20);
    
    *root = updateAssignmentStatus(*root, assignmentId, status);
    printf("Task status updated successfully!\n");
}

void saveAllData(AVLNode* userRoot, AssignmentNode* assignmentRoot) {
    saveUsersToFile(userRoot, CREDENTIALS_FILE);
    
    saveAssignmentsToFile(assignmentRoot, ASSIGNMENTS_FILE);
    
    printf("All data has been saved successfully!\n");
}
