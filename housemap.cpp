#include "citymap.h"
#include "housemap.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
int _getch() {
    struct termios oldt, newt;
    int ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}
#endif

HouseNode* houseRoot = NULL;

static int housemap_max(int a, int b) { return (a > b) ? a : b; }
int getHeight(HouseNode* node) { return node ? node->height : 0; }
int getBalance(HouseNode* node) { return node ? getHeight(node->left) - getHeight(node->right) : 0; }

HouseNode* rightRotate(HouseNode* y) {
    HouseNode* x = y->left;
    HouseNode* T2 = x->right;
    x->right = y;
    y->left = T2;
    y->height = 1 + housemap_max(getHeight(y->left), getHeight(y->right));
    x->height = 1 + housemap_max(getHeight(x->left), getHeight(x->right));
    return x;
}
HouseNode* leftRotate(HouseNode* x) {
    HouseNode* y = x->right;
    HouseNode* T2 = y->left;
    y->left = x;
    x->right = T2;
    x->height = 1 + housemap_max(getHeight(x->left), getHeight(x->right));
    y->height = 1 + housemap_max(getHeight(y->left), getHeight(y->right));
    return y;
}

HouseNode* insertHouse(HouseNode* node, House house) {
    if (!node) {
        HouseNode* newNode = (HouseNode*)malloc(sizeof(HouseNode));
        newNode->house = house;
        newNode->left = newNode->right = NULL;
        newNode->height = 1;
        return newNode;
    }
    
    if (house.id < node->house.id)
        node->left = insertHouse(node->left, house);
    else if (house.id > node->house.id)
        node->right = insertHouse(node->right, house);
    else
        return node; // Already exists

    node->height = 1 + housemap_max(getHeight(node->left), getHeight(node->right));
    int balance = getBalance(node);

    if (balance > 1 && house.id < node->left->house.id)
        return rightRotate(node);
    if (balance < -1 && house.id > node->right->house.id)
        return leftRotate(node);
    if (balance > 1 && house.id > node->left->house.id) {
        node->left = leftRotate(node->left);
        return rightRotate(node);
    }
    if (balance < -1 && house.id < node->right->house.id) {
        node->right = rightRotate(node->right);
        return leftRotate(node);
    }
    return node;
}
HouseNode* searchHouse(HouseNode* root, int gridX, int gridY) {
    if (!root) return NULL;
    
    // Check if this node matches the coordinates
    if (root->house.gridX == gridX && root->house.gridY == gridY)
        return root;
    
    // Search both subtrees since we can't determine which side has our coordinates
    HouseNode* leftResult = searchHouse(root->left, gridX, gridY);
    if (leftResult) return leftResult;
    
    return searchHouse(root->right, gridX, gridY);
}
HouseNode* searchHouseById(HouseNode* root, int id) {
    if (!root) return NULL;
    
    if (id == root->house.id)
        return root;
    else if (id < root->house.id)
        return searchHouseById(root->left, id);
    else
        return searchHouseById(root->right, id);
}
HouseNode* searchHouseInChunk(HouseNode* root, int chunkX, int chunkY, int gridX, int gridY) {
    if (!root) return NULL;
    
    int id = chunkY * 10000 + chunkX * 100 + gridY * 10 + gridX;
    return searchHouseById(root, id);
}
HouseNode* copyHouseTree(HouseNode* root) {
    if (!root) return NULL;
    HouseNode* newNode = (HouseNode*)malloc(sizeof(HouseNode));
    newNode->house = root->house;
    newNode->left = copyHouseTree(root->left);
    newNode->right = copyHouseTree(root->right);
    newNode->height = root->height;
    return newNode;
}

void freeHouseTree(HouseNode* root) {
    if (!root) return;
    freeHouseTree(root->left);
    freeHouseTree(root->right);
    free(root);
}


#define COLOR_RESET   "\033[0m"
#define COLOR_CURSOR  "\033[1;32m"
#define COLOR_HOUSE   "\033[1;34m"
#define COLOR_EMPTY   "\033[0;37m"

void drawGrid(int cursorX, int cursorY, HouseNode* root, int gridW, int gridH) {
    system("cls"); 
    printf("Use WASD to move, ENTER to place/show, Q to quit, L to list all houses.\n\n");
    for (int y = 0; y < gridH; ++y) {
        for (int x = 0; x < gridW; ++x) {
            if (x == cursorX && y == cursorY)
                printf(COLOR_CURSOR "[");
            else
                printf(" ");
            HouseNode* node = searchHouse(root, x, y);
            if (node)
                printf(COLOR_HOUSE "H" COLOR_RESET);
            else if (x == cursorX && y == cursorY)
                printf(COLOR_CURSOR "X" COLOR_RESET);
            else
                printf(COLOR_EMPTY "." COLOR_RESET);
            if (x == cursorX && y == cursorY)
                printf(COLOR_CURSOR "]" COLOR_RESET);
            else
                printf(" ");
        }
        printf("\n");
    }
}

void saveHouseInorder(HouseNode* root, FILE* file) {
    if (root) {
        saveHouseInorder(root->left, file);
        fprintf(file, "%d,%s,%s,%d,%d,%d,%s\n",
            root->house.id, root->house.name, root->house.owner,
            root->house.gridX, root->house.gridY,
            root->house.electricPower, root->house.internetSpeed);
        saveHouseInorder(root->right, file);
    }
}


void printAllHouses(HouseNode* root) {
    if (!root) return;
    printAllHouses(root->left);
    printf("Name: %s | Owner: %s | Location: %s | Electric: %dW | Internet: %dMbps\n",
        root->house.name, root->house.owner, root->house.location,
        root->house.electricPower, root->house.internetSpeed,
        root->house.gridX, root->house.gridY);
    printAllHouses(root->right);
}




void housePlacementLoopForChunk(Chunk* cityChunk) {
    int cursorX = 0, cursorY = 0;
    char name[100], owner[100], location[100];

    if (!cityChunk->houseRoot) cityChunk->houseRoot = NULL;

    int gridW = cityChunk->width;
    int gridH = cityChunk->height;

    while (1) {
        drawGrid(cursorX, cursorY, cityChunk->houseRoot, gridW, gridH);

        int ch = _getch();
        if (ch == 'q' || ch == 'Q') break;
        else if (ch == 'w' || ch == 'W') { if (cursorY > 0) cursorY--; }
        else if (ch == 's' || ch == 'S') { if (cursorY < gridH-1) cursorY++; }
        else if (ch == 'a' || ch == 'A') { if (cursorX > 0) cursorX--; }
        else if (ch == 'd' || ch == 'D') { if (cursorX < gridW-1) cursorX++; }
        else if (ch == 'l' || ch == 'L') {
            printf("\n--- All Houses in This City ---\n");
            printAllHouses(cityChunk->houseRoot);
            printf("Press any key to continue...\n");
            _getch();
        }
        else if (ch == 13) { // ENTER key
            HouseNode* existing = searchHouseInChunk(cityChunk->houseRoot, 
                cityChunk->x, cityChunk->y,
                cursorX, cursorY);
                if (existing) {
                printf("\n--- House Detail ---\n");
                printf("Name: %s\n", existing->house.name);
                printf("Owner: %s\n", existing->house.owner);
                printf("Location: %s\n", existing->house.location);
                printf("Electric Power: %d Watt\n", existing->house.electricPower);
                printf("Internet Speed: %d Mbps\n", existing->house.internetSpeed);
                // printf("Grid: (%d,%d)\n", existing->house.gridX, existing->house.gridY);
                printf("Press any key to continue...\n");
                _getch();
            } else {
                printf("Enter house name: ");
                fgets(name, sizeof(name), stdin);
                name[strcspn(name, "\n")] = 0;
                printf("Enter owner name: ");
                fgets(owner, sizeof(owner), stdin);
                owner[strcspn(owner, "\n")] = 0;
                printf("Enter location: ");
                fgets(location, sizeof(location), stdin);
                location[strcspn(location, "\n")] = 0;

                int electricPower = 0;
                printf("Enter electric power (Watt): ");
                scanf("%d", &electricPower);
                while(getchar() != '\n');

                int internetSpeed = 0;
                printf("Enter internet speed (Mbps): ");
                scanf("%d", &internetSpeed);
                while(getchar() != '\n');

                House house;
                house.id = cityChunk->y * 10000 + cityChunk->x * 100 + cursorY * 10 + cursorX;
                strcpy(house.name, name);
                strcpy(house.owner, owner);
                strcpy(house.location, location);
                house.gridX = cursorX;
                house.gridY = cursorY;
                house.electricPower = electricPower;
                house.internetSpeed = internetSpeed;

                cityChunk->houseRoot = insertHouse(cityChunk->houseRoot, house);
            }
        }
    }
}




void housePlacementLoop() {
    int cursorX = 0, cursorY = 0;
    char name[100], owner[100];
    drawGrid(cursorX, cursorY, houseRoot, GRID_COLS, GRID_ROWS);

    while (1) {
        int ch = _getch();
        if (ch == 'q' || ch == 'Q') break;
        else if (ch == 'w' || ch == 'W') { if (cursorY > 0) cursorY--; }
        else if (ch == 's' || ch == 'S') { if (cursorY < GRID_ROWS-1) cursorY++; }
        else if (ch == 'a' || ch == 'A') { if (cursorX > 0) cursorX--; }
        else if (ch == 'd' || ch == 'D') { if (cursorX < GRID_COLS-1) cursorX++; }
        else if (ch == 13) { // ENTER key
            HouseNode* existing = searchHouse(houseRoot, cursorX, cursorY);
            if (existing) {
                printf("\n--- House Detail ---\n");
                printf("Name: %s\n", existing->house.name);
                printf("Owner: %s\n", existing->house.owner);
                printf("Electric Power: %d Watt\n", existing->house.electricPower);
                printf("Internet Speed: %d Mbps\n", existing->house.internetSpeed);
                printf("Press any key to continue...\n");
                _getch();
            } else {
                printf("Enter house name: ");
                fgets(name, sizeof(name), stdin);
                name[strcspn(name, "\n")] = 0;
                printf("Enter owner name: ");
                fgets(owner, sizeof(owner), stdin);
                owner[strcspn(owner, "\n")] = 0;

                int electricPower = 0;
                printf("Enter electric power (Watt): ");
                scanf("%d", &electricPower);
                while(getchar() != '\n');

                int internetSpeed = 0;
                printf("Enter internet speed (Mbps): ");
                scanf("%d", &internetSpeed);
                while(getchar() != '\n');

                House house;
                house.id = cursorY * GRID_COLS + cursorX;
                strcpy(house.name, name);
                strcpy(house.owner, owner);
                house.gridX = cursorX;
                house.gridY = cursorY;
                house.electricPower = electricPower;
                house.internetSpeed = internetSpeed;
                houseRoot = insertHouse(houseRoot, house);
            }
        }
        drawGrid(cursorX, cursorY, houseRoot, GRID_COLS, GRID_ROWS);
    }
}