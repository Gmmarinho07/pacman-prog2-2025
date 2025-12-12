#include "game_data.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h> 

// cores dos fantasmas  mantidos
static const Color DEFAULT_GHOST_COLORS[4] = { RED, PINK, ORANGE, BLUE };

// Funcao para salvar
bool SaveGame(const GameState *game, const char *filename) {
    if (!game || !game->map) return false;

    GameSaveData data;
    memset(&data, 0, sizeof(data));

    data.score = game->score;
    data.lives = game->pacman.lives;
    data.level = game->level;
    data.pelletsRemaining = game->pelletsRemaining;
    data.powerMode = game->powerMode;
    data.powerTimer = game->powerTimer;

    data.pacmanPos = game->pacman.pos;
    
    data.ghostCount = (game->ghostCount > 4) ? 4 : game->ghostCount;
    for (int i = 0; i < data.ghostCount; i++) {
        data.ghostPositions[i] = game->ghosts[i].pos;
        data.ghostModes[i] = game->ghosts[i].mode;
    }
    
    
    // garante que map->tiles está alocado no tamanho esperado
    if (!game->map->tiles) return false;
    memcpy(data.mapTiles, game->map->tiles, MAP_ROWS * MAP_COLS * sizeof(TileType));

    // 4. Escreve a struct completa no arquivo
    FILE *f = fopen(filename, "wb");
    if (!f) return false;
    size_t written = fwrite(&data, sizeof(GameSaveData), 1, f);
    fclose(f);

    return written == 1;
}


bool LoadGame(GameState *game, const char *filename) {
    if (!game) return false;

    FILE *f = fopen(filename, "rb"); // 'rb' para leitura binaria
    if (!f) return false;

    GameSaveData data;
    size_t read = fread(&data, sizeof(GameSaveData), 1, f);
    fclose(f);
    if (read != 1) return false;

    // Garante que game->map existe
    if (!game->map) {
        game->map = malloc(sizeof(Map));
        if (!game->map) return false;
        // inicializa valores mínimos
        game->map->rows = MAP_ROWS;
        game->map->cols = MAP_COLS;
        game->map->tiles = NULL;
        game->map->pelletCount = 0;
        game->map->powerPelletCount = 0;
    }

    
    if (!game->map->tiles) {
        game->map->tiles = malloc(MAP_ROWS * MAP_COLS * sizeof(TileType));
        if (!game->map->tiles) return false;
    }

    //restaura o estado do jogo
    game->score = data.score;
    game->pacman.lives = data.lives;
    game->level = data.level;
    game->pelletsRemaining = data.pelletsRemaining;
    game->powerMode = data.powerMode;
    game->powerTimer = data.powerTimer;


    game->pacman.pos = data.pacmanPos;

    
    if (game->ghosts) {
        free(game->ghosts);
        game->ghosts = NULL;
    }

    game->ghostCount = (data.ghostCount > 4) ? 4 : data.ghostCount;
    if (game->ghostCount > 0) {
        game->ghosts = malloc(sizeof(Ghost) * game->ghostCount);
        if (!game->ghosts) {
            game->ghostCount = 0;
            return false;
        }
    }

    for (int i = 0; i < game->ghostCount; i++) {
        
        Color col = DEFAULT_GHOST_COLORS[i % 4];
        InitGhost(&game->ghosts[i], data.ghostPositions[i].row, data.ghostPositions[i].col, col);
        // agora ajuste estado salvado
        game->ghosts[i].pos = data.ghostPositions[i];
        game->ghosts[i].mode = data.ghostModes[i];
    }

    
    memcpy(game->map->tiles, data.mapTiles, MAP_ROWS * MAP_COLS * sizeof(TileType)); // aqui tive que pesquisar oq memcpy faz
    game->map->rows = MAP_ROWS;
    game->map->cols = MAP_COLS;


    game->map->pelletCount = 0;
    game->map->powerPelletCount = 0;
    for (int i = 0; i < MAP_ROWS * MAP_COLS; ++i) {
        if (game->map->tiles[i] == PELLET) game->map->pelletCount++;
        else if (game->map->tiles[i] == POWER_PELLET) game->map->powerPelletCount++;
    }

    return true;
}
