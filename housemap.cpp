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
    
    // Store original positions and characters
    int cx = x1, cy = y1;
    
    // Calculate path points for animation
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
    
    // Slower animation - each point takes longer to light up
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
            // Increased pause between steps (from 50ms to 200ms)
            usleep(200000);
        }
    }
}

void animateKruskal(HouseNode* root) {
    HousePos houses[GRID_ROWS * GRID_COLS];
    int n = 0;
    collectHouses(root, houses, &n);
    if (n < 2) {
        printf("Need at least 2 houses to connect!\n");
        _getch();
        return;
    }

    // Build the edges between all houses
    int edgeCount = 0;
    HouseEdge edges[n * (n - 1) / 2];
    for (int i = 0; i < n; ++i)
        for (int j = i + 1; j < n; ++j) {
            double dx = houses[i].x - houses[j].x;
            double dy = houses[i].y - houses[j].y;
            edges[edgeCount++] = (HouseEdge){i, j, sqrt(dx*dx + dy*dy)};
        }
        
    // Sort edges by weight (distance)
    for (int i = 0; i < edgeCount - 1; ++i)
        for (int j = i + 1; j < edgeCount; ++j)
            if (edges[i].weight > edges[j].weight) {
                HouseEdge tmp = edges[i]; edges[i] = edges[j]; edges[j] = tmp;
            }

    int parent[n];
    for (int i = 0; i < n; ++i) parent[i] = i;

    // Initialize grid with houses marked
    char grid[GRID_ROWS][GRID_COLS];
    memset(grid, '.', sizeof(grid));
    for (int i = 0; i < n; ++i)
        grid[houses[i].y][houses[i].x] = 'H';

    // Show initial grid with just houses
    system("cls");
    printf("Starting to connect houses (Kruskal MST)...\n\n");
    for (int y = 0; y < GRID_ROWS; ++y) {
        // ...existing code...
    }
    // Longer initial pause (from 500ms to 1000ms)
    usleep(1000000); 

    // Run Kruskal's algorithm with animated connections
    int added = 0;
    for (int i = 0; i < edgeCount && added < n - 1; ++i) {
        int u = edges[i].u, v = edges[i].v;
        if (findSet(parent, u) != findSet(parent, v)) {
            unionSet(parent, u, v);
            
            // Announce which houses are being connected
            printf("\nConnecting %s to %s...", 
                   houses[u].node->house.name, 
                   houses[v].node->house.name);
            // Longer pause before connection (from 300ms to 800ms)
            usleep(800000);
            
            // Animate the connection between houses
            drawLineWithAnimation(houses[u].x, houses[u].y, 
                                 houses[v].x, houses[v].y, grid);
            
            // Add a pause after each connection is complete
            usleep(500000);
            
            added++;
        }
    }
    
    printf("\nConnection complete! All houses are connected.\n");
    printf("Press any key to continue...\n");
    _getch();
}

void disconnectHouses(HouseNode* root) {
    HousePos houses[GRID_ROWS * GRID_COLS];
    int n = 0;
    collectHouses(root, houses, &n);
    
    if (n < 2) {
        printf("No connections to remove.\n");
        _getch();
        return;
    }

    // First show the connected grid
    char grid[GRID_ROWS][GRID_COLS];
    memset(grid, '.', sizeof(grid));
    
    // Place houses
    for (int i = 0; i < n; ++i)
        grid[houses[i].y][houses[i].x] = 'H';
    
    // Build minimal spanning tree
    int parent[n];
    for (int i = 0; i < n; ++i) parent[i] = i;
    
    int edgeCount = 0;
    HouseEdge edges[n * (n - 1) / 2];
    for (int i = 0; i < n; ++i)
        for (int j = i + 1; j < n; ++j) {
            double dx = houses[i].x - houses[j].x;
            double dy = houses[i].y - houses[j].y;
            edges[edgeCount++] = (HouseEdge){i, j, sqrt(dx*dx + dy*dy)};
        }
    
    // Sort edges by weight for Kruskal's algorithm
    for (int i = 0; i < edgeCount - 1; ++i)
        for (int j = i + 1; j < edgeCount; ++j)
            if (edges[i].weight > edges[j].weight) {
                HouseEdge tmp = edges[i]; edges[i] = edges[j]; edges[j] = tmp;
            }
    
    // Create connections and save all line paths for later traceback
    typedef struct {
        int x, y;
    } Point;
    
    typedef struct {
        Point* points;
        int numPoints;
        char* houseName1;
        char* houseName2;
    } LinePath;
    
    LinePath paths[n-1];  // MST has n-1 edges
    int pathCount = 0;
    
    // Create connections for display
    int added = 0;
    for (int i = 0; i < edgeCount && added < n - 1; ++i) {
        int u = edges[i].u, v = edges[i].v;
        if (findSet(parent, u) != findSet(parent, v)) {
            unionSet(parent, u, v);
            
            // Draw line on grid and save the path
            int x1 = houses[u].x, y1 = houses[u].y;
            int x2 = houses[v].x, y2 = houses[v].y;
            
            // Allocate temporary points array
            Point tempPoints[GRID_ROWS * GRID_COLS];
            int tempPointCount = 0;
            
            int dx = abs(x2 - x1), dy = abs(y2 - y1);
            int sx = (x1 < x2) ? 1 : -1;
            int sy = (y1 < y2) ? 1 : -1;
            int err = dx - dy;
            int cx = x1, cy = y1;
            
            while (!(cx == x2 && cy == y2)) {
                int e2 = 2 * err;
                if (e2 > -dy) { err -= dy; cx += sx; }
                if (e2 < dx) { err += dx; cy += sy; }
                
                if (cx >= 0 && cx < GRID_COLS && cy >= 0 && cy < GRID_ROWS && grid[cy][cx] != 'H') {
                    grid[cy][cx] = '*';
                    // Save this point in the path
                    tempPoints[tempPointCount].x = cx;
                    tempPoints[tempPointCount].y = cy;
                    tempPointCount++;
                }
            }
            
            // Allocate and save the final path
            paths[pathCount].points = (Point*)malloc(tempPointCount * sizeof(Point));
            paths[pathCount].numPoints = tempPointCount;
            paths[pathCount].houseName1 = houses[u].node->house.name;
            paths[pathCount].houseName2 = houses[v].node->house.name;
            
            for (int j = 0; j < tempPointCount; j++) {
                paths[pathCount].points[j] = tempPoints[j];
            }
            pathCount++;
            added++;
        }
    }
    
    // Show the initial connected network
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
    usleep(1000000);  // 1 second pause
    
    // Traceback disconnection - disconnect one line at a time
    for (int p = pathCount - 1; p >= 0; p--) {
        printf("\nDisconnecting line between %s and %s...\n", 
               paths[p].houseName1, paths[p].houseName2);
        usleep(800000);  // 0.8 second pause
        
        // Trace back the line point by point
        for (int i = paths[p].numPoints - 1; i >= 0; i--) {
            int x = paths[p].points[i].x;
            int y = paths[p].points[i].y;
            
            // Clear this point
            grid[y][x] = '.';
            
            // Show the grid with this point removed
            system("cls");
            printf("Tracing back connection from %s to %s... (%d%%)\n\n", 
                   paths[p].houseName1, paths[p].houseName2,
                   (int)(100 * (paths[p].numPoints - i) / (float)paths[p].numPoints));
            
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
            usleep(150000);  // 0.15 second pause between points
        }
        
        // Free the path points
        free(paths[p].points);
    }
    
    usleep(500000);  // 0.5 second final pause
    printf("\nDisconnection traceback complete!\n");
    printf("Press any key to continue...\n");
    _getch();
}

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
        // --- connect command ---
        else if (ch == 'c' || ch == 'C') {
            animateKruskal(cityChunk->houseRoot);
        }
        // --- disconnect command ---
        else if (ch == 'x' || ch == 'X') {
            disconnectHouses(cityChunk->houseRoot);
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