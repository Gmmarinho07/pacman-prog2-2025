// src/main.c
#include "raylib.h"
#include "game.h"

// Protótipo de main (usado pela WinMain)
int main(void);

int main(void) {
    GameState game;

    if (!InitGame(&game, "maps/mapa1.txt")) {
        return 1; // erro ao carregar o mapa
    }

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        UpdateGame(&game, dt);
        RenderGame(&game);
    }

    CleanupGame(&game);
    return 0;
}

// --- GAMBIS PRO WINDOWS/UCRT64 ---
// Alguns toolchains usam entrada WinMain em vez de main.
// Aqui a gente só redireciona WinMain -> main.
int WinMain() {
    return main();
}
