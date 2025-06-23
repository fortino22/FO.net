#include "worldmap.h"
#include <conio.h>
#include "citymap.h"

int currentCountryId = -1; 
int currentWorldId = 0;
int totalWorlds = 1; 

Country countries[MAX_COUNTRIES];
Edge edges[MAX_COUNTRIES * MAX_COUNTRIES];
int parent[MAX_COUNTRIES];
char map[HEIGHT][WIDTH];
int edgeCount = 0;
int playerX = 0, playerY = 0;
int showPlayer = 0;

void clearMap() {
    for (int y = 0; y < HEIGHT; y++)
        for (int x = 0; x < WIDTH; x++)
            map[y][x] = '.';
}

void printMap() {
    printf("\033[H");
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            if (showPlayer && x == playerX && y == playerY) {
                printf("\033[31mP\033[0m");
            } else if (map[y][x] == '#') {
                printf("\033[32m#\033[0m");
            } else if (map[y][x] == '.') {
                printf("\033[34m.\033[0m");
            } else if (map[y][x] == '+') {
                printf("\033[33m+\033[0m");
            } else if (map[y][x] == '=') {
                printf("\033[35m=\033[0m");
            } else {
                putchar(map[y][x]);
            }
        }
        putchar('\n');
    }
}

int isInside(int x, int y, int vertices[][2], int n) {
    int i, j, c = 0;
    for (i = 0, j = n-1; i < n; j = i++) {
        if (((vertices[i][1] > y) != (vertices[j][1] > y)) &&
            (x < (vertices[j][0] - vertices[i][0]) * (y - vertices[i][1]) /
           (vertices[j][1] - vertices[i][1]) + vertices[i][0]))
            c = !c;
    }
    return c;
}

void fillPolygon(int vertices[][2], int n, char ch) {
    int minX = WIDTH, maxX = 0, minY = HEIGHT, maxY = 0;

    for (int i = 0; i < n; i++) {
        if (vertices[i][0] < minX) minX = vertices[i][0];
        if (vertices[i][0] > maxX) maxX = vertices[i][0];
        if (vertices[i][1] < minY) minY = vertices[i][1];
        if (vertices[i][1] > maxY) maxY = vertices[i][1];
    }

    if (minX < 0) minX = 0;
    if (minY < 0) minY = 0;
    if (maxX >= WIDTH) maxX = WIDTH - 1;
    if (maxY >= HEIGHT) maxY = HEIGHT - 1;

    for (int y = minY; y <= maxY; y++) {
        for (int x = minX; x <= maxX; x++) {
            if (isInside(x, y, vertices, n)) {
                map[y][x] = ch;
            }
        }
    }
}

int getPlayerCountry(int numCountries) {
    for (int i = 0; i < numCountries; i++) {
        if (!countries[i].valid) continue;
        
        if (isInside(playerX, playerY, countries[i].vertices, countries[i].numVertices)) {
            return i;
        }
    }
    return -1;
}

int isOverlapping(int vertices[][2], int n) {
    int minX = WIDTH, maxX = 0, minY = HEIGHT, maxY = 0;

    for (int i = 0; i < n; i++) {
        if (vertices[i][0] < minX) minX = vertices[i][0];
        if (vertices[i][0] > maxX) maxX = vertices[i][0];
        if (vertices[i][1] < minY) minY = vertices[i][1];
        if (vertices[i][1] > maxY) maxY = vertices[i][1];
    }

    minX -= COUNTRY_MARGIN;
    minY -= COUNTRY_MARGIN;
    maxX += COUNTRY_MARGIN;
    maxY += COUNTRY_MARGIN;

    if (minX < 0) minX = 0;
    if (minY < 0) minY = 0;
    if (maxX >= WIDTH) maxX = WIDTH - 1;
    if (maxY >= HEIGHT) maxY = HEIGHT - 1;

    for (int y = minY; y <= maxY; y++) {
        for (int x = minX; x <= maxX; x++) {
            if (map[y][x] != '.') {
                for (int i = 0; i < n; i++) {
                    int dx = x - vertices[i][0];
                    int dy = y - vertices[i][1];
                    if (dx*dx + dy*dy <= COUNTRY_MARGIN*COUNTRY_MARGIN) {
                        return 1;
                    }
                }

                if (isInside(x, y, vertices, n)) {
                    return 1;
                }
            }
        }
    }
    return 0;
}

void distributeCountryCenters(int n, int centers[][2]) {
    if (n == 4) {
        centers[0][0] = WIDTH / 4;
        centers[0][1] = HEIGHT / 4;

        centers[1][0] = WIDTH * 3 / 4;
        centers[1][1] = HEIGHT / 4;

        centers[2][0] = WIDTH / 4;
        centers[2][1] = HEIGHT * 3 / 4;

        centers[3][0] = WIDTH * 3 / 4;
        centers[3][1] = HEIGHT * 3 / 4;

        for (int i = 0; i < n; i++) {
            centers[i][0] += (rand() % 11) - 5;
            centers[i][1] += (rand() % 11) - 5;

            if (centers[i][0] < 10) centers[i][0] = 10;
            if (centers[i][0] >= WIDTH-10) centers[i][0] = WIDTH-10;
            if (centers[i][1] < 5) centers[i][1] = 5;
            if (centers[i][1] >= HEIGHT-5) centers[i][1] = HEIGHT-5;
        }
        return;
    }

    int gridCols = ceil(sqrt(n));
    int gridRows = ceil((double)n / gridCols);

    int cellWidth = (WIDTH - 20) / gridCols;
    int cellHeight = (HEIGHT - 10) / gridRows;

    int idx = 0;
    for (int row = 0; row < gridRows && idx < n; row++) {
        for (int col = 0; col < gridCols && idx < n; col++) {
            int baseX = 10 + col * cellWidth + cellWidth / 2;
            int baseY = 5 + row * cellHeight + cellHeight / 2;

            centers[idx][0] = baseX + (rand() % (cellWidth/2)) - (cellWidth/4);
            centers[idx][1] = baseY + (rand() % (cellHeight/2)) - (cellHeight/4);

            idx++;
        }
    }
}

void generateSmoothPolygon(int vertices[][2], int *n, int centerX, int centerY) {
    *n = 6 + rand() % 7;

    int baseRadius = 5 + rand() % 4;

    double sizeVariation = 0.8 + (rand() % 40) / 100.0;
    baseRadius = (int)(baseRadius * sizeVariation);

    for (int i = 0; i < *n; i++) {
        double angle = (double)i / *n * 2 * M_PI;

        double variation = 0.85 + (rand() % 30) / 100.0;
        int r = baseRadius * variation;

        vertices[i][0] = centerX + r * cos(angle);
        vertices[i][1] = centerY + r * sin(angle);

        if (vertices[i][0] < 0) vertices[i][0] = 0;
        if (vertices[i][0] >= WIDTH) vertices[i][0] = WIDTH - 1;
        if (vertices[i][1] < 0) vertices[i][1] = 0;
        if (vertices[i][1] >= HEIGHT) vertices[i][1] = HEIGHT - 1;
    }
}

int generateCountries(int desiredCount) {
    for (int i = 0; i < MAX_COUNTRIES; i++) {
        countries[i].valid = 0;
    }

    int count = 0;
    int maxAttempts = 300;
    int totalMaxAttempts = 3000;
    int totalAttempts = 0;

    int centers[MAX_COUNTRIES][2];
    distributeCountryCenters(desiredCount, centers);

    while (count < desiredCount && totalAttempts < totalMaxAttempts) {
        int attempts = 0;
        int success = 0;

        while (!success && attempts < maxAttempts) {
            attempts++;
            totalAttempts++;

            int vertices[MAX_VERTICES][2];
            int numVertices;
            int centerX = centers[count][0];
            int centerY = centers[count][1];

            generateSmoothPolygon(vertices, &numVertices, centerX, centerY);

            if (isOverlapping(vertices, numVertices)) {
                if (attempts % 50 == 0) {
                    centerX += (rand() % 7) - 3;
                    centerY += (rand() % 7) - 3;

                    if (centerX < 10) centerX = 10;
                    if (centerX >= WIDTH-10) centerX = WIDTH-10;
                    if (centerY < 5) centerY = 5;
                    if (centerY >= HEIGHT-5) centerY = HEIGHT-5;

                    centers[count][0] = centerX;
                    centers[count][1] = centerY;
                }
                continue;
            }

            for (int i = 0; i < numVertices; i++) {
                countries[count].vertices[i][0] = vertices[i][0];
                countries[count].vertices[i][1] = vertices[i][1];
            }
            countries[count].numVertices = numVertices;
            countries[count].centerX = centerX;
            countries[count].centerY = centerY;
            countries[count].baseRadius = 5 + rand() % 4;
            countries[count].valid = 1;

            // Per-country city data initialization
            countries[count].cityGenerated = 0;
            countries[count].cityNumChunks = 0;
            countries[count].lastCityPlayerX = -1;
            countries[count].lastCityPlayerY = -1;
            for (int j = 0; j < MAX_CHUNKS; j++) {
                countries[count].savedChunks[j].houseRoot = NULL;
                countries[count].savedChunks[j].connectionCount = 0; 
                
                for (int k = 0; k < MAX_CONNECTIONS; k++) {
                    countries[count].savedChunks[j].connections[k].active = 0;
                }
            }
            fillPolygon(vertices, numVertices, '#');

            if (count % 2 == 0) {
                printMap();
                usleep(FRAME_DELAY * 2);
            }

            success = 1;
            count++;
        }

        if (!success) {
            printf("Warning: Could not place country %d\n", count);
            count++;
        }
    }

    int validCount = 0;
    for (int i = 0; i < desiredCount; i++) {
        if (countries[i].valid) validCount++;
    }

    return validCount;
}
void createEdges(int n) {
    edgeCount = 0;
    for (int i = 0; i < n; i++) {
        if (!countries[i].valid) continue;

        for (int j = i + 1; j < n; j++) {
            if (!countries[j].valid) continue;

            double dist = sqrt(
                pow(countries[i].centerX - countries[j].centerX, 2) +
                pow(countries[i].centerY - countries[j].centerY, 2)
            );
            edges[edgeCount++] = (Edge){i, j, dist};
        }
    }
}

int find(int x) {
    if (parent[x] != x)
        parent[x] = find(parent[x]);
    return parent[x];
}

int unionSet(int x, int y) {
    int rx = find(x), ry = find(y);
    if (rx == ry) return 0;
    parent[ry] = rx;
    return 1;
}

int compareEdges(const void *a, const void *b) {
    Edge *e1 = (Edge *)a;
    Edge *e2 = (Edge *)b;
    return (e1->weight > e2->weight) - (e1->weight < e2->weight);
}

void drawDoubleLine(int x1, int y1, int x2, int y2) {
    int dx = abs(x2 - x1), dy = abs(y2 - y1);
    int sx = x1 < x2 ? 1 : -1;
    int sy = y1 < y2 ? 1 : -1;
    int err = dx - dy;
    int err2;

    double length = sqrt(dx*dx + dy*dy);
    if (length < 0.0001) return;

    double nx = -sy / length;
    double ny = sx / length;

    int cx1 = x1, cy1 = y1;
    while (1) {
        if (cx1 >= 0 && cx1 < WIDTH && cy1 >= 0 && cy1 < HEIGHT) {
            if (map[cy1][cx1] == '.') map[cy1][cx1] = '+';
        }

        if (dx > 0 && dy > 0) {
            if (cx1 != x2 && cy1 != y2) {
                if (cx1+sx >= 0 && cx1+sx < WIDTH && cy1 >= 0 && cy1 < HEIGHT) {
                    if (map[cy1][cx1+sx] == '.') map[cy1][cx1+sx] = '+';
                }
                if (cx1 >= 0 && cx1 < WIDTH && cy1+sy >= 0 && cy1+sy < HEIGHT) {
                    if (map[cy1+sy][cx1] == '.') map[cy1+sy][cx1] = '+';
                }
            }
        }

        if (cx1 == x2 && cy1 == y2) break;

        err2 = 2 * err;
        if (err2 > -dy) { err -= dy; cx1 += sx; }
        if (err2 < dx) { err += dx; cy1 += sy; }
    }

    int cx2 = x1 + round(nx), cy2 = y1 + round(ny);
    err = dx - dy;

    while (1) {
        if (cx2 >= 0 && cx2 < WIDTH && cy2 >= 0 && cy2 < HEIGHT) {
            if (map[cy2][cx2] == '.') map[cy2][cx2] = '=';
        }

        if (dx > 0 && dy > 0) {
            if (cx2 != (x2 + round(nx)) && cy2 != (y2 + round(ny))) {
                if (cx2+sx >= 0 && cx2+sx < WIDTH && cy2 >= 0 && cy2 < HEIGHT) {
                    if (map[cy2][cx2+sx] == '.') map[cy2][cx2+sx] = '=';
                }
                if (cx2 >= 0 && cx2 < WIDTH && cy2+sy >= 0 && cy2+sy < HEIGHT) {
                    if (map[cy2+sy][cx2] == '.') map[cy2+sy][cx2] = '=';
                }
            }
        }

        if (cx2 == (x2 + round(nx)) && cy2 == (y2 + round(ny))) break;

        err2 = 2 * err;
        if (err2 > -dy) { err -= dy; cx2 += sx; }
        if (err2 < dx) { err += dx; cy2 += sy; }
    }

    printMap();
    usleep(FRAME_DELAY);
}

void kruskalAndDrawDoubleLine(int n) {
    int validCountries[MAX_COUNTRIES];
    int validCount = 0;

    for (int i = 0; i < n; i++) {
        if (countries[i].valid) {
            parent[i] = i;
            validCountries[validCount++] = i;
        }
    }

    if (validCount <= 1) return;

    qsort(edges, edgeCount, sizeof(Edge), compareEdges);

    int added = 0;
    for (int i = 0; i < edgeCount && added < validCount - 1; i++) {
        int u = edges[i].u, v = edges[i].v;

        if (!countries[u].valid || !countries[v].valid) continue;

        if (unionSet(u, v)) {
            drawDoubleLine(
                countries[u].centerX, countries[u].centerY,
                countries[v].centerX, countries[v].centerY
            );
            added++;
        }
    }
}

void initializePlayerPosition(int numCountries) {
    for (int i = 0; i < numCountries; i++) {
        if (countries[i].valid) {
            playerX = countries[i].centerX;
            playerY = countries[i].centerY;
            return;
        }
    }

    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            if (map[y][x] == '#') {
                playerX = x;
                playerY = y;
                return;
            }
        }
    }
}


void movePlayer(char direction, int numCountries) {
    int prevX = playerX, prevY = playerY;
    int newX = playerX, newY = playerY;

    if (direction == 'w') newY--;
    if (direction == 's') newY++;
    if (direction == 'a') newX--;
    if (direction == 'd') newX++;

    if (newX >= 0 && newX < WIDTH && newY >= 0 && newY < HEIGHT &&
        (map[newY][newX] == '#' || map[newY][newX] == '+' || map[newY][newX] == '=')) {
        playerX = newX;
        playerY = newY;

        int countryId = getPlayerCountry(numCountries);
        if (countryId >= 0) {
            int prevCountry = -1;
            if (prevX != newX || prevY != newY) {
                for (int i = 0; i < numCountries; i++) {
                    if (!countries[i].valid) continue;
                    if (isInside(prevX, prevY, countries[i].vertices, countries[i].numVertices)) {
                        prevCountry = i;
                        break;
                    }
                }
            }

            if (prevCountry >= 0 && prevCountry != countryId && countries[prevCountry].cityGenerated) {
                memcpy(countries[prevCountry].savedCityMap, cityMap, sizeof(cityMap));
                for (int i = 0; i < MAX_CHUNKS; ++i) {
                    freeHouseTree(countries[prevCountry].savedChunks[i].houseRoot);
                    countries[prevCountry].savedChunks[i].x = chunks[i].x;
                    countries[prevCountry].savedChunks[i].y = chunks[i].y;
                    countries[prevCountry].savedChunks[i].width = chunks[i].width;
                    countries[prevCountry].savedChunks[i].height = chunks[i].height;
                    countries[prevCountry].savedChunks[i].valid = chunks[i].valid;
                    countries[prevCountry].savedChunks[i].houseRoot = copyChunkData(&chunks[i], &countries[prevCountry].savedChunks[i]);

                }
                countries[prevCountry].lastCityPlayerX = cityPlayerX;
                countries[prevCountry].lastCityPlayerY = cityPlayerY;
            }

            
                if (prevCountry != countryId) {
                    printf("\nEntering %s country...\n", countries[countryId].name ? countries[countryId].name : "unnamed");
                    printf("Press any key to explore the city...");
                    _getch();

                    system("cls");
                    printf("\033[H\033[J");
                    currentCountryId = countryId;
                    
                    memcpy(cityMap, countries[countryId].savedCityMap, sizeof(cityMap));
                    for (int i = 0; i < MAX_CHUNKS; ++i) {
                        freeHouseTree(chunks[i].houseRoot);
                        chunks[i].x = countries[countryId].savedChunks[i].x;
                        chunks[i].y = countries[countryId].savedChunks[i].y;
                        chunks[i].width = countries[countryId].savedChunks[i].width;
                        chunks[i].height = countries[countryId].savedChunks[i].height;
                        chunks[i].valid = countries[countryId].savedChunks[i].valid;
                        chunks[i].connectionCount = countries[countryId].savedChunks[i].connectionCount;
                        
                        for (int j = 0; j < MAX_CONNECTIONS; j++) {
                            chunks[i].connections[j] = countries[countryId].savedChunks[i].connections[j];
                        }
                        
                        chunks[i].houseRoot = copyChunkData(&countries[countryId].savedChunks[i], &chunks[i]);
                    }
                    cityPlayerX = countries[countryId].lastCityPlayerX;
                    cityPlayerY = countries[countryId].lastCityPlayerY;
                    printCityMap();
                    
                    citySandbox();
                    system("cls");
                    printf("\033[H\033[J");
                    printf("Returning to world map...\n");
                    printMap();
                }
        }
    }
}

int initializeWorld(int numCountries) {
    srand(time(NULL));
    printf("\033[2J");

    totalWorlds = 1; 

    clearMap();
    int validCount = generateCountries(numCountries); 
    printf("Successfully generated %d countries in the world\n", validCount);

    createEdges(numCountries);
    kruskalAndDrawDoubleLine(numCountries);

    printf("\nPre-generating all cities...\n");
    for (int i = 0; i < numCountries; i++) {
        if (countries[i].valid) {
            printf("Generating city for country %d...\n", i+1);
            
            char tempCityMap[CITY_HEIGHT][CITY_WIDTH];
            memcpy(tempCityMap, cityMap, sizeof(cityMap));
            Chunk tempChunks[MAX_CHUNKS];
            for (int j = 0; j < MAX_CHUNKS; j++) {
                tempChunks[j] = chunks[j];
                chunks[j].houseRoot = NULL;
                chunks[j].connectionCount = 0;
                for (int k = 0; k < MAX_CONNECTIONS; k++) {
                    chunks[j].connections[k].active = 0;
                }
            }
            
            int tempCountryId = currentCountryId;
            currentCountryId = i;
            int numChunks = 10 + (countries[i].baseRadius / 2);
            int validCityChunks = initializeCity(numChunks);
            countries[i].cityGenerated = 1;
            countries[i].cityNumChunks = numChunks;
            memcpy(countries[i].savedCityMap, cityMap, sizeof(cityMap));
            
            for (int j = 0; j < MAX_CHUNKS; ++j) {
                countries[i].savedChunks[j].x = chunks[j].x;
                countries[i].savedChunks[j].y = chunks[j].y;
                countries[i].savedChunks[j].width = chunks[j].width;
                countries[i].savedChunks[j].height = chunks[j].height;
                countries[i].savedChunks[j].valid = chunks[j].valid;
                countries[i].savedChunks[j].houseRoot = NULL;
                countries[i].savedChunks[j].connectionCount = 0;
                for (int k = 0; k < MAX_CONNECTIONS; k++) {
                    countries[i].savedChunks[j].connections[k].active = 0;
                }
            }
            countries[i].lastCityPlayerX = cityPlayerX;
            countries[i].lastCityPlayerY = cityPlayerY;
            
            currentCountryId = tempCountryId;
            memcpy(cityMap, tempCityMap, sizeof(cityMap));
            for (int j = 0; j < MAX_CHUNKS; j++) {
                freeHouseTree(chunks[j].houseRoot);
                chunks[j] = tempChunks[j];
            }
        }
    }
    printf("All cities generated!\n");

    initializePlayerPosition(numCountries);

    showPlayer = 1;
    printMap();

    return validCount;
}

void startGameLoop(int numCountries) {
    char move;
    int running = 1;

    printf("\nExplore the world! Use WASD to move, enter countries to view cities\n");
    printf("Press Q to quit exploration\n\n");

    while (running) {
        if (_kbhit()) {
            move = _getch();
            if (move == 'q' || move == 'Q') {
                running = 0;
            } else {
                movePlayer(move, numCountries);
                printMap();
            }
        }
    }
}