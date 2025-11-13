#include <raylib.h>
#include <game.h>

int main(void) {
    GameState game = {0};

    //Inicialização do Jogo

    if(!InitGame(&game)){
        return -1;

    }

    //Sisitema de loop do Jogo

    while(!WindowShouldClose()) {
        UpddateGame(&game);
        RenderGame(&game);

    }
    //Limpeza
    CleanupGame(&game);

    return 0;
}
