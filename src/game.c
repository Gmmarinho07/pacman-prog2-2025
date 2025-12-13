#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "game.h"
#include "map.h"
#include "entities.h"
#include "raylib.h"
#include "game_data.h"  

#define HUD_HEIGHT 40
#define MAPFILE_MAX 256
#define SAVE_FILE "pacman_save.bin"

// Sprites dos personagens

static void DrawPacmanSprite(const GameState *game) {
    int cx = game->pacman.pos.col * BLOCK_SIZE + BLOCK_SIZE / 2;
    int cy = game->pacman.pos.row * BLOCK_SIZE + BLOCK_SIZE / 2;
    float radius = BLOCK_SIZE / 2.0f - 6.0f;

    static int frame = 0;
    frame++;
    bool mouthOpen = ((frame / 8) % 2) == 0; // Alterna a boca aberta e fechada

    float startAngle = 0.0f;
    float endAngle = 360.0f;

    if (mouthOpen) {
        switch (game->pacman.pos.direction) {
            case DIR_RIGHT: startAngle = 30.0f; endAngle = 330.0f; break;
            case DIR_LEFT:  startAngle = 210.0f; endAngle = 150.0f; break;
            case DIR_UP:    startAngle = 120.0f; endAngle = 420.0f; break;
            case DIR_DOWN:  startAngle = -60.0f; endAngle = 240.0f; break;
            default: break;
        }
        DrawCircleSector((Vector2){cx, cy}, radius, startAngle, endAngle, 0, YELLOW);
    } else {
        DrawCircle(cx, cy, radius, YELLOW);
    }
}

// Gera Pacman e Fantasmas a partir do mapa
static void SpawnEntitiesFromMap(GameState *game) {
    Color ghostColors[4] = { RED, PINK, ORANGE, BLUE };

    bool pacmanFound = false;
    bool ghostsSpawned = false;

    // Limpa fantasmas existentes
    if (game->ghosts) {
        free(game->ghosts);
        game->ghosts = NULL;
        game->ghostCount = 0;
    }

    for (int r = 0; r < game->map->rows; r++) {
        for (int c = 0; c < game->map->cols; c++) {
            TileType t = GetTile(game->map, r, c);

            if (t == PACMAN_START) {
                game->pacmanBaseRow = r;
                game->pacmanBaseCol = c;
                InitPacman(&game->pacman, r, c);
                SetTile(game->map, r, c, EMPTY);
                pacmanFound = true;
            }
            else if (t == GHOST_START && !ghostsSpawned) {
                game->ghostBaseRow = r;
                game->ghostBaseCol = c;

                game->ghostCount = 4;
                game->ghosts = malloc(sizeof(Ghost) * game->ghostCount);
                if (!game->ghosts) {
                    game->ghostCount = 0;
                    ghostsSpawned = true; // evita repetição se alocação falhar
                    continue;
                }
                for (int i = 0; i < game->ghostCount; i++) {
                    InitGhost(&game->ghosts[i], r, c, ghostColors[i]);
                }

                SetTile(game->map, r, c, EMPTY);
                ghostsSpawned = true;
            }
        }
    }

    if (!pacmanFound) {
        printf("AVISO: Pacman não encontrado no mapa, usando posição (1,1)\n");
        InitPacman(&game->pacman, 1, 1);
    }
}

// Inicializa o jogo

bool InitGame(GameState *game, const char *mapFile) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Pac-Man - Prog2");
    SetTargetFPS(60);

    
    strncpy(game->mapFile, mapFile, sizeof(game->mapFile));
    game->mapFile[sizeof(game->mapFile)-1] = '\0';

    game->map = malloc(sizeof(Map));
    if (!game->map) return false;

    if (!LoadMap(game->map, mapFile)) {
        printf("Erro ao carregar o mapa: %s\n", mapFile);
        return false;
    }

    // Inicialização da câmera
    game->camera.target = (Vector2){ SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f };
    game->camera.offset = (Vector2){ 0.0f, 0.0f };
    game->camera.rotation = 0.0f;
    game->camera.zoom = 1.0f;

    game->currentScreen = MENU_MAIN;

    // Inicializa estado base do jogo
    game->ghostCount = 0;
    game->ghosts = NULL;
    game->score = 0;
    game->level = 1;
    game->powerMode = false;
    game->powerTimer = 0.0f;

    // Calcula pellets restantes
    game->pelletsRemaining = game->map->pelletCount + game->map->powerPelletCount;

    SpawnEntitiesFromMap(game);

    return true;
}

// Reinicia o nível sem fechar a janela
bool ResetLevel(GameState *game, const char *mapFile) {

    if (game->ghosts) {
        free(game->ghosts);
        game->ghosts = NULL;
    }

    if (game->map) {
        FreeMap(game->map);
    }

    if (!LoadMap(game->map, mapFile)) {
        printf("Erro ao recarregar mapa: %s\n", mapFile);
        return false;
    }

    game->score = 0;
    game->pacman.lives = 3; // Resetar vidas
    game->powerMode = false;
    game->powerTimer = 0.0f;
    game->pelletsRemaining = game->map->pelletCount + game->map->powerPelletCount;
    game->ghostCount = 0;

    SpawnEntitiesFromMap(game);

    game->currentScreen = MENU_MAIN;
    return true;
}

// Atualiza o estado do jogo

void UpdateGame(GameState *game, float dt) {
    switch (game->currentScreen) {

        case MENU_MAIN:
            if (IsKeyPressed(KEY_ENTER)) {
                
                if (game->pacman.lives <= 0 || game->pelletsRemaining <= 0)
                    ResetLevel(game, game->mapFile);

                game->currentScreen = GAME_ACTIVE;
            }
            if (IsKeyPressed(KEY_ESCAPE)) {
                CloseWindow();
            }
            break;

        case GAME_ACTIVE: {
            // Pressionar TAB pausa o jogo
            if (IsKeyPressed(KEY_TAB)) {
                game->currentScreen = GAME_PAUSED;
                return;
            }

            // -------- PACMAN --------

            int pacmanPrevRow = game->pacman.pos.row; // para detectar mudança de tile
            int pacmanPrevCol = game->pacman.pos.col;
            int ghostPrevRow[4] = {0};
            int ghostPrevCol[4] = {0};

            PacmanEvent ev = UpdatePacman(&game->pacman, game->map, dt);

            if (ev == PACMAN_EVENT_PELLET) {
                game->score += 10;
                game->pelletsRemaining--;
            }
            else if (ev == PACMAN_EVENT_POWER_PELLET) {
                game->score += 50;
                game->pelletsRemaining--;
                game->powerMode = true;
                game->powerTimer = POWER_MODE_TIME;

                for (int i = 0; i < game->ghostCount; i++) {
                    game->ghosts[i].mode = FRIGHTENED;
                    game->ghosts[i].frightenedTimer = POWER_MODE_TIME;
                }
            }

            // -------- FANTASMAS --------
            for (int i = 0; i < game->ghostCount; i++) {
                Ghost *g = &game->ghosts[i];

                 ghostPrevRow[i] = g->pos.row; // para detectar mudança de tile
                 ghostPrevCol[i] = g->pos.col;

                if (g->mode == FRIGHTENED) {
                    g->speed = 0.33f; // mais lento
                } else {
                    g->speed = 0.25f; // normal
                }
                UpdateGhost(&game->ghosts[i], game->map, &game->pacman, dt);
                g->pos.moving = false;
            }

            // -------- POWER TIMER --------
            if (game->powerMode) {
                game->powerTimer -= dt;
                if (game->powerTimer <= 0.0f) {
                    game->powerMode = false;
                    for (int i = 0; i < game->ghostCount; i++) {
                        game->ghosts[i].mode = SCATTER;
                    }
                }
            }

            // -------- COLISÕES --------
            for (int i = 0; i < game->ghostCount; i++) {
                Ghost *g = &game->ghosts[i];

                bool sameTile = 
                    g->pos.row == game->pacman.pos.row &&
                    g->pos.col == game->pacman.pos.col;
                    
                bool crossed = 
                    g->pos.row == pacmanPrevRow &&
                    g->pos.col == pacmanPrevCol &&
                    ghostPrevRow[i] == game->pacman.pos.row &&
                    ghostPrevCol[i] == game->pacman.pos.col;
                    
                    
                    if (sameTile || crossed) // colisão detectada
                    {

                    if (game->powerMode && g->mode == FRIGHTENED) {
                        game->score += 100;
                        g->pos.row = game->ghostBaseRow;
                        g->pos.col = game->ghostBaseCol;
                        g->mode = SCATTER;
                    }
                    else {
                        game->pacman.lives--;
                        game->score = (game->score >= 200) ? game->score - 200 : 0;
                        if (game->pacman.lives <= 0) {
                            game->currentScreen = GAME_OVER;
                        }
                        else {
                            // Conseto do Erro apontado
                            game->pacman.pos.row = game->pacmanBaseRow;
                            game->pacman.pos.col = game->pacmanBaseCol;
                            game->pacman.pos.direction = DIR_RIGHT;
                            game->pacman.pos.nextDirection = DIR_RIGHT;
                            game->pacman.moveTimer = 0.0f;


                            // E agoa reposiciona os fantasmas

                            for(int g = 0; g < game->ghostCount; g++) {
                                game->ghosts[g].pos.row = game->ghostBaseRow;
                                game->ghosts[g].pos.col = game->ghostBaseCol;
                                game->ghosts[g].mode = SCATTER;
                                game->ghosts[g].moveTimer = 0.0f;
                            }
                            
                        }
                    }
                }
            }

            // -------- VITÓRIA --------
            if (game->pelletsRemaining <= 0) {
                game->currentScreen = GAME_OVER;
            }

            break;
        }

        case GAME_PAUSED: {
            // Voltar ao jogo
            if (IsKeyPressed(KEY_V)) {
                game->currentScreen = GAME_ACTIVE;
            }
            
            else if (IsKeyPressed(KEY_N)) {
                game->pacman.lives = 3;
                game->score = 0;
                game->level = 1;
                ResetLevel(game, game->mapFile);
                game->currentScreen = GAME_ACTIVE;
            }
            // Salvar jogo
            else if (IsKeyPressed(KEY_S)) {
                if (SaveGame(game, SAVE_FILE)) {
                    printf("Jogo salvo com sucesso!\n");
                } else {
                    printf("Erro ao salvar o jogo!\n");
                }
            }
            // Carregar jogo
            else if (IsKeyPressed(KEY_C)) {
                if (LoadGame(game, SAVE_FILE)) {
                    printf("Jogo carregado com sucesso!\n");
                    game->currentScreen = GAME_ACTIVE;
                } else {
                    printf("Erro ao carregar o jogo!\n");
                }
            }
            // Sair do jogo
            else if (IsKeyPressed(KEY_Q)) {
                CloseWindow();
            }
            break;
        }

        case GAME_OVER:
            if (IsKeyPressed(KEY_ENTER)) {
                /* Reinicia o nível atual */
                ResetLevel(game, game->mapFile);
                game->currentScreen = MENU_MAIN;
            }
            if (IsKeyPressed(KEY_ESCAPE)) {
                CloseWindow();
            }
            break;
    } // switch
}

// Renderiza o estado atual do jogo

void RenderGame(GameState *game) {
    BeginDrawing();
    ClearBackground(BLACK);

    switch (game->currentScreen) {

        case MENU_MAIN:
            DrawText("PAC-MAN (Prog2)", 480, 250, 60, YELLOW);
            DrawText("Pressione ENTER para iniciar", 480, 350, 30, WHITE);
            DrawText("Setas: mover | ESC: sair", 480, 400, 22, GRAY);
            break;

        case GAME_ACTIVE:
        case GAME_OVER: {
            
            for (int r = 0; r < game->map->rows; r++) {
                for (int c = 0; c < game->map->cols; c++) {
                    int x = c * BLOCK_SIZE;
                    int y = r * BLOCK_SIZE;
                    TileType t = GetTile(game->map, r, c);

                    if (t == WALL)
                        DrawRectangle(x, y, BLOCK_SIZE, BLOCK_SIZE, BLUE);
                    else if (t == PELLET)
                        DrawCircle(x + BLOCK_SIZE / 2, y + BLOCK_SIZE / 2, 4, WHITE);
                    else if (t == POWER_PELLET)
                        DrawCircle(x + BLOCK_SIZE / 2, y + BLOCK_SIZE / 2, 8, GREEN);
                    else if (t == PORTAL)
                        DrawRectangle(x, y, BLOCK_SIZE, BLOCK_SIZE, PINK);
                }
            }

            DrawPacmanSprite(game);

            for (int i = 0; i < game->ghostCount; i++) {
                Ghost *g = &game->ghosts[i];
                if (!g->alive) continue;

                int cx = g->pos.col * BLOCK_SIZE + BLOCK_SIZE / 2;
                int cy = g->pos.row * BLOCK_SIZE + BLOCK_SIZE / 2;

                float radius = BLOCK_SIZE / 2.0f - 6.0f;

                Color drawColor = g->color;
                if (game->powerMode && g->mode == FRIGHTENED)
                    drawColor = WHITE;

                DrawCircle(cx, cy, radius, drawColor);
            }

            DrawText(
                TextFormat("Score: %06d | Lives: %d | Pellets: %d",
                    game->score, game->pacman.lives, game->pelletsRemaining),
                10, game->map->rows * BLOCK_SIZE + 8, 20, WHITE
            );

            if (game->currentScreen == GAME_OVER) {
                DrawText("GAME OVER!", 600, 350, 50, RED);
                DrawText("Pressione ENTER para reiniciar ou ESC para sair.",
                         520, 410, 22, WHITE);
            }

            break;
        }

        case GAME_PAUSED: {
            DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLACK, 0.8f));

            DrawText("PAUSADO", 650, 200, 40, WHITE);
            DrawText("Pressione uma opcao:", 600, 260, 25, GRAY);

            DrawText("N - Novo Jogo", 650, 320, 30, YELLOW);
            DrawText("V - Voltar ao Jogo", 650, 370, 30, YELLOW);
            DrawText("S - Salvar Jogo", 650, 420, 30, YELLOW);
            DrawText("C - Carregar Jogo", 650, 470, 30, YELLOW);
            DrawText("ESC - Sair do Jogo", 650, 520, 30, YELLOW);
            break;
        }
    }

    EndDrawing();
}

//Limpeza e fechamento do jogo

void CleanupGame(GameState *game) {
    if (game->map) {
        FreeMap(game->map);
        free(game->map);
        game->map = NULL;
    }

    if (game->ghosts) {
        free(game->ghosts);
        game->ghosts = NULL;
    }

    CloseWindow();
}
