#include "citymap.h"
#include <float.h>
#include <string.h>

Chunk chunks[MAX_CHUNKS];
char cityMap[CITY_HEIGHT][CITY_WIDTH];
int cityPlayerX = 0, cityPlayerY = 0;
int showCityPlayer = 0;

void clearCityMap() {
    for (int y = 0; y < CITY_HEIGHT; y++) {
        for (int x = 0; x < CITY_WIDTH; x++) {
            cityMap[y][x] = ' ';
        }
    }
}

void printCityMap() {
    printf("\033[H");
    
    for (int y = 0; y < CITY_HEIGHT; y++) {
        for (int x = 0; x < CITY_WIDTH; x++) {
            if (showCityPlayer && x == cityPlayerX && y == cityPlayerY) {
                printf("\033[31mP\033[0m");
            } else {
                switch (cityMap[y][x]) {
                    case '#':
                        printf("\033[36m#\033[0m"); // Buildings in cyan
                        break;
                    case '+':
                        printf("\033[33m+\033[0m"); // Primary road in yellow like worldmap
                        break;
                    case '=':
                        printf("\033[35m=\033[0m"); // Secondary road in magenta like worldmap
                        break;
                    case ' ':
                        printf("\033[34m.\033[0m"); // Empty space as blue dots
                        break;
                    default:
                        printf("%c", cityMap[y][x]);
                }
            }
        }
        putchar('\n');
    }
}


bool isChunkOverlapping(int x, int y, int width, int height) {
    if (x < 0 || y < 0 || x + width >= CITY_WIDTH || y + height >= CITY_HEIGHT) {
        return true;
    }
    
    for (int j = y - CHUNK_MARGIN; j < y + height + CHUNK_MARGIN; j++) {
        for (int i = x - CHUNK_MARGIN; i < x + width + CHUNK_MARGIN; i++) {
            if (i < 0 || j < 0 || i >= CITY_WIDTH || j >= CITY_HEIGHT) {
                continue;
            }
            
            if (cityMap[j][i] != ' ') {
                return true;
            }
        }
    }
    
    return false;
}

void fillChunk(int x, int y, int width, int height) {
    for (int j = y; j < y + height; j++) {
        for (int i = x; i < x + width; i++) {
            cityMap[j][i] = '#';
        }
    }
}

void generateCityChunks(int count) {
    memset(chunks, 0, sizeof(chunks));
    
    int placementAttempts = 0;
    int maxAttempts = 500;
    
    for (int i = 0; i < count && placementAttempts < maxAttempts; i++) {
        int valid = 0;
        int attempts = 0;
        
        while (!valid && attempts < 50) {
            int width = 3 + rand() % 6;
            int height = 2 + rand() % 4;
            
            int x = rand() % (CITY_WIDTH - width - 2) + 1;
            int y = rand() % (CITY_HEIGHT - height - 2) + 1;
            
            if (!isChunkOverlapping(x, y, width, height)) {
                chunks[i].x = x + width/2;
                chunks[i].y = y + height/2;
                chunks[i].width = width;
                chunks[i].height = height;
                chunks[i].valid = 1;
                
                fillChunk(x, y, width, height);
                
                printCityMap();
                usleep(FRAME_DELAY);
                
                valid = 1;
            }
            
            attempts++;
            placementAttempts++;
        }
        
        if (!valid) {
            printf("Could not place chunk %d\n", i);
        }
    }
    
    int validCount = 0;
    for (int i = 0; i < count; i++) {
        if (chunks[i].valid) validCount++;
    }
    
    printf("Successfully placed %d out of %d city chunks\n", validCount, count);
}

void drawRoad(int x1, int y1, int x2, int y2) {
    int dx = abs(x2 - x1), dy = abs(y2 - y1);
    int sx = x1 < x2 ? 1 : -1;
    int sy = y1 < y2 ? 1 : -1;
    int err = dx - dy;
    int err2;
    
    double length = sqrt(dx*dx + dy*dy);
    if (length < 0.0001) return;
    
    int cx1 = x1, cy1 = y1;
    while (1) {
        if (cx1 >= 0 && cx1 < CITY_WIDTH && cy1 >= 0 && cy1 < CITY_HEIGHT) {
            if (cityMap[cy1][cx1] == ' ' || cityMap[cy1][cx1] == '+') {
                cityMap[cy1][cx1] = '+';
            }
        }
        
        if (dx > 2 && dy > 2) {
            if (cx1 != x2 && cy1 != y2) {
                if (cx1+sx >= 0 && cx1+sx < CITY_WIDTH && cy1 >= 0 && cy1 < CITY_HEIGHT) {
                    if (cityMap[cy1][cx1+sx] == ' ') cityMap[cy1][cx1+sx] = '+';
                }
                if (cx1 >= 0 && cx1 < CITY_WIDTH && cy1+sy >= 0 && cy1+sy < CITY_HEIGHT) {
                    if (cityMap[cy1+sy][cx1] == ' ') cityMap[cy1+sy][cx1] = '+';
                }
            }
        }
        
        if (cx1 == x2 && cy1 == y2) break;
        
        err2 = 2 * err;
        if (err2 > -dy) { err -= dy; cx1 += sx; }
        if (err2 < dx) { err += dx; cy1 += sy; }
    }
    
    if (length > 5) {
        double nx = -sy / length;
        double ny = sx / length;
        int offset = 1;
        
        int cx2 = x1 + round(nx * offset);
        int cy2 = y1 + round(ny * offset);
        err = dx - dy;
        
        while (1) {
            if (cx2 >= 0 && cx2 < CITY_WIDTH && cy2 >= 0 && cy2 < CITY_HEIGHT) {
                if (cityMap[cy2][cx2] == ' ' || cityMap[cy2][cx2] == '=') {
                    cityMap[cy2][cx2] = '=';
                }
            }
            
            if (dx > 2 && dy > 2) {
                int nx2 = cx2 + round(nx * offset);
                int ny2 = cy2 + round(ny * offset);
                
                if (nx2 >= 0 && nx2 < CITY_WIDTH && ny2 >= 0 && ny2 < CITY_HEIGHT) {
                    if (cityMap[ny2][nx2] == ' ') cityMap[ny2][nx2] = '=';
                }
            }
            
            int destX = x2 + round(nx * offset);
            int destY = y2 + round(ny * offset);
            
            if (cx2 == destX && cy2 == destY) break;
            
            err2 = 2 * err;
            if (err2 > -dy) { err -= dy; cx2 += sx; }
            if (err2 < dx) { err += dx; cy2 += sy; }
        }
    }
    
    if (rand() % 3 == 0) {
        printCityMap();
        usleep(FRAME_DELAY/3);
    }
}




void primMST(int numChunks) {
    int validCount = 0;
    int validIndices[MAX_CHUNKS];
    
    for (int i = 0; i < numChunks; i++) {
        if (chunks[i].valid) {
            validIndices[validCount++] = i;
        }
    }
    
    if (validCount <= 1) return;
    
    // Standard Prim's algorithm implementation
    bool inMST[MAX_CHUNKS];
    double key[MAX_CHUNKS];
    int parent[MAX_CHUNKS];
    
    for (int i = 0; i < validCount; i++) {
        key[i] = DBL_MAX;
        inMST[i] = false;
    }
    
    key[0] = 0;
    parent[0] = -1;
    
    for (int count = 0; count < validCount - 1; count++) {
        double min = DBL_MAX;
        int minIndex = 0;
        
        for (int v = 0; v < validCount; v++) {
            if (!inMST[v] && key[v] < min) {
                min = key[v];
                minIndex = v;
            }
        }
        
        inMST[minIndex] = true;
        
        for (int v = 0; v < validCount; v++) {
            int idx1 = validIndices[minIndex];
            int idx2 = validIndices[v];
            double dist = sqrt(
                pow(chunks[idx1].x - chunks[idx2].x, 2) +
                pow(chunks[idx1].y - chunks[idx2].y, 2)
            );
            
            if (!inMST[v] && dist < key[v]) {
                parent[v] = minIndex;
                key[v] = dist;
            }
        }
    }
    
    // Draw main roads first (MST backbone)
    for (int i = 1; i < validCount; i++) {
        int idx1 = validIndices[parent[i]];
        int idx2 = validIndices[i];
        
        drawRoad(
            chunks[idx1].x, chunks[idx1].y,
            chunks[idx2].x, chunks[idx2].y
        );
        
        printCityMap();
        usleep(FRAME_DELAY);
    }
    
    // Add additional roads for better connectivity (similar to worldmap's additional connections)
    int extraRoads = validCount / 3;
    for (int i = 0; i < extraRoads; i++) {
        int src = validIndices[rand() % validCount];
        int dest = validIndices[rand() % validCount];
        
        // Don't connect to itself and only make connections between chunks that aren't too close
        if (src != dest) {
            double dist = sqrt(
                pow(chunks[src].x - chunks[dest].x, 2) +
                pow(chunks[src].y - chunks[dest].y, 2)
            );
            
            // Only add extra roads if distance is meaningful
            if (dist > 5) {
                drawRoad(chunks[src].x, chunks[src].y, chunks[dest].x, chunks[dest].y);
            }
        }
    }
    
    // Add cross-city roads but fewer than before
    addBorderRoads();
}

void addBorderRoads() {
    // Create fewer, more strategic cross-city roads
    int hroads = 2 + rand() % 2; // 2-3 horizontal roads
    int vroads = 2 + rand() % 2; // 2-3 vertical roads
    
    // Add horizontal roads
    for (int i = 0; i < hroads; i++) {
        int y = (CITY_HEIGHT / (hroads+1)) * (i+1);
        int lastRoadX = -1;
        
        for (int x = 0; x < CITY_WIDTH; x++) {
            if (cityMap[y][x] == '+' || cityMap[y][x] == '#' || cityMap[y][x] == '=') {
                if (lastRoadX != -1 && x - lastRoadX > 3) {
                    drawRoad(lastRoadX, y, x, y);
                }
                lastRoadX = x;
            }
        }
    }
    
    // Add vertical roads
    for (int i = 0; i < vroads; i++) {
        int x = (CITY_WIDTH / (vroads+1)) * (i+1);
        int lastRoadY = -1;
        
        for (int y = 0; y < CITY_HEIGHT; y++) {
            if (cityMap[y][x] == '+' || cityMap[y][x] == '#' || cityMap[y][x] == '=') {
                if (lastRoadY != -1 && y - lastRoadY > 3) {
                    drawRoad(x, lastRoadY, x, y);
                }
                lastRoadY = y;
            }
        }
    }
}
void initializePlayerPosition() {
    for (int y = 0; y < CITY_HEIGHT; y++) {
        for (int x = 0; x < CITY_WIDTH; x++) {
            if (cityMap[y][x] == '+') {
                cityPlayerX = x;
                cityPlayerY = y;
                return;
            }
        }
    }
    
    cityPlayerX = CITY_WIDTH / 2;
    cityPlayerY = CITY_HEIGHT / 2;
}
void movePlayerInCity(char direction) {
    int newX = cityPlayerX;
    int newY = cityPlayerY;
    
    switch (direction) {
        case 'w': newY--; break;
        case 's': newY++; break;
        case 'a': newX--; break;
        case 'd': newX++; break;
    }
    
    // Check if new position is within bounds
    if (newX < 0 || newX >= CITY_WIDTH || newY < 0 || newY >= CITY_HEIGHT) {
        return;
    }
    
    // Allow movement on all road types and buildings
    if (cityMap[newY][newX] == '+' || cityMap[newY][newX] == '=' || cityMap[newY][newX] == '#') {
        cityPlayerX = newX;
        cityPlayerY = newY;
        return;
    }
    
    // If coming from a road, allow creating new road segments
    if (cityMap[cityPlayerY][cityPlayerX] == '+' || cityMap[cityPlayerY][cityPlayerX] == '=') {
        cityMap[newY][newX] = '+';  // Create new road segment
        cityPlayerX = newX;
        cityPlayerY = newY;
        return;
    }
    
    // Check for nearby road segments to connect to
    for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
            if (dx == 0 && dy == 0) continue;
            
            int checkX = newX + dx;
            int checkY = newY + dy;
            
            if (checkX >= 0 && checkX < CITY_WIDTH && checkY >= 0 && checkY < CITY_HEIGHT) {
                if (cityMap[checkY][checkX] == '+' || cityMap[checkY][checkX] == '=') {
                    cityMap[newY][newX] = '+';
                    cityPlayerX = newX;
                    cityPlayerY = newY;
                    return;
                }
            }
        }
    }
}

int initializeCity(int numChunks) {
    srand(time(NULL));
    printf("\033[2J");
    
    clearCityMap();
    generateCityChunks(numChunks);
    
    printf("Building city roads using Prim's algorithm...\n");
    primMST(numChunks);
    
    initializePlayerPosition();
    showCityPlayer = 1;
    printCityMap();
    
    int validCount = 0;
    for (int i = 0; i < numChunks; i++) {
        if (chunks[i].valid) validCount++;
    }
    
    return validCount;
}

void citySandbox() {
    char move;
    int running = 1;
    
    printf("\nWelcome to the city! Use WASD to move, 'q' to quit\n");
    
    while (running) {
        printf("Move direction (w/a/s/d, q to quit): ");
        scanf(" %c", &move);
        
        if (move == 'q') {
            running = 0;
        } else if (move == 'w' || move == 'a' || move == 's' || move == 'd') {
            movePlayerInCity(move);
            printCityMap();

            if (cityMap[cityPlayerY][cityPlayerX] == '#') {
                printf("You are at a building.\n");
            } else if (cityMap[cityPlayerY][cityPlayerX] == '+') {
                printf("You are on a road.\n");
            }
        }
    }
}