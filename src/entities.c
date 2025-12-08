#include <stdlib.h>
#include "entities.h"
#include "map.h"
#include "raylib.h"

// --- Agora sim, PACMAN ---
void InitPacman( Pacman * pacman, int startRow, int startCol) {
    pacman->pos.row = startRow;
    pacman->pos.col = startCol;
    pacman->pos.direction = DIR_RIGHT;
    pacman->pos.nextDirection = DIR_RIGHT;
    pacman->pos.moving = false;
    pacman->lives = 3;
    pacman->alive = true;
    pacman->score = 0;

}

static bool CanMoveTo(Map *map, int row, int col) {
    TileType t = GetTile(map, row, col);
    return IsWalkable(t);
    // Verifica se Pacman pode se mover para a posição (row, col)
}

void MovePacman(Pacman *pacman, Map *map) {
    // Nas Direções: 0=Direita, 1=Baixo, 2=Esquerda, 3=Cima
    const int dr[4] = {0, 1, 0, -1};
    const int dc[4] = {1, 0, -1, 0};

    int nr = pacman->pos.row + dr[pacman->pos.direction];
    int nc = pacman->pos.col + dc[pacman->pos.direction];


    // verificação do portal antes de mover

    TileType dest = GetTile(map, nr, nc);
    if (dest == PORTAL) {
        int or, oc;
        FindOtherPortal(map, nr, nc, &or, &oc);
        if(or >= 0) {
            pacman->pos.row = or;
            pacman->pos.col = oc;
            return; // Sai da função após usar o portal

        } 
    }
// Movimento normal
    if(CanMoveTo(map,nr, nc)) {
        pacman->pos.row = nr;
        pacman->pos.col = nc;

        TileType t = GetTile(map, nr, nc);
        if( t == PELLET) {
            SetTile(map, nr, nc, EMPTY);
            pacman->score += 10;
            map->pelletCount--; // Decrementa o contador de pellets
        } else if( t == POWER_PELLET) {
            SetTile(map, nr, nc, EMPTY);
            pacman->score += 50;
            map->powerPelletCount--; // Decrementa o contador de power pellets
        }
    }
}


// Atualiza o estado do Pacman com base na entrada do jogador
void UpdatePacman(Pacman *pacman, Map *map) {
    if(IsKeyPressed(KEY_UP)) pacman->pos.direction = DIR_UP;
    if (IsKeyPressed(KEY_DOWN)) pacman->pos.direction = DIR_DOWN;
    if (IsKeyPressed(KEY_LEFT)) pacman->pos.direction = DIR_LEFT;
    if (IsKeyPressed(KEY_RIGHT)) pacman->pos.direction = DIR_RIGHT;

    MovePacman(pacman, map); // Tenta mover Pacman na direção atual

}


// --- Agora os Fantasmas ---

void InitGhost(Ghost *ghost, int startRow, int startCol, Color color){
    ghost->pos.row = startRow;
    ghost->pos.col = startCol;
    ghost->pos.direction = DIR_LEFT;
    ghost->pos.nextDirection = DIR_LEFT;
    ghost->pos.moving = false;
    ghost-> mode = SCATTER; // Modo inicial
    ghost->color = color;
    ghost->alive = true;
    ghost-> speed = 4.0f;
    ghost -> frightenedTimer = 0.0f;

}

static int CountAvailableDirections(Map *map, int r, int c, int avail[4]){
    // Essa funções retorna o número de direções disponíveis para o fantasma se mover
    const int dr[4] = {0,1,0,-1};
    const int dc[4] = {1,0,-1,0};
    int count = 0;
    for(int d = 0; d < 4; d++) {
        int nr = r + dr[d];
        int nc = c + dc[d];
        TileType t = GetTile(map, nr, nc);
        if (IsWalkable(t)) { avail[d] = 1; count++;
        }else avail[d] = 0;
    }
    return count;
}

void MoveGhost(Ghost *ghost, Map *map) {
    const int dr[4] = {0,1,0,-1};
    const int dc[4] = {1,0,-1,0};

    int r = ghost->pos.row;
    int c = ghost->pos.col;


    // Verifica se o fantasma pode se mover na direção atual, se não escolhe um pellet aleatório
    int fowardR = r + dr[ghost->pos.direction];
    int fowardC = c + dc[ghost->pos.direction];
    if (IsWalkable(GetTile(map, fowardR, fowardC))) {
        ghost->pos.row = fowardR;
        ghost->pos.col = fowardC;
        return; // Conseguiu se mover
    }

    // Escolhe uma direção aleatória entre as disponíveis
    int avail[4] = {0,0,0,0};
    int cnt = CountAvailableDirections(map, r, c, avail);
    if (cnt == 0) return; // Sem opção de movimento


    // Tenta escolher uma direção aleatória diferente da atual
    int reverse = (ghost->pos.direction + 2) % 4;
    int choices[4]; int n = 0;
    for (int d = 0; d < 4; d++) {
        if (avail[d] && d != reverse) choices[n++] = d; // Evita voltar

    } 
    if (n == 0) {
        for (int d = 0; d < 4; d++)
            if (avail[d]) choices[n++] = d; // Só pode voltar
    }

    int pick = choices[ GetRandomValue(0 , n -1)]; // Escolhe aleatoriamente
    ghost->pos.direction = pick;
    ghost->pos.row += dr[pick];
    ghost->pos.col += dc[pick];

}

void UpdateGhost (Ghost *ghost, Map *map /*Pacman *pacman*/) {
    // Os fantasmas também podem usar portais
    TileType t = GetTile(map, ghost->pos.row, ghost->pos.col);
    if (t == PORTAL) {
        int or, oc;
        FindOtherPortal(map, ghost->pos.row, ghost->pos.col, &or, &oc);
        if (or >=0) {
            ghost->pos.row = or;
            ghost->pos.col = oc;
        }
    }


MoveGhost(ghost, map); // Tenta mover o fantasma
}
