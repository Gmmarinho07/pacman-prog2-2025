#ifndef GAME_DATA_H
#define GAME_DATA_H

#include "game.h"

// Estrutura que representa o estado completo do jogo a ser salvo em BINARIO (Req. 2.10)
// Deve conter tudo que é necessário para restaurar o jogo.
typedef struct {
    int score;
    int lives;
    int level;
    int pelletsRemaining;
    bool powerMode;
    float powerTimer;

    // Posicao Pacman
    Position pacmanPos; 

    // Fantasmas (Armazenaremos até 4 fantasmas para simplificar)
    int ghostCount;
    Position ghostPositions[4]; 
    GhostMode ghostModes[4];
    
    // O mapa em si (o array dinâmico precisa ser transformado em array estático para salvar)
    TileType mapTiles[MAP_ROWS * MAP_COLS]; 
} GameSaveData;


// Prototipos das funcoes de salvar/carregar
bool SaveGame(const GameState *game, const char *filename);
bool LoadGame(GameState *game, const char *filename);

#endif // GAME_DATA_H