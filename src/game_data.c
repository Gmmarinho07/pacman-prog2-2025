// src/game_data.c
#include "game_data.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h> 

// Cores padrão para fantasmas — escolha a mesma ordem que usa em SpawnEntitiesFromMap
static const Color DEFAULT_GHOST_COLORS[4] = { RED, PINK, ORANGE, BLUE };

// Funcao para salvar o estado do jogo em arquivo binario (Req. 2.10)
bool SaveGame(const GameState *game, const char *filename) {
    if (!game || !game->map) return false;

    GameSaveData data;
    memset(&data, 0, sizeof(data));

    // 1. Copia o estado do jogo para a struct serializavel
    data.score = game->score;
    data.lives = game->pacman.lives;
    data.level = game->level;
    data.pelletsRemaining = game->pelletsRemaining;
    data.powerMode = game->powerMode;
    data.powerTimer = game->powerTimer;

    data.pacmanPos = game->pacman.pos;
    
    // 2. Copia Fantasmas (limitado a 4)
    data.ghostCount = (game->ghostCount > 4) ? 4 : game->ghostCount;
    for (int i = 0; i < data.ghostCount; i++) {
        data.ghostPositions[i] = game->ghosts[i].pos;
        data.ghostModes[i] = game->ghosts[i].mode;
    }
    
    // 3. Copia o Mapa (matriz plana)
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

// Funcao para carregar o estado do jogo de arquivo binario (Req. 2.10)
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

    // (Re)aloca tiles para o mapa se necessário
    if (!game->map->tiles) {
        game->map->tiles = malloc(MAP_ROWS * MAP_COLS * sizeof(TileType));
        if (!game->map->tiles) return false;
    }

    // 1. Restaura o estado do jogo
    game->score = data.score;
    game->pacman.lives = data.lives;
    game->level = data.level;
    game->pelletsRemaining = data.pelletsRemaining;
    game->powerMode = data.powerMode;
    game->powerTimer = data.powerTimer;

    // 2. Restaura Pacman e Fantasmas
    game->pacman.pos = data.pacmanPos;

    // Reinicializa fantasmas com as posicoes salvas
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
        // inicializa ghost com cor padrão e valores base
        Color col = DEFAULT_GHOST_COLORS[i % 4];
        InitGhost(&game->ghosts[i], data.ghostPositions[i].row, data.ghostPositions[i].col, col);
        // agora ajuste estado salvo
        game->ghosts[i].pos = data.ghostPositions[i];
        game->ghosts[i].mode = data.ghostModes[i];
    }

    // 3. Restaura o Mapa (copia a matriz plana para o array dinâmico)
    memcpy(game->map->tiles, data.mapTiles, MAP_ROWS * MAP_COLS * sizeof(TileType));
    game->map->rows = MAP_ROWS;
    game->map->cols = MAP_COLS;

    // Recalcula contagem de pellets (opcional, coerente com o savedata)
    game->map->pelletCount = 0;
    game->map->powerPelletCount = 0;
    for (int i = 0; i < MAP_ROWS * MAP_COLS; ++i) {
        if (game->map->tiles[i] == PELLET) game->map->pelletCount++;
        else if (game->map->tiles[i] == POWER_PELLET) game->map->powerPelletCount++;
    }

    return true;
}
