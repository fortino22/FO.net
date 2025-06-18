#ifndef CITYMAP_H
#define CITYMAP_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>

#define CITY_WIDTH  80
#define CITY_HEIGHT 24
#define MAX_CHUNKS 25
#define FRAME_DELAY 20000
#define CHUNK_MARGIN 1

typedef struct {
    int x, y;         
    int width;        
    int height;     
    int valid;      
} Chunk;

typedef struct {
    int src, dest;  
    double weight;  
    bool inMST;      
} CityEdge;

extern Chunk chunks[MAX_CHUNKS];
extern char cityMap[CITY_HEIGHT][CITY_WIDTH];
extern int cityPlayerX, cityPlayerY;
extern int showCityPlayer;

void clearCityMap(void);
void printCityMap(void);
void generateCityChunks(int count);
bool isChunkOverlapping(int x, int y, int width, int height);
void fillChunk(int x, int y, int width, int height);
void primMST(int numChunks);
void drawRoad(int x1, int y1, int x2, int y2);
void initializePlayerPosition(void);
void movePlayerInCity(char direction);
int initializeCity(int numChunks);
void citySandbox(void);
void addBorderRoads(void);
void addPerimeterRoads(void);
#endif