#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "map.h"

bool LoadMap(Map *map, const char *filename) {
    FILE *f  = fopen(filename, "r"); // Abre o arquivo do mapa
    if(!f) return false; //Linha reduzida para verificar se abriu corretamente

    map->rows = MAP_ROWS;
    map->cols = MAP_COLS;
    map->tiles = malloc(sizeof(TileType) * map->rows * map->cols); // Aloca memória para os tiles
    if(!map->tiles) { fclose(f); return false; } // Verifica se a alocação foi bem-sucedida

    map->pelletCount = 0;
    map->powerPelletCount = 0; // Inicializa contadores

    char line[256];
    for(int r = 0; r < map->rows; r++) {
        if (!fgets(line, sizeof(line), f)) { // Verifica se a leitura foi bem-sucedida
            for(int c = 0; c < map->cols; c++) map->tiles[r*map->cols + c] = EMPTY; 
            continue; // Preenche com EMPTY se a linha não for lida
            
        }

        // Garante que a linha tem o tamanho correto
        int len = (int)strlen(line);
        for(int c = 0; c < map->cols; c++) {
            char ch = (c < len) ? line[c] : ' ';   // Usa espaço se a linha for curta com o operador ternário
            if(ch == '\n' || ch == '\r') ch = ' '; // Substitui caracteres de nova linha por espaço
            map->tiles[r*map->cols + c] = (TileType)ch;
            if (ch == PELLET) map->pelletCount++;
            if (ch == POWER_PELLET) map->powerPelletCount++;
    }
}
fclose(f);
return true;
}

void FreeMap(Map *map) {
    if(!map) return;
    free(map->tiles);
    map->tiles = NULL;

}

TileType GetTile(const Map *map, int row, int col) {
    if (!map) return WALL;
    if (row < 0 || col < 0 || row >= map->rows || col >= map->cols) return WALL;  // Fora dos limites  
    return map->tiles[row*map->cols + col];
}

void SetTile( Map *map, int row, int col, TileType tile) {
    if (!map) return;
    if (row < 0 || col < 0 || row >= map->rows || col >= map->cols) return;  // Fora dos limites
    map->tiles[row*map->cols + col] = tile; // Define o tile ou modifica o contador se necessário
    
}

bool IsWalkable(TileType tile) {
    return tile == EMPTY || tile ==PELLET || tile == POWER_PELLET || tile == PORTAL || tile == PACMAN_START; // Tiles que Pacman pode atravessar

}

// Encontrar outro portal no mapa

void FindOtherPortal(const Map *map, int inR, int inC, int *outR, int *outC) {
    if (!map) {*outR = -1; *outC = -1; return;}
    for (int r = 0; r < map->rows; r++) {
        for(int c = 0; c < map->cols; c++) {
            if (r == inR && c == inC) continue; 
            if (map->tiles[r*map->cols + c] == PORTAL) { *outR = r; *outC = c; return;}

        }
    }
    *outR = -1; *outC = -1 ; // Nenhum outro portal encontrado
}