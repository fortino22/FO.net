#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "task_management.h"
#include <conio.h>
#include "worldmap.h"


bool initializeGameEnvironment(int* validCount) {
    static int worldGenerated = 0; 

    if (!worldGenerated) {
        printf("\033[2J\033[H");
        int numCountries = 5;
        *validCount = initializeWorld(numCountries);

        if (*validCount <= 0) {
            printf("Failed to generate countries. Please try again.\n");
            return false;
        }
        worldGenerated = 1;
    }
    return true;
}

void displayGameInterface(AssignmentNode** assignments, int assignmentCount) {
  printf("\033[H");
  for (int y = 0; y < HEIGHT; y++) {
      for (int x = 0; x < WIDTH; x++) {
          if (showPlayer && x == playerX && y == playerY) {
              printf("\033[31mP\033[0m");
          } else if (x < WIDTH && y < HEIGHT && map[y][x] == '#') {
              printf("\033[32m#\033[0m");
          } else if (x < WIDTH && y < HEIGHT && map[y][x] == '.') {
              printf("\033[34m.\033[0m");
          } else if (x < WIDTH && y < HEIGHT && map[y][x] == '+') {
              printf("\033[33m+\033[0m");
          } else if (x < WIDTH && y < HEIGHT && map[y][x] == '=') {
              printf("\033[35m=\033[0m");
          } else {
              putchar('.');
          }
      }
      printf("\n");
  }
  
  printf("\n\033[1m--- YOUR ASSIGNMENTS ---\033[0m\n");
  for (int i = 0; i < assignmentCount; i++) {
      printf("ID: %d | Title: %s | Status: %s\n", 
             assignments[i]->assignment.assignmentId, 
             assignments[i]->assignment.title,
             assignments[i]->assignment.status);
  }
}

int handleNavigationMode(char* input) {
  printf("\n\033[1m--- CONTROLS ---\033[0m\n");
  printf("WASD - Move player | ESC - Exit navigation mode | Q - Return to menu\n");
  printf("\nAction: ");
  *input = _getch();
  
  if (*input == 'q') {
      return 0;
  } else if (*input == 27) {
      return 2;
  } else if (*input == 'w' || *input == 'a' || *input == 's' || *input == 'd') {
      movePlayer(*input, 5);
  }
  return 1;
}

int handleUpdateMode(AssignmentNode** root, AssignmentNode** currentAssignments, 
                     int assignmentCount, char* input) {
  printf("\n\033[1m--- CONTROLS ---\033[0m\n");
  printf("Enter assignment ID to update status, or press ESC to return to navigation mode\n");
  printf("\nEnter assignment ID (or press ESC to go back): ");
  
  *input = _getch();
  if (*input == 27) {
      system("cls");
      return 1;
  }
  
  char idStr[10] = {0};
  idStr[0] = *input;
  printf("%c", *input);
  
  fgets(idStr + 1, 9, stdin);
  idStr[strcspn(idStr, "\n")] = 0;
  
  int assignmentId = atoi(idStr);
  return processAssignmentUpdate(root, currentAssignments, assignmentCount, assignmentId);
}

int processAssignmentUpdate(AssignmentNode** root, AssignmentNode** currentAssignments, 
                            int assignmentCount, int assignmentId) {
  AssignmentNode* assignment = NULL;
  int selectedIndex = -1;
  
  for (int i = 0; i < assignmentCount; i++) {
      if (currentAssignments[i]->assignment.assignmentId == assignmentId) {
          assignment = currentAssignments[i];
          selectedIndex = i;
          break;
      }
  }
  
  if (assignment) {
      printf("Current status: %s\n", assignment->assignment.status);
      
      char newStatus[20];
      getValidStatus(newStatus, 20);
      
      *root = updateAssignmentStatus(*root, assignmentId, newStatus);
      
      currentAssignments[selectedIndex] = searchAssignment(*root, assignmentId);
      
      printf("Status updated to: %s\n", newStatus);
      printf("Press Enter to continue...");
      getchar();
      
      saveAssignmentsToFile(*root, ASSIGNMENTS_FILE);
      
      system("cls");
      return 1;
  } else {
        printf("Invalid assignment ID. Press Enter to continue...");
        int c;
        while ((c = getchar()) != '\n' && c != EOF) {}
        return 2;
}
}

void startAssignment(AssignmentNode** root, int workerId) {
    system("cls");
  int validCount;
  
  if (!initializeGameEnvironment(&validCount)) {
      return;
  }
  
  int assignmentCount = 0;
  AssignmentNode* currentAssignments[10];
  collectUserAssignments(*root, workerId, currentAssignments, &assignmentCount);
  
  if (assignmentCount == 0) {
      printf("You don't have any assignments yet.\n");
      printf("Press Enter to return...");
      getchar();
      return;
  }
  
  char input;
  int running = 1;
  int navigationMode = 1;
  
  while (running) {
      displayGameInterface(currentAssignments, assignmentCount);
      
      if (navigationMode) {
          int result = handleNavigationMode(&input);
          if (result == 0) {
              running = 0;
          } else if (result == 2) {
              navigationMode = 0;
          }
      } else {
          int result = handleUpdateMode(root, currentAssignments, assignmentCount, &input);
          if (result == 1) {
              navigationMode = 1;
          }else if(result == 2){
            system("cls");
          }
      }
  }
  
  printf("\n\033[0mReturning to work menu...\n");
}
