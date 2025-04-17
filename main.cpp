#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "avl.h"

#define CREDENTIAL_FILE "credential.txt"

void menu() {
    printf("\n1. Register (auto as worker)\n");
    printf("2. Login\n");
    printf("3. Exit\n");
    printf(">> ");
}

void managerPage(const char* username) {
    printf("\nRedirecting to Manager Page...\n");
    printf("Hello, Manager %s!\n", username);
    printf("\n===== Manager Dashboard =====\n");
    printf("- Total workers: 5\n");
    printf("- Pending tasks: 3\n");
    printf("=============================\n");
}

void workerPage(const char* username) {
    printf("\nRedirecting to Worker Page...\n");
    printf("Hello, Worker %s!\n", username);
}

void saveCredential(const char* username, const char* password, const char* role) {
    FILE* fp = fopen(CREDENTIAL_FILE, "a");
    if (fp) {
        fprintf(fp, "%s,%s,%s\n", username, password, role);
        fclose(fp);
    } else {
        printf("Failed to open credential file for writing!\n");
    }
}

AVLNode* loadCredentials(AVLNode* root) {
    FILE* fp = fopen(CREDENTIAL_FILE, "r");
    if (!fp) return root; 

    char line[150];
    while (fgets(line, sizeof(line), fp)) {
        char* username = strtok(line, ",");
        char* password = strtok(NULL, ",");
        char* role = strtok(NULL, ",\n");
        if (username && password && role) {
            root = insert(root, username, password, role);
        }
    }

    fclose(fp);
    return root;
}

int main() {
    AVLNode *root = NULL;
    root = loadCredentials(root);  

    int choice;
    char username[50], password[50];

    while (1) {
        menu();
        scanf("%d", &choice);
        getchar(); 

        if (choice == 1) {
            printf("Enter username: ");
            scanf("%s", username);
            printf("Enter password: ");
            scanf("%s", password);

            const char* role = "worker"; 
            AVLNode* existing = search(root, username);
            if (existing) {
                printf("Username already exists. Please try another.\n");
            } else {
                root = insert(root, username, password, role);
                saveCredential(username, password, role);
                printf("User registered as 'worker'!\n");
            }

        } else if (choice == 2) {
            printf("Enter username: ");
            scanf("%s", username);
            printf("Enter password: ");
            scanf("%s", password);
            AVLNode *user = search(root, username);
            if (user && strcmp(user->user.password, password) == 0) {
                printf("Login successful! Welcome, %s\n", username);
                if (strcmp(user->user.role, "manager") == 0) {
                    managerPage(username);
                } else {
                    workerPage(username);
                }
            } else {
                printf("Login failed. Invalid credentials.\n");
            }
        } else if (choice == 3) {
            printf("Goodbye!\n");
            break;
        } else {
            printf("Invalid choice.\n");
        }
    }

    return 0;
}

