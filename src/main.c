
#include "raylib.h"
#include "game.h"


int main(void);

int main(void) {
    GameState game;

    if (!InitGame(&game, "maps/mapa1.txt")) {
        return 1; // erro ao carregar o mapa
    }

    while (!WindowShouldClose()) {
        float dt = GetFrameTime(); // tempo desde o último frame
        UpdateGame(&game, dt);
        RenderGame(&game);
    }

    CleanupGame(&game);
    return 0;
}
