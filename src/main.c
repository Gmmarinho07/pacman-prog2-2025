#include "raylib.h"
#include "game.h"

int main(void) {

    GameState game = {0};

    if (!InitGame(&game, "maps/mapa1.txt")) {
        return -1;
    }

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        UpdateGame(&game, dt);
        RenderGame(&game);
    }

    CleanupGame(&game);

    return 0;
}
