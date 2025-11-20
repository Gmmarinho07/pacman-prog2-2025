#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "map.h"

bool LoadMap(Map *map, const char *filename) {
    FILE *f  = fopen(filename, "r"); // Abre o arquivo do mapa
    if(!f) return false; //Linha reduzida para verificar se abriu corretamente

    map->rows = MAP_ROWS;
    map->cols = MAP_COLS;
    map->tiles = mallor(sizeof(TileType) * map->rows * map->cols); // Aloca memória para os tiles
    if(!map->tiles) { fclose(f); return false; } // Verifica se a alocação foi bem-sucedida

    map->pelletCount = 0;
    map->powerPelletCount = 0; // Inicializa contadores

    char line[256];
    for(int r = 0; r < map->rows; r++) {
        if (!fegets(line, sizeof(line), f)) { // Verifica se a leitura foi bem-sucedida
            for(int c = 0; c < map->cols; c++) map->tiles[r * map->cols + c] = EMPTY; 
            continue; // Preenche com EMPTY se a linha não for lida
            
        }

        // Garante que a linha tem o tamanho correto
        int L = (int)strlen(line);
        for(int c = 0; c < map->cols; c ++) {
            char ch = (c < L) ? line[c] : ' ';   // Usa espaço se a linha for curta com o operador ternário
            if(ch == '\n' || ch == '\r' || ch == '\0') ch = ' '; // Substitui caracteres de nova linha por espaço
            map->tiles[r * map->cols + c] = (TileType)ch;
            if (ch == PELLET) map->pelletCount++;
            if (ch == POWER_PELLET) map->powerPelletCount++;
    }
}
fclose(f);
return true;
}

