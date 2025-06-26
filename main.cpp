#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h> 
#include "avl.h"
#include "task_management.h"

#define RESET "\x1b[0m"
#define BRIGHT_WHITE "\x1b[97m"
#define BLUE "\x1b[34m"
#define CYAN "\x1b[36m"
#define WHITE "\x1b[37m"

void clr() {
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 100;
    info.bVisible = FALSE;
    COORD cursorPosition;
    cursorPosition.X = 0;
    cursorPosition.Y = 0;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cursorPosition);
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleCursorInfo(consoleHandle, &info);
  }
  void animateAscii() {
    const char *asciiArt[] = {
        "                                     *****#                             "
        "       ",
        "                           ****                  ,****                  "
        "        ",
        "                      **%   //*(((#####%%(####((((//    **(             "
        "        ",
        "                  **/  //((#%%&%&&&&&&%&&&&&&#&&&&&%(#(//   **          "
        "        ",
        "               %*   /(#(%&&&&&&&&&&&&&,,...&&&&&&&&&&&#&(#///  **       "
        "        ",
        "             **  /(#%&&&&&&&&&&&&&&,,,,,,,,...&&&&&#&&&&&&&%((/  **     "
        "        ",
        "           **  /(#&&&&&&&&&&&&&&&,,,,,,,,,,,,,..&&&&#&&&&&&&&&#(// **   "
        "        ",
        "          *  /(#&&&%#&&&&&&&&&,,,,,,,,,,,,,,,(((&&&&&&&&&&%%#&&&#(/  ** "
        "        ",
        "        ** *(#%%&&&&&&&&&&#&,,,,,,,#,,,,,,(((&&&&&&&&&&&&&&&&&&&&%#(/ "
        "%*        ",
        "       ** /(#&&&&&&&&&&&&,,,,,,,,,,,,,,,(((&&&&&&&&&&&&&&&&&&&&&&&&#(/  "
        "*       ",
        "      (* /(#&%#&&&&&&&&,,,,,,,,,,,,,,%((&&&&&,,,...&&&&&&&&&%&&&&##&#(/ "
        "%*      ",
        "      *  "
        "(#&&%&&&&&&,,,,,,,,%,,,,,,(((&&&&&,,,,,,,(..%&&&&&&&&&&&&&&&#(/ *#     ",
        "     ** "
        "/(%&&&&&&&,,,,,,,,,,,,,,%((&&&&&,,,,,,,,,,,,,...&&&&&&&&&&&&&%(/  *    "
        " ",
        "     ** "
        "/#%&&&&&&&&,,,,,,,,,,,,,,,(((&&&&,,,,,,,,,,,%,,(..%&&#&&&&&%&&#(  *    "
        " ",
        "     *  "
        "(#&&&&&&&&&&&%,,,,,(,,,,,,,,(((&&&&,,,,,,,,,,,,,,,...#&&&&&&&&#(/ *    "
        " ",
        "     ** "
        "/#%&&&&&&&&&&&&&%,,,,,,,,,,,,,,,,(((&&&&,%,,,,,,,,,,,,(..%&&&%&&#(  *   "
        "  ",
        "     ** "
        "/(%&&&&&&&&&&&&&&&%,,,,,,,,,,(((&&&&&,,,%,,,,,,,,,,,(((&&&&&&%(/  *    "
        " ",
        "      *  "
        "(#&&%&&&&&&&&&&&&&&&,,,,,%((%&&&&%,,,,,,,,,,,,,,(((&&&&&&&&&#(/ *      ",
        "      /* /(#&##&&&&&&&&&&&&&&&&%(((&&&&&,,,,,,,,,,,,,%,(((&&%&&&&%#&%(/ "
        "**      ",
        "       ** /(#&&&&&&&&&&&&&&&&&&&&&&&&%,,,%,,,,,,,,,,(((&&&&&&&&&&&&%(/  "
        "*       ",
        "        ** .(#&&&&&&&&&&&&#&&&&&&&&,,,,,,,,,,,,,,,(((&&&&&&&&&&&&&#(/ "
        "**        ",
        "          *  /(%&&&%#&&&&&&&&&&&%,,,,,,,,,,,,,%(((&&&&&&&&&%#&&%%(/  ** "
        "        ",
        "           **  ((%&&&&&&&&&&&&&&&&,,,,,,,,,,,(((&&&&%&&&&&&&&&%#(/ **   "
        "        ",
        "             **  /(#%&&&&&&&&&&&&&&&&,,,,,(((&&&&&&#&&&&&&&%((/  **     "
        "        ",
        "                *#/(#(&&&&&&&&&&&&&&&,(((&&&&&&&&&&&%&(#/(/  **         "
        "      ",
        "                  ***  /*(#%%&&#&&&&&&%&&&&&&%&&&&&%(#((/   **          "
        "        ",
        "                      ***   ///((###%%%%%(%%###((((/    **              "
        "        ",
        "                           ****                  %***                   "
        "       ",
        "                                     #*****                             "
        "         "};
  
    int numLines = sizeof(asciiArt) / sizeof(asciiArt[0]);
    int len = (int)strlen(asciiArt[0]);
    int steps = (len + 1) / 2;
  
    static char buffer[256];
  
    for (int step = 0; step <= steps; step += 2) {
      for (int i = 0; i < numLines; i++) {
        memset(buffer, ' ', len);
        buffer[len] = '\0';
  
        for (int j = 0; j < step && j < len; j++) {
          buffer[j] = asciiArt[i][j];
          if (buffer[j] == '*') {
            printf("%s%c", BRIGHT_WHITE, buffer[j]);
          } else if (buffer[j] == '&') {
            printf("%s%c", BLUE, buffer[j]);
          } else if (buffer[j] == ',') {
            printf("%s%c", CYAN, buffer[j]);
          } else {
            printf("%s%s%c", BRIGHT_WHITE, BRIGHT_WHITE, buffer[j]);
          }
        }
        for (int j = step; j < len - step; j++) {
          if (asciiArt[i][j] == ',') {
            printf("%s%c", CYAN, asciiArt[i][j]);
          } else {
            printf("%s%s ", BRIGHT_WHITE, BRIGHT_WHITE);
          }
        }
        for (int j = len - step; j < len; j++) {
          if (j >= 0 && j < len) {
            buffer[j] = asciiArt[i][j];
            if (buffer[j] == '*') {
              printf("%s%c", BRIGHT_WHITE, buffer[j]);
            } else if (buffer[j] == '&') {
              printf("%s%c", BLUE, buffer[j]);
            } else if (buffer[j] == ',') {
              printf("%s%c", CYAN, buffer[j]);
            } else {
              printf("%s%s%c", BRIGHT_WHITE, BRIGHT_WHITE, buffer[j]);
            }
          }
        }
        printf("%s\n", RESET);
      }
      if (step < steps) {
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        GetConsoleScreenBufferInfo(hConsole, &csbi);
        COORD pos = {0, csbi.dwCursorPosition.Y - numLines};
        SetConsoleCursorPosition(hConsole, pos);
      }
      Sleep(10);
    }
  
    printf("\n");
    printf("%s            "
           "===========================================================\n",
           CYAN);
    printf("            |      Confront the challenges of learning and outgrow   "
           " |\n");
    printf("            |              the boundaries together ~ 24-2            "
           " |\n");
    printf("            "
           "===========================================================%s\n",
           RESET);
  }
  

int main() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hConsole, &cursorInfo);
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(hConsole, &cursorInfo);

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
                // saveAllData(userRoot, assignmentRoot);
                animateAscii();
                Sleep(3000);
                exit(0);
                break;
        }
    } while (choice != 0);
    
    return 0;
}