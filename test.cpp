#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <unistd.h>

#define WIDTH   100
#define HEIGHT 30
#define MAX_COUNTRIES 5
#define FRAME_DELAY 30000
#define MAX_VERTICES 15
#define COUNTRY_MARGIN 5  // Margin between countries

typedef struct {
    int vertices[MAX_VERTICES][2];  // [x,y] pairs
    int numVertices;
    int centerX, centerY;
    int baseRadius;  // Store the base radius for better proportions
    int valid;  // Flag to indicate if country was successfully placed
} Country;

typedef struct {
    int u, v;
    double weight;
} Edge;

Country countries[MAX_COUNTRIES];
Edge edges[MAX_COUNTRIES * MAX_COUNTRIES];
int parent[MAX_COUNTRIES];
char map[HEIGHT][WIDTH];
int edgeCount = 0;
int playerX = 0, playerY = 0;
int showPlayer = 0;

// Clear map
void clearMap() {
    for (int y = 0; y < HEIGHT; y++)
        for (int x = 0; x < WIDTH; x++)
            map[y][x] = '.';
}

// Display map with colors
void printMap() {
    printf("\033[H"); // Move cursor to top
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            if (showPlayer && x == playerX && y == playerY) {
                printf("\033[31mP\033[0m"); // Red for player
            } else if (map[y][x] == '#') {
                printf("\033[32m#\033[0m"); // Green for countries
            } else if (map[y][x] == '.') {
                printf("\033[34m.\033[0m"); // Blue for ocean
            } else if (map[y][x] == '+') {
                printf("\033[33m+\033[0m"); // Yellow for paths
            } else if (map[y][x] == '=') {
                printf("\033[35m=\033[0m"); // Magenta for second path line
            } else {
                putchar(map[y][x]);
            }
        }
        putchar('\n');
    }
}

// Check if a point is inside a polygon
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

// Fill a polygon with a character
void fillPolygon(int vertices[][2], int n, char ch) {
    int minX = WIDTH, maxX = 0, minY = HEIGHT, maxY = 0;
    
    // Find bounding box
    for (int i = 0; i < n; i++) {
        if (vertices[i][0] < minX) minX = vertices[i][0];
        if (vertices[i][0] > maxX) maxX = vertices[i][0];
        if (vertices[i][1] < minY) minY = vertices[i][1];
        if (vertices[i][1] > maxY) maxY = vertices[i][1];
    }
    
    // Clip bounds
    if (minX < 0) minX = 0;
    if (minY < 0) minY = 0;
    if (maxX >= WIDTH) maxX = WIDTH - 1;
    if (maxY >= HEIGHT) maxY = HEIGHT - 1;
    
    // Fill polygon
    for (int y = minY; y <= maxY; y++) {
        for (int x = minX; x <= maxX; x++) {
            if (isInside(x, y, vertices, n)) {
                map[y][x] = ch;
            }
        }
    }
}

// Check if polygon overlaps with existing countries, including a margin
int isOverlapping(int vertices[][2], int n) {
    int minX = WIDTH, maxX = 0, minY = HEIGHT, maxY = 0;
    
    // Find bounding box
    for (int i = 0; i < n; i++) {
        if (vertices[i][0] < minX) minX = vertices[i][0];
        if (vertices[i][0] > maxX) maxX = vertices[i][0];
        if (vertices[i][1] < minY) minY = vertices[i][1];
        if (vertices[i][1] > maxY) maxY = vertices[i][1];
    }
    
    // Add margin for checking overlap
    minX -= COUNTRY_MARGIN; 
    minY -= COUNTRY_MARGIN;
    maxX += COUNTRY_MARGIN; 
    maxY += COUNTRY_MARGIN;
    
    // Clip bounds
    if (minX < 0) minX = 0;
    if (minY < 0) minY = 0;
    if (maxX >= WIDTH) maxX = WIDTH - 1;
    if (maxY >= HEIGHT) maxY = HEIGHT - 1;
    
    // Check if any point from the extended area is already occupied
    for (int y = minY; y <= maxY; y++) {
        for (int x = minX; x <= maxX; x++) {
            if (map[y][x] != '.') {
                // Check if this point is within margin distance of any vertex
                for (int i = 0; i < n; i++) {
                    int dx = x - vertices[i][0];
                    int dy = y - vertices[i][1];
                    if (dx*dx + dy*dy <= COUNTRY_MARGIN*COUNTRY_MARGIN) {
                        return 1; // Too close to existing country
                    }
                }
                
                // Check if inside the polygon
                if (isInside(x, y, vertices, n)) {
                    return 1; // Overlaps with existing country
                }
            }
        }
    }
    return 0;
}

// Modified to distribute evenly for 4 countries
void distributeCountryCenters(int n, int centers[][2]) {
    // For simplicity when n=4, place them in four quadrants
    if (n == 4) {
        centers[0][0] = WIDTH / 4;
        centers[0][1] = HEIGHT / 4;
        
        centers[1][0] = WIDTH * 3 / 4;
        centers[1][1] = HEIGHT / 4;
        
        centers[2][0] = WIDTH / 4;
        centers[2][1] = HEIGHT * 3 / 4;
        
        centers[3][0] = WIDTH * 3 / 4;
        centers[3][1] = HEIGHT * 3 / 4;
        
        // Add some randomness
        for (int i = 0; i < n; i++) {
            centers[i][0] += (rand() % 11) - 5;
            centers[i][1] += (rand() % 11) - 5;
            
            // Keep in bounds
            if (centers[i][0] < 10) centers[i][0] = 10;
            if (centers[i][0] >= WIDTH-10) centers[i][0] = WIDTH-10;
            if (centers[i][1] < 5) centers[i][1] = 5;
            if (centers[i][1] >= HEIGHT-5) centers[i][1] = HEIGHT-5;
        }
        return;
    }
    
    // Original logic for other numbers of countries
    int gridCols = ceil(sqrt(n));
    int gridRows = ceil((double)n / gridCols);
    
    int cellWidth = (WIDTH - 20) / gridCols;
    int cellHeight = (HEIGHT - 10) / gridRows;
    
    int idx = 0;
    for (int row = 0; row < gridRows && idx < n; row++) {
        for (int col = 0; col < gridCols && idx < n; col++) {
            // Calculate base position at center of the grid cell
            int baseX = 10 + col * cellWidth + cellWidth / 2;
            int baseY = 5 + row * cellHeight + cellHeight / 2;
            
            // Add some randomness within the cell for natural look
            centers[idx][0] = baseX + (rand() % (cellWidth/2)) - (cellWidth/4);
            centers[idx][1] = baseY + (rand() % (cellHeight/2)) - (cellHeight/4);
            
            idx++;
        }
    }
}

// Generate a more proportional and smooth polygon
void generateSmoothPolygon(int vertices[][2], int *n, int centerX, int centerY) {
    // Number of vertices (6-12 for smoother shapes)
    *n = 6 + rand() % 7;
    
    // Base radius proportional to map size
    int baseRadius = 5 + rand() % 4;  // Increased base radius
    
    // Scale radius differently based on position to create variable sized countries
    double sizeVariation = 0.8 + (rand() % 40) / 100.0;  // 0.8 to 1.2
    baseRadius = (int)(baseRadius * sizeVariation);
    
    // Generate vertices with smoother variation
    for (int i = 0; i < *n; i++) {
        double angle = (double)i / *n * 2 * M_PI;
        
        // More controlled radius variation for smoother shapes
        double variation = 0.85 + (rand() % 30) / 100.0;  // 0.85 to 1.15
        int r = baseRadius * variation;
        
        vertices[i][0] = centerX + r * cos(angle);
        vertices[i][1] = centerY + r * sin(angle);
        
        // Make sure vertices are in bounds
        if (vertices[i][0] < 0) vertices[i][0] = 0;
        if (vertices[i][0] >= WIDTH) vertices[i][0] = WIDTH - 1;
        if (vertices[i][1] < 0) vertices[i][1] = 0;
        if (vertices[i][1] >= HEIGHT) vertices[i][1] = HEIGHT - 1;
    }
}

// Generate 4 countries with proper spacing
int generateCountries(int desiredCount) {
    // Initialize all countries as invalid
    for (int i = 0; i < MAX_COUNTRIES; i++) {
        countries[i].valid = 0;
    }
    
    int count = 0;
    int maxAttempts = 300; // per country
    int totalMaxAttempts = 3000; // total attempts
    int totalAttempts = 0;
    
    // Pre-distribute country centers for better overall layout
    int centers[MAX_COUNTRIES][2];
    distributeCountryCenters(desiredCount, centers);
    
    while (count < desiredCount && totalAttempts < totalMaxAttempts) {
        int attempts = 0;
        int success = 0;
        
        while (!success && attempts < maxAttempts) {
            attempts++;
            totalAttempts++;
            
            // Generate smooth polygon around pre-distributed centers
            int vertices[MAX_VERTICES][2];
            int numVertices;
            int centerX = centers[count][0];
            int centerY = centers[count][1];
            
            generateSmoothPolygon(vertices, &numVertices, centerX, centerY);
            
            // Check if it overlaps with existing countries
            if (isOverlapping(vertices, numVertices)) {
                // If we've tried many times for this country, slightly adjust the center and try again
                if (attempts % 50 == 0) {
                    centerX += (rand() % 7) - 3;
                    centerY += (rand() % 7) - 3;
                    
                    // Keep center in bounds
                    if (centerX < 10) centerX = 10;
                    if (centerX >= WIDTH-10) centerX = WIDTH-10;
                    if (centerY < 5) centerY = 5;
                    if (centerY >= HEIGHT-5) centerY = HEIGHT-5;
                    
                    centers[count][0] = centerX;
                    centers[count][1] = centerY;
                }
                continue;
            }
            
            // Copy vertices to country
            for (int i = 0; i < numVertices; i++) {
                countries[count].vertices[i][0] = vertices[i][0];
                countries[count].vertices[i][1] = vertices[i][1];
            }
            countries[count].numVertices = numVertices;
            countries[count].centerX = centerX;
            countries[count].centerY = centerY;
            countries[count].baseRadius = 5 + rand() % 4;
            countries[count].valid = 1; // Mark as valid
            
            // Fill the polygon
            fillPolygon(vertices, numVertices, '#');
            
            // Display progress during generation
            if (count % 2 == 0) {
                printMap();
                usleep(FRAME_DELAY * 2);
            }
            
            success = 1;
            count++;
        }
        
        // If we couldn't place this country after max attempts, just skip it
        if (!success) {
            printf("Warning: Could not place country %d\n", count);
            count++; // Move to next country
        }
    }
    
    // Return actual number of valid countries
    int validCount = 0;
    for (int i = 0; i < desiredCount; i++) {
        if (countries[i].valid) validCount++;
    }
    
    return validCount;
}

// Create edge list for only valid countries
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

// Union-Find
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

// Sort edges
int compareEdges(const void *a, const void *b) {
    Edge *e1 = (Edge *)a;
    Edge *e2 = (Edge *)b;
    return (e1->weight > e2->weight) - (e1->weight < e2->weight);
}

// Draw a line with 2 parallel lines
void drawDoubleLine(int x1, int y1, int x2, int y2) {
    int dx = abs(x2 - x1), dy = abs(y2 - y1);
    int sx = x1 < x2 ? 1 : -1;
    int sy = y1 < y2 ? 1 : -1;
    int err = dx - dy;
    int err2;
    
    // Calculate normal vector for offset
    double length = sqrt(dx*dx + dy*dy);
    if (length < 0.0001) return;  // Avoid division by zero
    
    // Perpendicular unit vector
    double nx = -sy / length;
    double ny = sx / length;
    
    // Draw main path
    int cx1 = x1, cy1 = y1;
    while (1) {
        if (cx1 >= 0 && cx1 < WIDTH && cy1 >= 0 && cy1 < HEIGHT) {
            if (map[cy1][cx1] == '.') map[cy1][cx1] = '+';
        }
        
        // Fill diagonal gaps
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
    
    // Draw parallel path with offset
    int cx2 = x1 + round(nx), cy2 = y1 + round(ny);
    err = dx - dy;
    
    while (1) {
        if (cx2 >= 0 && cx2 < WIDTH && cy2 >= 0 && cy2 < HEIGHT) {
            if (map[cy2][cx2] == '.') map[cy2][cx2] = '=';
        }
        
        // Fill diagonal gaps
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
    
    // Update display
    printMap();
    usleep(FRAME_DELAY);
}

// Modified Kruskal algorithm to only connect valid countries
void kruskalAndDrawDoubleLine(int n) {
    // Initialize parent array for only valid countries
    int validCountries[MAX_COUNTRIES];
    int validCount = 0;
    
    for (int i = 0; i < n; i++) {
        if (countries[i].valid) {
            parent[i] = i;
            validCountries[validCount++] = i;
        }
    }
    
    if (validCount <= 1) return; // Nothing to connect
    
    qsort(edges, edgeCount, sizeof(Edge), compareEdges);

    int added = 0;
    for (int i = 0; i < edgeCount && added < validCount - 1; i++) {
        int u = edges[i].u, v = edges[i].v;
        
        // Skip invalid countries
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

// Initialize player position on a valid country
void initializePlayerPosition(int numCountries) {
    // Find first valid country
    for (int i = 0; i < numCountries; i++) {
        if (countries[i].valid) {
            playerX = countries[i].centerX;
            playerY = countries[i].centerY;
            return;
        }
    }
    
    // Fallback if no valid countries (shouldn't happen)
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

// Enhanced player movement logic
void movePlayer(char direction) {
    int newX = playerX, newY = playerY;
    if (direction == 'w') newY--; // Up
    if (direction == 's') newY++; // Down
    if (direction == 'a') newX--; // Left
    if (direction == 'd') newX++; // Right

    // Check if new position is within bounds and on a valid path
    if (newX >= 0 && newX < WIDTH && newY >= 0 && newY < HEIGHT && 
        (map[newY][newX] == '#' || map[newY][newX] == '+' || map[newY][newX] == '=')) {
        playerX = newX;
        playerY = newY;
    }
}

int main() {
    srand(time(NULL));
    printf("\033[2J"); // Clear screen

    int numCountries = 5; // Changed to exactly 4 countries

    clearMap();
    int validCount = generateCountries(numCountries);
    printf("Successfully generated %d countries\n", validCount);
    
    createEdges(numCountries);

    // Connect the countries with player hidden
    showPlayer = 0; // Hide player during path generation
    kruskalAndDrawDoubleLine(numCountries);
    
    // Initialize player position on a valid location
    initializePlayerPosition(numCountries);
    
    // Now show the player for gameplay
    showPlayer = 1;
    printMap();
    sleep(1);

    // Now allow user to move
    char move;
    while (1) {
        printf("Move player (w/a/s/d): ");
        scanf(" %c", &move);
        movePlayer(move);
        printMap();
    }

    printf("\n\033[0mDone!\n");
    return 0;
}
