#ifndef GAME_H
#define GAME_H

#include "raylib.h"
#include "map.h"
#include "entities.h"
#include <stdbool.h>

//Dimensões da tela
#define SCREEN_WIDTH 1600
#define SCREEN_HEIGHT 840

// Tamanho dos tiles
#define BLOCK_SIZE 40



typedef enum {
    MENU_MAIN,
    MENU_PAUSE,
    GAME_ACTIVE,
    GAME_OVER

} GameScreen;  // Telas do jogo

typedef struct {
    GameScreen curreentScreen;

    Map *map;
    Pacman pacman;

    Ghost *ghosts;
    int ghostCount; // Número de fantasmas

    int score;
    int level;
    int pelletsRemaining;

    bool powerMode;

    float powerMode;
    float powerTimer;

    Camera2D camera; // Câmera do jogo

} GameState;


// Funções princiapais do jogo

bool InitGame(GameState *game, const char *mapFile);
void UpdateGame(GameState *game, float dt);
void RenderGame(const GameState *game); // Renderiza o jogo
void CleanupGame(GameState *game); // Limpa os recursos do jogo

#endif
