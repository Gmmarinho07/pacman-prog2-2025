#ifndef GAME_DATA_H
#define GAME_DATA_H

#include "game.h"

// Estrutura pra salvar o jogo

typedef struct {
    int score;
    int lives;
    int level;
    int pelletsRemaining;
    bool powerMode;
    float powerTimer;

    // Posicao Pacman
    Position pacmanPos; 

    // Fantasmas
    int ghostCount;
    Position ghostPositions[4]; 
    GhostMode ghostModes[4];
    
    // O mapa em si
    TileType mapTiles[MAP_ROWS * MAP_COLS]; 
} GameSaveData;


// Funções de salvamento
bool SaveGame(const GameState *game, const char *filename);
bool LoadGame(GameState *game, const char *filename);

#endif 