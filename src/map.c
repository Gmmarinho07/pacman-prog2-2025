#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "map.h"

bool LoadMap(Map *map, const char *filename) {
    FILE *f  = fopen(filename, "r"); // Abre o arquivo do mapa
    if(!f) return false; //Linha reduzida para verificar se abriu corretamente

    map->rows = MAP_ROWS;
    map->cols = MAP_COLS;
    map->tiles = malloc(sizeof(TileType) * map->rows * map->cols); 
    if(!map->tiles) { fclose(f); return false; }

    map->pelletCount = 0;
    map->powerPelletCount = 0;

    char line[256];
    for(int r = 0; r < map->rows; r++) {

        if (!fgets(line, sizeof(line), f)) {
            for(int c = 0; c < map->cols; c++)
                map->tiles[r * map->cols + c] = EMPTY;
            continue;
        }

        int len = (int)strlen(line);

        for(int c = 0; c < map->cols; c++) {

            char ch = (c < len) ? line[c] : ' ';
            if(ch == '\n' || ch == '\r') ch = ' ';

            map->tiles[r * map->cols + c] = (TileType)ch;

            if (ch == PELLET) map->pelletCount++;
            if (ch == POWER_PELLET) map->powerPelletCount++;
        }
    }

    fclose(f);
    return true;
}

void FreeMap(Map *map) {
    if (!map) return;

    if (map->tiles) {
        free(map->tiles);
        map->tiles = NULL;
    }

    
    map->rows = 0;
    map->cols = 0;
    map->pelletCount = 0;
    map->powerPelletCount = 0;
}

TileType GetTile(const Map *map, int row, int col) {
    if (!map || !map->tiles) return WALL;
    if (row < 0 || col < 0 || row >= map->rows || col >= map->cols)
        return WALL;  
    return map->tiles[row * map->cols + col];
}

void SetTile(Map *map, int row, int col, TileType tile) {
    if (!map || !map->tiles) return;
    if (row < 0 || col < 0 || row >= map->rows || col >= map->cols) return;
    TileType prev = map->tiles[row * map->cols + col];
    if (prev == POWER_PELLET && tile == EMPTY) { // Aqui tentei um debug porem n consegui ver o erro deixei pq fez parte do processo
        printf("[DEBUG SetTile] POWER_PELLET -> EMPTY at r=%d c=%d (prev='%c').\n", row, col, (char)prev);
    }

    map->tiles[row * map->cols + col] = tile;
}

bool IsWalkable(TileType tile) {
    return tile == EMPTY || 
           tile == PELLET || 
           tile == POWER_PELLET || 
           tile == PORTAL || 
           tile == PACMAN_START ||
           tile == GHOST_START;
}

void FindOtherPortal(const Map *map, int inR, int inC, int *outR, int *outC) {
    if (!map || !map->tiles) {
        *outR = -1;
        *outC = -1;
        return;
    }

    for (int r = 0; r < map->rows; r++) {
        for (int c = 0; c < map->cols; c++) {

            if (r == inR && c == inC) continue;

            if (map->tiles[r * map->cols + c] == PORTAL) {
                *outR = r;
                *outC = c;
                return;
            }
        }
    }

    *outR = -1;
    *outC = -1;
}
