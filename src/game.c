#include <stdio.h>
#include <stdlib.h>
#include "game.h"
#include "map.h"
#include "entities.h"
#include "raylib.h"

#define HUD_HEIGHT 40
#define POWER_MODE_TIME 8.0f

// Funções de sprites

static void DrawPacmanSprite(const GameState *game) {
    int cx = game->pacman.pos.col * BLOCK_SIZE + BLOCK_SIZE / 2;
    int cy = game->pacman.pos.row * BLOCK_SIZE + BLOCK_SIZE / 2;
    float radius = BLOCK_SIZE / 2.0f - 6.0f;

    // Uma animação simples da boca
    static int frame = 0;
    frame++;
    bool mouthOpen = ((frame / 8) % 2) == 0; // Alterna a boca aberta e fechada

    float startAngle = 0.0f;
    float endAngle = 360.0f; // Um curculo completo com a boca

    if(mouthOpen) {
        switch (game->pacman.pos.direction) {
            case DIR_RIGHT: startAngle = 30.0f; endAngle = 330.0f; break;
            case DIR_LEFT: startAngle = 210.0f; endAngle = 150.0f; break;
            case DIR_UP: startAngle = 120.0f; endAngle = 420.0f; break;
            case DIR_DOWN: startAngle = -60.0f; endAngle = 240.0f; break;

        }
        DrawCircleSector((Vector2){cx, cy},  radius, startAngle, endAngle, 0, YELLOW);
    } else {
        DrawCircle(cx, cy, radius, YELLOW);

    }
}

// Inicializa o jogo

bool InitGame(GameState *game, const char *mapFile) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Pac-Man - Prog2");
    SetTargetFPS(60);

    game->map = malloc(sizeof(Map));
    if (!game->map) {
        return false;
    }

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

    game->ghostCount = 0;
    game->ghosts = NULL;
    game->score = 0;
    game->level = 1;
    game->powerMode = false;
    game->powerTimer = 0.0f;
    game->pelletsRemaining = game->map->pelletCount + game->map->powerPelletCount;

    game->ghostBaseRow = 1;
    game->ghostBaseCol = 1;


    // Cores dos fantasmas

    Color ghostColors[4] = { RED, PINK, ORANGE, BLUE};

bool ghostsSpawned = false;

    // Procura Pacman e Fantasmas no mapa
    for (int r = 0; r < game->map->rows; r++) {
        for (int c = 0; c < game->map->cols; c++) {
            TileType t = GetTile(game->map, r, c);

            if (t == PACMAN_START) {
                InitPacman(&game->pacman, r, c);
                SetTile(game->map, r, c, EMPTY);
            } else if (t == GHOST_START && !ghostsSpawned) {
                    game->ghostBaseRow = r;
                    game->ghostBaseCol = c;

                    game->ghostCount = 4;
                    game->ghosts = malloc(sizeof(Ghost) * game->ghostCount);
                    for(int i = 0; i < game->ghostCount; i++) {
                        InitGhost(&game->ghosts[i], r, c, ghostColors[i]);

                    }

                    SetTile(game->map, r, c, EMPTY);
                    ghostsSpawned = true;

                }

            }
     }
     return true;
    }


// Atualiza o estado do jogo


void UpdateGame(GameState *game, float dt) {
    switch (game->currentScreen) {
        case MENU_MAIN:
            if (IsKeyPressed(KEY_ENTER)) {
                game->currentScreen = GAME_ACTIVE; // Inicia o jogo
            }
            break;

            case GAME_ACTIVE: {
                // Atualiza o Pacman
                PacmanEvent ev = UpdatePacman(&game->pacman, game->map);

                if( ev == PACMAN_EVENT_PELLET) {
                    game->score += 10;
                    game->pelletsRemaining--;
                } else if ( ev == PACMAN_EVENT_POWER_PELLET) {
                    game->score += 50;
                    game->pelletsRemaining--;
                    game->powerMode = true;
                    game->powerTimer = POWER_MODE_TIME;

                    // Coloca os fantasmas no modo assustado
                    for(int i = 0; i < game->ghostCount; i++) {
                        game->ghosts[i].mode = FRIGHTENED;
                    }
                }

                // Atualiza os fantasmas mais devagares
                static int ghostFrame = 0;
                ghostFrame++;
                bool moveGhosts = (ghostFrame % 8 == 0); // quanto maior, mais devagar

                if (moveGhosts) {
                    for( int i = 0; i < game->ghostCount; i++) {
                        if(!game->ghosts[i].alive) continue; // Não atualiza fantasmas "mortos"
                        UpdateGhost(&game->ghosts[i], game->map, &game->pacman);

                    }
                }
                
                // Tempo do Modo DEUS
                if (game->powerMode) {
                    game->powerTimer -= dt;
                    if(game->powerTimer <= 0.0f) {
                        game->powerMode = false;
                        for(int i = 0; i < game->ghostCount; i++) {
                            if(game->ghosts[i].alive) 
                                game->ghosts[i].mode = SCATTER;  // Retorna os fantasmas ao modo normal
                            }
                        }
                    }

                    // Verifica colisões entre Pacman e fantasmas (tentei de forma simples)
                    for (int i = 0; i < game->ghostCount; i++) {
                        Ghost *g = &game->ghosts[i];
                        if(!g->alive) continue;

                        if(g->pos.row == game->pacman.pos.row &&
                           g->pos.col == game->pacman.pos.col) {
                            
                            if(game->powerMode && g->mode == FRIGHTENED) {
                                // Pacman come o fantasma
                                game->score += 200;
                                // Fantasma "morto"
                                g->pos.row = game->ghostBaseRow;
                                g->pos.col = game->ghostBaseCol;
                                g->mode = SCATTER; // Retorna ao modo normal
                            }else {
                                // Pacman perderia uma vida 
                                game->pacman.lives--;
                                if (game->pacman.lives <= 0) {
                                    game->currentScreen = GAME_OVER; // Fim de jogo
                                } else {
                                    // Reseta a posição de Pacman
                                    game->pacman.pos.row = 1;
                                    game->pacman.pos.col = 1;
                                }
                            }

                        }
                    }

                    // Vitória por pellets

                    if(game->pelletsRemaining <= 0) {
                        game->currentScreen = GAME_OVER;
                    }
                    break;
                }
                case GAME_OVER: 
                if (IsKeyPressed(KEY_ENTER)) {
                    // Reinicia o jogo
                    game->currentScreen = MENU_MAIN;
                    // Aqui mais tarde adciono  o menu para reiniciar
                }
                break;
            }
        }


// Renderiza o jogo
 void RenderGame(GameState *game) {
    BeginDrawing();
    ClearBackground(BLACK);

    switch (game->currentScreen) {
        case MENU_MAIN:
        DrawText("PAC-MAN (Prog2)",480, 250, 60, YELLOW);
        DrawText("Pressione ENTER para iniciar", 480, 350, 30, WHITE);
        DrawText("Setas: mover | ESC: sair", 480, 400, 22, GRAY);
        break;

    case GAME_ACTIVE:
    case GAME_OVER: {
        // Desenha o mapa
        for( int r = 0; r < game->map->rows; r++) {
            for(int c = 0; c < game->map->cols; c++){
                int x= c * BLOCK_SIZE;
                int y = r * BLOCK_SIZE;
                TileType t = GetTile(game->map, r, c);

                if (t == WALL)
                    DrawRectangle(x, y, BLOCK_SIZE, BLOCK_SIZE, BLUE);
                else if (t == PELLET)
                    DrawCircle(x + BLOCK_SIZE / 2, y + BLOCK_SIZE / 2, 4, GOLD);
                else if (t == POWER_PELLET)
                    DrawCircle(x + BLOCK_SIZE / 2, y + BLOCK_SIZE / 2, 8, ORANGE);
                else if (t == PORTAL)
                    DrawRectangle(x, y, BLOCK_SIZE, BLOCK_SIZE, DARKPURPLE);

            }
        }

    // Desenha o Pacman
    DrawPacmanSprite(game);

    // Desenha os fantasmas
    for (int i = 0; i < game->ghostCount; i++) {
        Ghost *g = &game->ghosts[i];
        if (!g->alive) continue; // Não desenha fantasmas "mortos"

        int cx = g->pos.col * BLOCK_SIZE + BLOCK_SIZE / 2;
        int cy = g->pos.row * BLOCK_SIZE + BLOCK_SIZE / 2;
        float radius = BLOCK_SIZE / 2.0f - 6.0f; // Raio do fantasma

        Color drawColor = g->color;
        if(game->powerMode && g->mode == FRIGHTENED) 
            drawColor = SKYBLUE; // Cor diferente no modo assustado

        DrawCircle(cx, cy, radius, drawColor);
    }

    //HUD
    DrawText(
        TextFormat("Score: %d, Lives: %d Pellets: %d",
             game->score, game->pacman.lives,
             game->pelletsRemaining),
    10, game->map->rows * BLOCK_SIZE + 8, 20, WHITE
);

    if(game->currentScreen == GAME_OVER) {
        DrawText("GAME OVER!", 600, 350, 50, RED); // Mensagem de fim de jogo
        DrawText("Feche a janela ou reinicie o jogo.",
                 520, 410, 22, WHITE);
                
    }
    break;
    }
    }

    EndDrawing();
}


// Limpa os recursos do jogo

void CleanupGame(GameState *game) {
    if (game->map) {
        FreeMap(game->map);
        free(game->map);
        game->map = NULL;
    }

    if(game->ghosts) {
        free(game->ghosts);
        game->ghosts = NULL;
    }


    CloseWindow();

}
   

