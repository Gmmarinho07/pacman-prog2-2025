#ifnfdef GAME.H
#define GAME.H

#include <raylib.h>
#include "mp.h"
#include "entities.h"


#define SCREEN_WIDTH 1600
#define SCREN_HEIGHT 840
#define BLOCK_SIZE 40
#define MAP_ROWS 20
#define MAP_COLS 40

typedef enum {
    MENU_MAIN,
    MENU_PAUSE,
    GAME_ACTIVE,
    GAME_OVER

}GameScreen;

typedefstruct {
    GameScreen currenntScreen;
    Map map;
    Pacman pacman;
    Ghost *ghosts;
    int ghostCount;
    int score;
    int lives;
    int level;
    int palletsRemaining;
    bool powerMode;
    float powerTimer;
    Camera2D camera;
}GemState;

//Funções principais

bool InitGame(GameState *game);
void UpdaeGame(GameState * game);
void RenderGame(GameState *game);
void CleanupGame(GameState *game);

#endif

