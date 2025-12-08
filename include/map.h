#ifndef MAP_H
#define MAP_H

#include <stdbool.h>

typedef enum {
    EMPTY  = ' ',
    WALL   = '#',
    PELLET = '.',
    POWER_PELLET = 'o',
    PACMAN_START = 'P',
    GHOST_START  = 'F',
    PORTAL   = 'T',

} TileType;

typedef struct {
    int rows;
    int cols;
    TileType *tiles;
    int pelletCount;
    int powerPelletCount;
} Map;

#define MAP_ROWS 20
#define MAP_COLS 40

//Funções do mapa

bool LoadMap(Map *map, const char *filename);
void FreeMap(Map *map);


TileType GetTile(const Map *map, int row, int col);
void SetTile(Map *map, int row, int col, TileType tile);

bool IsWalkable(TileType tile);
void FindOtherPortal(const Map *map, int inR, int inC, int *outR, int *outC);


#endif



