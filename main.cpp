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
    
    int choice = -1;
    char input[10];
    do {
        system("cls");
        printf(
            "      _____         _____         _____   ______        ______   _________________ \n"
            " ____|\\    \\   ____|\\    \\       |\\    \\ |\\     \\   ___|\\     \\ /                 \\\n"
            "|    | \\    \\ /     /\\    \\       \\\\    \\| \\     \\ |     \\     \\\\______     ______/\n"
            "|    |______//     /  \\    \\       \\|    \\  \\     ||     ,_____/|  \\( /    /  )/   \n"
            "|    |----'\\|     |    |    |       |     \\  |    ||     \\--'\\_|/   ' |   |   '    \n"
            "|    |_____/|     |    |    |       |      \\ |    ||     /___/|       |   |        \n"
            "|    |      |\\     \\  /    /|       |    |\\ \\|    ||     \\____|\\     /   //        \n"
            "|____|      | \\_____\\/____/ |  ___  |____||\\_____/||____ '     /|   /___//         \n"
            "|    |       \\ |    ||    | / |   | |    |/ \\|   |||    /_____/ |  |`   |          \n"
            "|____|        \\|____||____|/  |___| |____|   |___|/|____|     | /  |____|          \n"
            "  )/             \\(    )/             \\(       )/    \\( |_____|/     \\(            \n"
            "  '               '    '               '       '      '    )/         '            \n"
        );
        printf("1. Register\n");
        printf("2. Login\n");
        printf("0. Exit\n");
        printf("Enter your choice: ");
        
        while (1) {
            fgets(input, sizeof(input), stdin);
            input[strcspn(input, "\n")] = 0;
            
            if (strlen(input) == 1 && (input[0] == '0' || input[0] == '1' || input[0] == '2')) {
                choice = input[0] - '0';
                break; 
            } else {
                printf("Invalid choice. Please enter a number from the menu (0, 1, or 2): ");
            }
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
        }
    } while (choice != 0);
    
    return 0;
}