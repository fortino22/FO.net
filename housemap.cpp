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
        return node; 

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
    
    if (root->house.gridX == gridX && root->house.gridY == gridY)
        return root;
    
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

typedef struct {
    int x, y;
    HouseNode* node;
} HousePos;

void collectHouses(HouseNode* root, HousePos* arr, int* idx) {
    if (!root) return;
    collectHouses(root->left, arr, idx);
    arr[*idx].x = root->house.gridX;
    arr[*idx].y = root->house.gridY;
    arr[*idx].node = root;
    (*idx)++;
    collectHouses(root->right, arr, idx);
}


typedef struct {
    int u, v;
    double weight;
} HouseEdge;

int findSet(int* parent, int x) {
    if (parent[x] != x) parent[x] = findSet(parent, parent[x]);
    return parent[x];
}

void unionSet(int* parent, int x, int y) {
    int rx = findSet(parent, x);
    int ry = findSet(parent, y);
    if (rx != ry) parent[ry] = rx;
}



#define COLOR_RESET   "\033[0m"
#define COLOR_CURSOR  "\033[1;32m"
#define COLOR_HOUSE   "\033[1;34m"
#define COLOR_EMPTY   "\033[0;37m"


void drawLineWithAnimation(int x1, int y1, int x2, int y2, char grid[GRID_ROWS][GRID_COLS]) {
    int dx = abs(x2 - x1), dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;
    
    int cx = x1, cy = y1;
    
    int pathX[GRID_ROWS*GRID_COLS], pathY[GRID_ROWS*GRID_COLS];
    int pathLen = 0;
    
    while (1) {
        if (cx >= 0 && cx < GRID_COLS && cy >= 0 && cy < GRID_ROWS && grid[cy][cx] != 'H') {
            pathX[pathLen] = cx;
            pathY[pathLen] = cy;
            pathLen++;
        }
        
        if (cx == x2 && cy == y2) break;
        
        int e2 = 2 * err;
        if (e2 > -dy) { err -= dy; cx += sx; }
        if (e2 < dx) { err += dx; cy += sy; }
    }
    
    for (int i = 0; i < pathLen; i++) {
        if (grid[pathY[i]][pathX[i]] != 'H') {
            grid[pathY[i]][pathX[i]] = '*';
            
            system("cls");
            printf("Connecting houses with power lines...\n\n");
            for (int y = 0; y < GRID_ROWS; ++y) {
                for (int x = 0; x < GRID_COLS; ++x) {
                    if (grid[y][x] == 'H')
                        printf(COLOR_HOUSE "H" COLOR_RESET " ");
                    else if (grid[y][x] == '*')
                        printf(COLOR_CURSOR "*" COLOR_RESET " ");
                    else
                        printf(". ");
                }
                printf("\n");
            }
            usleep(200000);
        }
    }
}

void animateKruskal(HouseNode* root) {
    Chunk* chunk = NULL;
    for (int i = 0; i < MAX_CHUNKS; i++) {
        if (chunks[i].houseRoot == root && chunks[i].valid) {
            chunk = &chunks[i];
            break;
        }
    }
    
    if (!chunk) {
        printf("Error: Could not find chunk for this house grid.\n");
        _getch();
        return;
    }

    HousePos houses[GRID_ROWS * GRID_COLS];
    int n = 0;
    collectHouses(root, houses, &n);
    if (n < 2) {
        printf("Need at least 2 houses to connect!\n");
        _getch();
        return;
    }

    int edgeCount = 0;
    HouseEdge edges[n * (n - 1) / 2];
    for (int i = 0; i < n; ++i)
        for (int j = i + 1; j < n; ++j) {
            double dx = houses[i].x - houses[j].x;
            double dy = houses[i].y - houses[j].y;
            edges[edgeCount++] = (HouseEdge){i, j, sqrt(dx*dx + dy*dy)};
        }
    
    for (int i = 0; i < edgeCount - 1; ++i)
        for (int j = i + 1; j < edgeCount; ++j)
            if (edges[i].weight > edges[j].weight) {
                HouseEdge tmp = edges[i]; edges[i] = edges[j]; edges[j] = tmp;
            }

    int parent[n];
    for (int i = 0; i < n; ++i) parent[i] = i;

    char grid[GRID_ROWS][GRID_COLS];
    memset(grid, '.', sizeof(grid));
    for (int i = 0; i < n; ++i)
        grid[houses[i].y][houses[i].x] = 'H';

    for (int i = 0; i < chunk->connectionCount; i++) {
        if (chunk->connections[i].chunkX == chunk->x && 
            chunk->connections[i].chunkY == chunk->y) {
            chunk->connections[i].active = 0;
        }
    }
    
    int activeCount = 0;
    for (int i = 0; i < chunk->connectionCount; i++) {
        if (chunk->connections[i].active) {
            if (activeCount != i) {
                chunk->connections[activeCount] = chunk->connections[i];
            }
            activeCount++;
        }
    }
    chunk->connectionCount = activeCount;

    system("cls");
    printf("Starting to connect houses (Kruskal MST)...\n\n");
    for (int y = 0; y < GRID_ROWS; ++y) {
        for (int x = 0; x < GRID_COLS; ++x) {
            if (grid[y][x] == 'H')
                printf(COLOR_HOUSE "H" COLOR_RESET " ");
            else
                printf(". ");
        }
        printf("\n");
    }
    usleep(1000000);
    int added = 0;
    for (int i = 0; i < edgeCount && added < n - 1; ++i) {
        int u = edges[i].u, v = edges[i].v;
        if (findSet(parent, u) != findSet(parent, v)) {
            unionSet(parent, u, v);
            
            printf("\nConnecting %s to %s...", 
                   houses[u].node->house.name, 
                   houses[v].node->house.name);
            usleep(800000);
            
            if (chunk->connectionCount < MAX_CONNECTIONS) {
                chunk->connections[chunk->connectionCount].startX = houses[u].x;
                chunk->connections[chunk->connectionCount].startY = houses[u].y;
                chunk->connections[chunk->connectionCount].endX = houses[v].x;
                chunk->connections[chunk->connectionCount].endY = houses[v].y;
                chunk->connections[chunk->connectionCount].active = 1;
                chunk->connections[chunk->connectionCount].chunkX = chunk->x; 
                chunk->connections[chunk->connectionCount].chunkY = chunk->y; 
                chunk->connectionCount++;
            }
            
            drawLineWithAnimation(houses[u].x, houses[u].y, 
                                 houses[v].x, houses[v].y, grid);
            
            usleep(500000);
            added++;
        }
    }
    
    printf("\nConnection complete! All houses are connected.\n");
    printf("The connections will remain visible until disconnected.\n");
    printf("Press any key to continue...\n");
    _getch();
}


void disconnectHouses(HouseNode* root) {
    Chunk* chunk = NULL;
    for (int i = 0; i < MAX_CHUNKS; i++) {
        if (chunks[i].houseRoot == root && chunks[i].valid) {
            chunk = &chunks[i];
            break;
        }
    }
    
    if (!chunk) {
        printf("Error: Could not find chunk for this house grid.\n");
        _getch();
        return;
    }

    HousePos houses[GRID_ROWS * GRID_COLS];
    int n = 0;
    collectHouses(root, houses, &n);
    
    int activeConnectionsInChunk = 0;
    for (int i = 0; i < chunk->connectionCount; i++) {
        if (chunk->connections[i].active && 
            chunk->connections[i].chunkX == chunk->x && 
            chunk->connections[i].chunkY == chunk->y) {
            activeConnectionsInChunk++;
        }
    }
    
    if (n < 2 || activeConnectionsInChunk == 0) {
        printf("No connections to remove in this city.\n");
        _getch();
        return;
    }

    char grid[GRID_ROWS][GRID_COLS];
    memset(grid, '.', sizeof(grid));
    
    for (int i = 0; i < n; ++i)
        grid[houses[i].y][houses[i].x] = 'H';
    
    for (int i = 0; i < chunk->connectionCount; i++) {
        if (chunk->connections[i].active && 
            chunk->connections[i].chunkX == chunk->x && 
            chunk->connections[i].chunkY == chunk->y) {
            
            int x1 = chunk->connections[i].startX;
            int y1 = chunk->connections[i].startY;
            int x2 = chunk->connections[i].endX;
            int y2 = chunk->connections[i].endY;
            
            int dx = abs(x2 - x1), dy = abs(y2 - y1);
            int sx = (x1 < x2) ? 1 : -1;
            int sy = (y1 < y2) ? 1 : -1;
            int err = dx - dy;
            int cx = x1, cy = y1;
            
            while (!(cx == x2 && cy == y2)) {
                int e2 = 2 * err;
                if (e2 > -dy) { err -= dy; cx += sx; }
                if (e2 < dx) { err += dx; cy += sy; }
                
                if (cx >= 0 && cx < GRID_COLS && cy >= 0 && cy < GRID_ROWS && grid[cy][cx] != 'H')
                    grid[cy][cx] = '*';
            }
        }
    }
    
    system("cls");
    printf("Power grid fully connected. Beginning traceback disconnection...\n\n");
    for (int y = 0; y < GRID_ROWS; ++y) {
        for (int x = 0; x < GRID_COLS; ++x) {
            if (grid[y][x] == 'H')
                printf(COLOR_HOUSE "H" COLOR_RESET " ");
            else if (grid[y][x] == '*')
                printf(COLOR_CURSOR "*" COLOR_RESET " ");
            else
                printf(". ");
        }
        printf("\n");
    }
    usleep(1000000);
    
    typedef struct {
        int x, y;
    } Point;
    
    for (int c = chunk->connectionCount - 1; c >= 0; c--) {
        if (!chunk->connections[c].active || 
            chunk->connections[c].chunkX != chunk->x || 
            chunk->connections[c].chunkY != chunk->y) continue;
        
        int x1 = chunk->connections[c].startX;
        int y1 = chunk->connections[c].startY;
        int x2 = chunk->connections[c].endX;
        int y2 = chunk->connections[c].endY;
        
        char *houseName1 = "Unknown";
        char *houseName2 = "Unknown";
        for (int i = 0; i < n; i++) {
            if (houses[i].x == x1 && houses[i].y == y1)
                houseName1 = houses[i].node->house.name;
            if (houses[i].x == x2 && houses[i].y == y2)
                houseName2 = houses[i].node->house.name;
        }
        
        printf("\nDisconnecting line between %s and %s...\n", houseName1, houseName2);
        usleep(800000);
        
        Point points[GRID_ROWS * GRID_COLS];
        int pointCount = 0;
        
        int dx = abs(x2 - x1), dy = abs(y2 - y1);
        int sx = (x1 < x2) ? 1 : -1;
        int sy = (y1 < y2) ? 1 : -1;
        int err = dx - dy;
        int cx = x1, cy = y1;
        
        while (!(cx == x2 && cy == y2)) {
            int e2 = 2 * err;
            if (e2 > -dy) { err -= dy; cx += sx; }
            if (e2 < dx) { err += dx; cy += sy; }
            
            if (cx >= 0 && cx < GRID_COLS && cy >= 0 && cy < GRID_ROWS && grid[cy][cx] == '*') {
                points[pointCount].x = cx;
                points[pointCount].y = cy;
                pointCount++;
            }
        }
        
        for (int i = pointCount - 1; i >= 0; i--) {
            int x = points[i].x;
            int y = points[i].y;
            
            grid[y][x] = '.';
            
            system("cls");
            printf("Tracing back connection from %s to %s... (%d%%)\n\n", 
                   houseName1, houseName2,
                   (int)(100 * (pointCount - i) / (float)pointCount));
            
            for (int y = 0; y < GRID_ROWS; ++y) {
                for (int x = 0; x < GRID_COLS; ++x) {
                    if (grid[y][x] == 'H')
                        printf(COLOR_HOUSE "H" COLOR_RESET " ");
                    else if (grid[y][x] == '*')
                        printf(COLOR_CURSOR "*" COLOR_RESET " ");
                    else
                        printf(". ");
                }
                printf("\n");
            }
            usleep(150000);
        }
        
        chunk->connections[c].active = 0;
    }
    
    int activeCount = 0;
    for (int i = 0; i < chunk->connectionCount; i++) {
        if (chunk->connections[i].active) {
            if (activeCount != i) {
                chunk->connections[activeCount] = chunk->connections[i];
            }
            activeCount++;
        }
    }
    chunk->connectionCount = activeCount;
    
    usleep(500000);
    printf("\nDisconnection traceback complete!\n");
    printf("Press any key to continue...\n");
    _getch();
}

void drawGrid(int cursorX, int cursorY, HouseNode* root, int gridW, int gridH) {
    system("cls"); 
    printf("Use WASD to move, ENTER to place/show, Q to quit, L to list all houses.\n");
    printf("C to connect houses, X to disconnect.\n\n");
    
    char displayGrid[GRID_ROWS][GRID_COLS];
    memset(displayGrid, '.', sizeof(displayGrid));
    
    HousePos houses[GRID_ROWS * GRID_COLS];
    int houseCount = 0;
    if (root) {
        collectHouses(root, houses, &houseCount);
        
        for (int i = 0; i < houseCount; i++) {
            if (houses[i].x >= 0 && houses[i].x < gridW && 
                houses[i].y >= 0 && houses[i].y < gridH) {
                displayGrid[houses[i].y][houses[i].x] = 'H';
            }
        }
    }
    
    Chunk* chunk = NULL;
    for (int i = 0; i < MAX_CHUNKS; i++) {
        if (chunks[i].houseRoot == root) {
            chunk = &chunks[i];
            break;
        }
    }
    
    if (chunk) {
        for (int i = 0; i < chunk->connectionCount; i++) {
            if (chunk->connections[i].active && 
                chunk->connections[i].chunkX == chunk->x && 
                chunk->connections[i].chunkY == chunk->y) {
                
                int x1 = chunk->connections[i].startX;
                int y1 = chunk->connections[i].startY;
                int x2 = chunk->connections[i].endX;
                int y2 = chunk->connections[i].endY;
                
                bool startHasHouse = false;
                bool endHasHouse = false;
                
                for (int j = 0; j < houseCount; j++) {
                    if (houses[j].x == x1 && houses[j].y == y1) {
                        startHasHouse = true;
                    }
                    if (houses[j].x == x2 && houses[j].y == y2) {
                        endHasHouse = true;
                    }
                }
                
                if (startHasHouse && endHasHouse) {
                    int dx = abs(x2 - x1), dy = abs(y2 - y1);
                    int sx = (x1 < x2) ? 1 : -1;
                    int sy = (y1 < y2) ? 1 : -1;
                    int err = dx - dy;
                    int cx = x1, cy = y1;
                    
                    while (!(cx == x2 && cy == y2)) {
                        int e2 = 2 * err;
                        if (e2 > -dy) { err -= dy; cx += sx; }
                        if (e2 < dx) { err += dx; cy += sy; }
                        
                        if (cx >= 0 && cx < gridW && cy >= 0 && cy < gridH && displayGrid[cy][cx] != 'H') {
                            displayGrid[cy][cx] = '*';
                        }
                    }
                }
            }
        }
    }
    
    for (int y = 0; y < gridH; ++y) {
        for (int x = 0; x < gridW; ++x) {
            if (x == cursorX && y == cursorY)
                printf(COLOR_CURSOR "[");
            else
                printf(" ");
                
            if (displayGrid[y][x] == 'H')
                printf(COLOR_HOUSE "H" COLOR_RESET);
            else if (displayGrid[y][x] == '*') 
                printf(COLOR_CURSOR "*" COLOR_RESET);
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


HouseNode* copyChunkData(Chunk* srcChunk, Chunk* destChunk) {
    int destX = destChunk->x;
    int destY = destChunk->y;
    
    destChunk->connectionCount = srcChunk->connectionCount;
    
    for (int i = 0; i < MAX_CONNECTIONS; i++) {
        if (i < srcChunk->connectionCount) {
            destChunk->connections[i] = srcChunk->connections[i];
            
            if (srcChunk->connections[i].chunkX == srcChunk->x && 
                srcChunk->connections[i].chunkY == srcChunk->y) {
                destChunk->connections[i].chunkX = destX;
                destChunk->connections[i].chunkY = destY;
            }
        } else {
            destChunk->connections[i].active = 0;
        }
    }
    
    destChunk->x = destX;
    destChunk->y = destY;
    
    return copyHouseTree(srcChunk->houseRoot);
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

    int gridW = GRID_COLS;
    int gridH = GRID_ROWS;

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
        else if (ch == 'c' || ch == 'C') {
            animateKruskal(cityChunk->houseRoot);
        }
        else if (ch == 'x' || ch == 'X') {
            disconnectHouses(cityChunk->houseRoot);
        }
        else if (ch == 13) { 
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
                while (1) {
                    char buf[32];
                    printf("Enter electric power (Watt): ");
                    fgets(buf, sizeof(buf), stdin);
                    buf[strcspn(buf, "\n")] = 0;
                    int valid = 1;
                    if (strlen(buf) == 0) valid = 0;
                    for (int i = 0; buf[i]; i++) {
                        if (buf[i] < '0' || buf[i] > '9') valid = 0;
                    }
                    if (valid && sscanf(buf, "%d", &electricPower) == 1) break;
                    printf("Invalid input. Please enter a positive integer.\n");
                }

                int internetSpeed = 0;
                while (1) {
                    char buf[32];
                    printf("Enter internet speed (Mbps): ");
                    fgets(buf, sizeof(buf), stdin);
                    buf[strcspn(buf, "\n")] = 0;
                    int valid = 1;
                    if (strlen(buf) == 0) valid = 0;
                    for (int i = 0; buf[i]; i++) {
                        if (buf[i] < '0' || buf[i] > '9') valid = 0;
                    }
                    if (valid && sscanf(buf, "%d", &internetSpeed) == 1) break;
                    printf("Invalid input. Please enter a positive integer.\n");
                }

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
