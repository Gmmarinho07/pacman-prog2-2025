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
#define HUD_HEIGHT 40

// Duração do modo power-up em segundos
#define POWER_MODE_TIME 8.0f


typedef enum {
    MENU_MAIN,
    GAME_ACTIVE,
    GAME_OVER
} GameScreen;  // Telas do jogo

typedef struct {
    GameScreen currentScreen;

    Map *map;
    Pacman pacman;

    Ghost *ghosts;
    int ghostCount; // Número de fantasmas

    int score;
    int level;
    int pelletsRemaining;

    bool powerMode;
    float powerTimer;

    int ghostBaseRow;
    int ghostBaseCol;

    Camera2D camera; // Câmera do jogo

    char mapFile[256]; // armazena o nome do arquivo do mapa

} GameState;


// Funções princiapais do jogo

bool InitGame(GameState *game, const char *mapFile);
bool ResetLevel(GameState *game, const char *mapFile); // Reinicia o nível atual
void UpdateGame(GameState *game, float dt);
void RenderGame(GameState *game); // Renderiza o jogo
void CleanupGame(GameState *game); // Limpa os recursos do jogo

#endif
