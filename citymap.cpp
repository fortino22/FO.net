#include "citymap.h"
#include <float.h>
#include <string.h>
#include <cctype>
#define PATH_CHAR '#'
#define BUILDING_CHAR '#'

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
            } else if (cityMap[y][x] == '#') {
                bool isBuilding = false;
                
                int adjacentCount = 0;
                if (y > 0 && cityMap[y-1][x] == '#') adjacentCount++;
                if (y < CITY_HEIGHT-1 && cityMap[y+1][x] == '#') adjacentCount++;
                if (x > 0 && cityMap[y][x-1] == '#') adjacentCount++;
                if (x < CITY_WIDTH-1 && cityMap[y][x+1] == '#') adjacentCount++;
                
                if (adjacentCount >= 3) {
                    printf("\033[36m#\033[0m");
                } else {
                    printf("\033[33m#\033[0m");
                }
            } else if (cityMap[y][x] == ' ') {
                printf("\033[34m.\033[0m"); 
            } else {
                printf("%c", cityMap[y][x]);
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

    if (count < 4) count = 4;
    if (count > MAX_CHUNKS) count = MAX_CHUNKS;

    const int FIXED_WIDTH = 4;
    const int FIXED_HEIGHT = 7;

    int placementAttempts = 0;
    int maxAttempts = 500;

    int xPad = CITY_WIDTH / 8;
    int yPad = CITY_HEIGHT / 8;
    int quadrants[4][4] = {
        {xPad, CITY_WIDTH/2 - xPad, yPad, CITY_HEIGHT/2 - yPad},
        {CITY_WIDTH/2 + xPad, CITY_WIDTH - xPad, yPad, CITY_HEIGHT/2 - yPad},
        {xPad, CITY_WIDTH/2 + xPad, CITY_HEIGHT/2 + yPad, CITY_HEIGHT - yPad},
        {CITY_WIDTH/2 + xPad, CITY_WIDTH - xPad, CITY_HEIGHT/2 + yPad, CITY_HEIGHT - yPad}
    };

    for (int q = 0; q < 4 && q < count; q++) {
        int valid = 0;
        int attempts = 0;

        while (!valid && attempts < 50) {
            int width = FIXED_WIDTH;
            int height = FIXED_HEIGHT;

            int minX = quadrants[q][0];
            int maxX = quadrants[q][1] - width;
            int minY = quadrants[q][2];
            int maxY = quadrants[q][3] - height;

            if (maxX <= minX || maxY <= minY) {
                attempts++;
                continue;
            }

            int x = minX + rand() % (maxX - minX + 1);
            int y = minY + rand() % (maxY - minY + 1);

            bool overlapping = false;
            int margin = 3; 

            for (int j = y - margin; j < y + height + margin; j++) {
                for (int i = x - margin; i < x + width + margin; i++) {
                    if (i < 0 || j < 0 || i >= CITY_WIDTH || j >= CITY_HEIGHT) {
                        continue;
                    }
                    if (cityMap[j][i] != ' ') {
                        overlapping = true;
                        break;
                    }
                }
                if (overlapping) break;
            }

            if (!overlapping) {
                chunks[q].x = x + width/2;
                chunks[q].y = y + height/2;
                chunks[q].width = width;
                chunks[q].height = height;
                chunks[q].valid = 1;

                fillChunk(x, y, width, height);

                printCityMap();
                usleep(FRAME_DELAY);

                valid = 1;
            }

            attempts++;
            placementAttempts++;
        }

        if (!valid) {
            printf("Could not place chunk in quadrant %d\n", q+1);
        }
    }

    for (int i = 4; i < count && placementAttempts < maxAttempts; i++) {
        int valid = 0;
        int attempts = 0;

        while (!valid && attempts < 50) {
            int width = FIXED_WIDTH;
            int height = FIXED_HEIGHT;

            int safeMargin = 4;
            int x = safeMargin + rand() % (CITY_WIDTH - 2*safeMargin - width);
            int y = safeMargin + rand() % (CITY_HEIGHT - 2*safeMargin - height);

            bool overlapping = false;
            int margin = 3; 

            for (int j = y - margin; j < y + height + margin; j++) {
                for (int k = x - margin; k < x + width + margin; k++) {
                    if (k < 0 || j < 0 || k >= CITY_WIDTH || j >= CITY_HEIGHT) {
                        continue;
                    }
                    if (cityMap[j][k] != ' ') {
                        overlapping = true;
                        break;
                    }
                }
                if (overlapping) break;
            }

            if (!overlapping) {
                chunks[i].x = x + width/2;
                chunks[i].y = y + height/2;
                chunks[i].width = width;
                chunks[i].height = height;
                chunks[i].valid = 1;

                fillChunk(x, y, width, height);

                if (i % 2 == 0) {
                    printCityMap();
                    usleep(FRAME_DELAY);
                }

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

    printCityMap();
    usleep(FRAME_DELAY);
}
void drawCleanRoad(int x1, int y1, int x2, int y2) {
    if (x1 < 0 || x1 >= CITY_WIDTH || y1 < 0 || y1 >= CITY_HEIGHT ||
        x2 < 0 || x2 >= CITY_WIDTH || y2 < 0 || y2 >= CITY_HEIGHT) {
        return; 
    }
    
    int safeMargin = 5;
    x1 = (x1 < safeMargin) ? safeMargin : ((x1 >= CITY_WIDTH-safeMargin) ? CITY_WIDTH-safeMargin-1 : x1);
    y1 = (y1 < safeMargin) ? safeMargin : ((y1 >= CITY_HEIGHT-safeMargin) ? CITY_HEIGHT-safeMargin-1 : y1);
    x2 = (x2 < safeMargin) ? safeMargin : ((x2 >= CITY_WIDTH-safeMargin) ? CITY_WIDTH-safeMargin-1 : x2);
    y2 = (y2 < safeMargin) ? safeMargin : ((y2 >= CITY_HEIGHT-safeMargin) ? CITY_HEIGHT-safeMargin-1 : y2);
    
    if (x1 == x2 && y1 == y2) {
        return; 
    }
    
    int startX = x1;
    int endX = x2;
    if (startX > endX) {
        startX = x2;
        endX = x1;
    }
    
    for (int x = startX; x <= endX; x++) {
        if (x >= 0 && x < CITY_WIDTH && y1 >= 0 && y1 < CITY_HEIGHT) {
            cityMap[y1][x] = '#';
        }
    }
    
    int startY = y1;
    int endY = y2;
    if (startY > endY) {
        startY = y2;
        endY = y1;
    }
    
    for (int y = startY; y <= endY; y++) {
        if (x2 >= 0 && x2 < CITY_WIDTH && y >= 0 && y < CITY_HEIGHT) {
            cityMap[y][x2] = '#';
        }
    }
    
    if (rand() % 3 == 0) {
        printCityMap();
        usleep(FRAME_DELAY/4);
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
            if (v == minIndex) continue; 
            
            int idx1 = validIndices[minIndex];
            int idx2 = validIndices[v];
            
            if (!chunks[idx1].valid || !chunks[idx2].valid) continue;
            
            double dist = sqrt(
                pow(chunks[idx1].x - chunks[idx2].x, 2) +
                pow(chunks[idx1].y - chunks[idx2].y, 2)
            );
            
            if (dist < 0.0001) dist = 0.0001; 
            
            if (!inMST[v] && dist < key[v]) {
                parent[v] = minIndex;
                key[v] = dist;
            }
        }
    }
    
    for (int i = 1; i < validCount; i++) {
        int idx1 = validIndices[parent[i]];
        int idx2 = validIndices[i];
        
        if (!chunks[idx1].valid || !chunks[idx2].valid) continue;
        
        drawCleanRoad(
            chunks[idx1].x, chunks[idx1].y,
            chunks[idx2].x, chunks[idx2].y
        );
    }
    
    int extraRoads = 1 + (validCount > 4 ? 1 : 0);
    
    typedef struct {
        int src, dest;
        double dist;
    } Connection;
    
    Connection connections[MAX_CHUNKS * MAX_CHUNKS];
    int connCount = 0;
    
    for (int i = 0; i < validCount; i++) {
        for (int j = i+1; j < validCount; j++) {
            bool connected = false;
            for (int k = 1; k < validCount; k++) {
                if ((parent[k] == i && k == j) || (parent[k] == j && k == i)) {
                    connected = true;
                    break;
                }
            }
            
            if (!connected) {
                int idx1 = validIndices[i];
                int idx2 = validIndices[j];
                
                if (!chunks[idx1].valid || !chunks[idx2].valid) continue;
                
                double dist = sqrt(
                    pow(chunks[idx1].x - chunks[idx2].x, 2) +
                    pow(chunks[idx1].y - chunks[idx2].y, 2)
                );
                
                if (dist < 0.0001) continue; 
                
                connections[connCount].src = idx1;
                connections[connCount].dest = idx2;
                connections[connCount].dist = dist;
                connCount++;
            }
        }
    }
    
    if (connCount == 0) return;
    
    for (int i = 0; i < connCount; i++) {
        for (int j = i+1; j < connCount; j++) {
            if (connections[j].dist > connections[i].dist) {
                Connection temp = connections[i];
                connections[i] = connections[j];
                connections[j] = temp;
            }
        }
    }
    
    for (int i = 0; i < extraRoads && i < connCount; i++) {
        drawCleanRoad(
            chunks[connections[i].src].x, chunks[connections[i].src].y,
            chunks[connections[i].dest].x, chunks[connections[i].dest].y
        );
    }
}


void addSimplifiedCrossRoads() {
    int minX = CITY_WIDTH, maxX = 0;
    int minY = CITY_HEIGHT, maxY = 0;
    
    for (int i = 0; i < MAX_CHUNKS; i++) {
        if (chunks[i].valid) {
            if (chunks[i].x - chunks[i].width/2 < minX) minX = chunks[i].x - chunks[i].width/2;
            if (chunks[i].x + chunks[i].width/2 > maxX) maxX = chunks[i].x + chunks[i].width/2;
            if (chunks[i].y - chunks[i].height/2 < minY) minY = chunks[i].y - chunks[i].height/2;
            if (chunks[i].y + chunks[i].height/2 > maxY) maxY = chunks[i].y + chunks[i].height/2;
        }
    }
    
    minX = minX < 3 ? 3 : minX;
    maxX = maxX >= CITY_WIDTH-3 ? CITY_WIDTH-4 : maxX;
    minY = minY < 3 ? 3 : minY;
    maxY = maxY >= CITY_HEIGHT-3 ? CITY_HEIGHT-4 : maxY;
    
    int midY = (minY + maxY) / 2;
    drawCleanRoad(minX, midY, maxX, midY);
    
    int midX = (minX + maxX) / 2;
    drawCleanRoad(midX, minY, midX, maxY);
}

void initializePlayerPosition() {
    for (int y = 5; y < CITY_HEIGHT - 5; y++) {
        for (int x = 5; x < CITY_WIDTH - 5; x++) {
            if (cityMap[y][x] == '#') {
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
    
    if (newX < 0 || newX >= CITY_WIDTH || newY < 0 || newY >= CITY_HEIGHT) {
        return;
    }
    
    if (cityMap[newY][newX] == '#') {
        cityPlayerX = newX;
        cityPlayerY = newY;
        return;
    }
}


int initializeCity(int numChunks) {
    numChunks = 4 + (rand() % 3);
    
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
        fflush(stdout);

        move = getchar();

        int c;
        while ((c = getchar()) != '\n' && c != EOF);

        if (move == 'q' || move == 'Q') {
            running = 0;
        } else if (move == 'w' || move == 'a' || move == 's' || move == 'd' ||
                   move == 'W' || move == 'A' || move == 'S' || move == 'D') {
            move = tolower(move);
            movePlayerInCity(move);

            printf("\033[H\033[J");
            printCityMap();
        } else {
            printf("Invalid input. Use w/a/s/d to move, q to quit.\n");
            fflush(stdout);
        }
    }
}
