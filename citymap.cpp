#include "worldmap.h"

#include "citymap.h"
#include <float.h>
#include <string.h>
#include <cctype>
#include <conio.h> 

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
                // Check if this is the center of any chunk
                bool isChunkCenter = false;
                for (int i = 0; i < MAX_CHUNKS; i++) {
                    if (chunks[i].valid &&
                        x >= chunks[i].x - chunks[i].width/2 &&
                        x <  chunks[i].x + (chunks[i].width+1)/2 &&
                        y >= chunks[i].y - chunks[i].height/2 &&
                        y <  chunks[i].y + (chunks[i].height+1)/2) {
                        isChunkCenter = true;
                        break;
                    }
                }
                if (isChunkCenter) {
                    printf("\033[34m#\033[0m"); // Always blue for chunk area
                } else {
                    printf("\033[33m#\033[0m"); // Yellow for path
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
            if (i >= 0 && i < CITY_WIDTH && j >= 0 && j < CITY_HEIGHT)
                cityMap[j][i] = '#';
        }
    }
}

void generateCityChunks(int count) {
    memset(chunks, 0, sizeof(chunks));

    for (int i = 0; i < MAX_CHUNKS; ++i) {
        chunks[i].houseRoot = NULL;
        chunks[i].connectionCount = 0;  
        for (int j = 0; j < MAX_CONNECTIONS; j++) {
            chunks[i].connections[j].active = 0;
        }
    }

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
                chunks[q].connectionCount = 0;  

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
                chunks[i].connectionCount = 0; 
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
// Replace your drawCleanRoad with this version:
void drawCleanRoad(int x1, int y1, int x2, int y2) {
    int dx = abs(x2 - x1), sx = x1 < x2 ? 1 : -1;
    int dy = -abs(y2 - y1), sy = y1 < y2 ? 1 : -1;
    int err = dx + dy, e2;

    int x = x1, y = y1;
    while (1) {
        if (x >= 0 && x < CITY_WIDTH && y >= 0 && y < CITY_HEIGHT)
            cityMap[y][x] = '#';

        if (x == x2 && y == y2) break;

        e2 = 2 * err;
        int prev_x = x, prev_y = y;

        if (e2 >= dy) { err += dy; x += sx; }
        if (e2 <= dx) { err += dx; y += sy; }

        // If both x and y changed, it's a diagonal step: widen the path
        if (x != prev_x && y != prev_y) {
            if (x >= 0 && x < CITY_WIDTH && prev_y >= 0 && prev_y < CITY_HEIGHT)
                cityMap[prev_y][x] = '#';
            if (prev_x >= 0 && prev_x < CITY_WIDTH && y >= 0 && y < CITY_HEIGHT)
                cityMap[y][prev_x] = '#';
        }
    }
}
void primMST(int numChunks) {
    if (numChunks <= 0 || numChunks > MAX_CHUNKS) {
        printf("Invalid number of chunks: %d\n", numChunks);
        return;
    }
    
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




void enterHouseGridForCurrentCity() {
    for (int i = 0; i < MAX_CHUNKS; ++i) {
        if (chunks[i].valid) {
            int left = chunks[i].x - chunks[i].width/2;
            int right = chunks[i].x + chunks[i].width/2;
            int top = chunks[i].y - chunks[i].height/2;
            int bottom = chunks[i].y + chunks[i].height/2;
            if (cityPlayerX >= left && cityPlayerX < right &&
                cityPlayerY >= top && cityPlayerY < bottom) {
                if (chunks[i].houseRoot == NULL) {
                    chunks[i].houseRoot = NULL;
                }
                printf("\nEntering house placement for this city chunk!\n");
                housePlacementLoopForChunk(&chunks[i]);
                printCityMap();
                return;
            }
        }
    }
    printf("You are not on a city chunk!\n");
}




int initializeCity(int numChunks) {
    numChunks = 4 + (rand() % 3);
    
    srand(time(NULL));
    printf("\033[2J");
    
    clearCityMap();
    generateCityChunks(numChunks);
    
    for (int i = 0; i < MAX_CHUNKS; ++i) {
        if (chunks[i].houseRoot != NULL) {
            freeHouseTree(chunks[i].houseRoot);
        }
        chunks[i].houseRoot = NULL;
    }
    
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



extern int currentCountryId; 
extern Country countries[];  

void citySandbox() {
    char move;
    int running = 1;

    printf("\nWelcome to the city!\n");
    printCityMap();
    printf("Controls: Use W/A/S/D to move (no Enter needed), E to enter house placement, Q to quit city.\n");

    while (running) {
        if (_kbhit()) {
            move = _getch();

            if (move == 'q' || move == 'Q') {
                running = 0;
            } else if (move == 'w' || move == 'a' || move == 's' || move == 'd' ||
                       move == 'W' || move == 'A' || move == 'S' || move == 'D') {
                move = tolower(move);
                movePlayerInCity(move);

                printf("\033[H\033[J");
                printCityMap();
                printf("Controls: Use W/A/S/D to move (no Enter needed), E to enter house placement, Q to quit city.\n");
            } else if (move == 'e' || move == 'E') {
                enterHouseGridForCurrentCity();
                printf("\033[H\033[J");
                printCityMap();
                printf("Controls: Use W/A/S/D to move (no Enter needed), E to enter house placement, Q to quit city.\n");
            } else {
                printf("Invalid input. Use w/a/s/d to move, e to enter city, q to quit.\n");
                fflush(stdout);
            }
        }
        // Optional: Sleep to reduce CPU usage
        // Sleep(10); // Uncomment if you include <windows.h>
    }
    if (currentCountryId >= 0) {
        memcpy(countries[currentCountryId].savedCityMap, cityMap, sizeof(cityMap));
        for (int i = 0; i < MAX_CHUNKS; ++i) {
            freeHouseTree(countries[currentCountryId].savedChunks[i].houseRoot);
            countries[currentCountryId].savedChunks[i].x = chunks[i].x;
            countries[currentCountryId].savedChunks[i].y = chunks[i].y;
            countries[currentCountryId].savedChunks[i].width = chunks[i].width;
            countries[currentCountryId].savedChunks[i].height = chunks[i].height;
            countries[currentCountryId].savedChunks[i].valid = chunks[i].valid;
            countries[currentCountryId].savedChunks[i].houseRoot = copyChunkData(&chunks[i], &countries[currentCountryId].savedChunks[i]);
        }
        countries[currentCountryId].lastCityPlayerX = cityPlayerX;
        countries[currentCountryId].lastCityPlayerY = cityPlayerY;
    }
}