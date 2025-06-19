#ifndef HOUSEMAP_H
#define HOUSEMAP_H

#define GRID_ROWS 20
#define GRID_COLS 20

struct House {
    int id;
    char name[100];
    char owner[100];
    int gridX, gridY;
    int electricPower;          
    int internetSpeed;  
    char location[100];
};

struct HouseNode {
    House house;
    HouseNode *left, *right;
    int height;
};
typedef struct Chunk Chunk;

extern HouseNode* houseRoot;

void drawGrid(int cursorX, int cursorY, HouseNode* root, int gridW, int gridH);
void housePlacementLoop();
HouseNode* insertHouse(HouseNode* root, House house);
HouseNode* searchHouse(HouseNode* root, int gridX, int gridY);
void saveHousesToFile(const char* filename);
void loadHousesFromFile(const char* filename);
void housePlacementLoopForChunk(Chunk* cityChunk);
HouseNode* copyHouseTree(HouseNode* root);
HouseNode* copyChunkData(Chunk* srcChunk, Chunk* destChunk);
void freeHouseTree(HouseNode* root);
#endif