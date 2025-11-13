#ifdef MAP_H
#define MAP_H

include <stdbool.h>

typedef enum {
    EMPTY = ' ',
    WALL = '#',
    PELLET = '.',
    POWER_PELLET = 'o',
    PACMAN_START = 'p',
    GHOST = 'F',
    PORTAL = 'T'

}TileType;

typedef struct {
    int rows;
    int cols;
    char **tiles;
    int pelletCount;
    int powerPelletCount;

}Map;

//Funções do Mapa

bool LoadMap(Map *map, const char *filename);
void FreeMap(Map *map);
TileType GetTile(const Map *map, int row, int col);
void SetTile(Map * map, int row, int col, TileType);
bool IsWalkable(tileType tile);


#endif
