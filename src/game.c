#include <stdio.h>
#include <stdlib.h>
#include "game.h"
#include "map.h"
#include "entities.h"
#include "raylib.h"

#define HUD_HEIGHT 40

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

    game->currentScreen = GAME_ACTIVE;

    game->ghostCount = 0;
    game->ghosts = NULL;
    game->score = 0;
    game->level = 1;
    game->powerMode = false;
    game->powerTimer = 0.0f;
    game->pelletsRemaining = game->map->pelletCount + game->map->powerPelletCount;

    // Procura Pacman e Fantasmas no mapa
    for (int r = 0; r < game->map->rows; r++) {
        for (int c = 0; c < game->map->cols; c++) {
            TileType t = GetTile(game->map, r, c);

            if (t == PACMAN_START) {
                InitPacman(&game->pacman, r, c);
                SetTile(game->map, r, c, EMPTY);
            } else if (t == GHOST_START) {
                game->ghostCount++;
                game->ghosts = realloc(game->ghosts, sizeof(Ghost) * game->ghostCount);
                InitGhost(&game->ghosts[game->ghostCount - 1], r, c, RED);
                SetTile(game->map, r, c, EMPTY);
            }
        }
    }

    return true;
}

void UpdateGame(GameState *game, float dt) {
    // Atualiza o Pacman
    UpdatePacman(&game->pacman, game->map);

    // Atualiza os fantasmas
    for( int i = 0; i < game->ghostCount; i++) {
        UpdateGhost(&game->ghosts[i], game->map /*&game->pacman*/);

    }
    // Tempo do Modo DEUS
    if (game->powerMode) {
        game->powerTimer -= dt;
        if (game->powerTimer <= 0.0f) {
            game->powerMode = false;

            // Retorna os fantasmas ao modo normal
            for(int i = 0; i < game->ghostCount; i++) 
                game->ghosts[i].mode = SCATTER;
        }
    }

    // Verifica colisões entre Pacman e fantasmas (tentei de forma simples)
    for(int i = 0; i < game->ghostCount; i++) {
        Ghost *g = &game->ghosts[i];
        if (!g->alive) continue;
        if (g->pos.row == game->pacman.pos.row && g->pos.col == game->pacman.pos.col) {
            if (g->mode == FRIGHTENED || game->powerMode) {
                game->score += 200; //Pontos por comer o fantasma
                g->alive = false; // Fantasma "morto"
            }else {
                // Pacman perderia uma vida 
                game->pacman.lives--;
                // Reseta a posição de Pacman
                // Teleporta Pacman para a posição inicial
                game->pacman.pos.row = 1;
                game->pacman.pos.col = 1;
            }
        }
    }
}

 void RenderGame(GameState *game) {
    BeginDrawing();
    ClearBackground(BLACK);

    // Desenha o mapa
    for( int r = 0; r < game->map->rows; r++) {
        for(int c = 0; c < game->map->cols; c++){
            int x = c * BLOCK_SIZE;
            int y = r * BLOCK_SIZE;
            TileType t = GetTile(game->map, r, c);
            if(t == WALL) DrawRectangle(x, y, BLOCK_SIZE, BLOCK_SIZE, BLUE);
            else if (t == PELLET) DrawCircle(x + BLOCK_SIZE / 2, y + BLOCK_SIZE / 2, 4, GOLD);
            else if (t == POWER_PELLET) DrawCircle(x + BLOCK_SIZE / 2, y + BLOCK_SIZE / 2, 8,  ORANGE);
            else if (t == PORTAL) DrawRectangle(x, y, BLOCK_SIZE, BLOCK_SIZE, DARKPURPLE);

        }
    }
// Desenha o Pacman
    DrawCircle(game->pacman.pos.col * BLOCK_SIZE + BLOCK_SIZE / 2,
                game->pacman.pos.row * BLOCK_SIZE + BLOCK_SIZE / 2,
                BLOCK_SIZE/2 - 6, YELLOW);

// Desenha os fantasmas
for (int i = 0; i < game->ghostCount; i++) {
    Ghost *g = &game->ghosts[i];
    if (!g->alive) continue; // Não desenha fantasmas "mortos"
    DrawCircle(g->pos.col * BLOCK_SIZE + BLOCK_SIZE / 2,
                g->pos.row * BLOCK_SIZE + BLOCK_SIZE / 2,
                BLOCK_SIZE / 2 - 6, g->color);
            
}

//HUD

DrawText (TextFormat("Score: %d, Lives: %d Pellets: %d", game->score, game->pacman.lives, game->pelletsRemaining),
         10, game->map->rows * BLOCK_SIZE + 8, 20, WHITE);

         EndDrawing();

}



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
   

