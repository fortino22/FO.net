#include "citymap.h" 

#ifndef WORLDMAP_H
#define WORLDMAP_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <unistd.h>

#define WIDTH   100
#define HEIGHT 30
#define MAX_COUNTRIES 5
#define MAX_VERTICES 15
#define COUNTRY_MARGIN 5

typedef struct {
    int vertices[MAX_VERTICES][2]; 
    int numVertices;
    int centerX, centerY;
    int baseRadius;  
    int valid;  
    char *name;  
    int cityGenerated;  
    int cityNumChunks;  
    char savedCityMap[CITY_HEIGHT][CITY_WIDTH];   
    Chunk savedChunks[MAX_CHUNKS]; 
    int lastCityPlayerX;
    int lastCityPlayerY;             
} Country;

typedef struct {
    int u, v;
    double weight;
} Edge;

extern Country countries[MAX_COUNTRIES];
extern Edge edges[MAX_COUNTRIES * MAX_COUNTRIES];
extern int parent[MAX_COUNTRIES];
extern char map[HEIGHT][WIDTH];
extern int edgeCount;
extern int playerX, playerY;
extern int showPlayer;

void clearMap(void);
void printMap(void);
int isInside(int x, int y, int vertices[][2], int n);
void fillPolygon(int vertices[][2], int n, char ch);
int isOverlapping(int vertices[][2], int n);
void distributeCountryCenters(int n, int centers[][2]);
void generateSmoothPolygon(int vertices[][2], int *n, int centerX, int centerY);
int generateCountries(int desiredCount);
void createEdges(int n);
int find(int x);
int unionSet(int x, int y);
int compareEdges(const void *a, const void *b);
void drawDoubleLine(int x1, int y1, int x2, int y2);
void kruskalAndDrawDoubleLine(int n);
void initializePlayerPosition(int numCountries);
void movePlayer(char direction, int numCountries);

int initializeWorld(int numCountries);
void startGameLoop(int numCountries);

#endif