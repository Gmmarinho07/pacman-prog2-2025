#include <stdlib.h>
#include "entities.h"
#include "map.h"
#include "raylib.h"

// -------------------------------------
// PACMAN
// -------------------------------------

void InitPacman(Pacman *pacman, int startRow, int startCol) {
    pacman->pos.row = startRow;
    pacman->pos.col = startCol;
    pacman->pos.direction = DIR_RIGHT;
    pacman->pos.nextDirection = DIR_RIGHT;
    pacman->pos.moving = false;

    pacman->lives = 3;
    pacman->alive = true;
    pacman->score = 0;

    pacman->moveDelay = 0.12f;
    pacman->moveTimer = 0.0f;
}

static bool CanMoveTo(Map *map, int row, int col) {
    TileType t = GetTile(map, row, col);
    return IsWalkable(t);
}

PacmanEvent MovePacman(Pacman *pacman, Map *map) {
    const int dr[4] = {0,1,0,-1};
    const int dc[4] = {1,0,-1,0};

    int nr = pacman->pos.row + dr[pacman->pos.direction];
    int nc = pacman->pos.col + dc[pacman->pos.direction];

    TileType dest = GetTile(map, nr, nc);

    if (dest == PORTAL) {
        int or, oc;
        FindOtherPortal(map, nr, nc, &or, &oc);
        if (or >= 0) {
            pacman->pos.row = or;
            pacman->pos.col = oc;
            return PACMAN_EVENT_NONE;
        }
    }

    PacmanEvent ev = PACMAN_EVENT_NONE;

    if (CanMoveTo(map, nr, nc)) {
        pacman->pos.row = nr;
        pacman->pos.col = nc;

        TileType t = GetTile(map, nr, nc);

        if (t == PELLET) {
            SetTile(map, nr, nc, EMPTY);
            ev = PACMAN_EVENT_PELLET;
        }
        else if (t == POWER_PELLET) {
            SetTile(map, nr, nc, EMPTY);
            ev = PACMAN_EVENT_POWER_PELLET;
        }
    }

    return ev;
}

PacmanEvent UpdatePacman(Pacman *pacman, Map *map, float dt) {
    if (IsKeyPressed(KEY_UP)) pacman->pos.nextDirection = DIR_UP;
    if (IsKeyPressed(KEY_DOWN)) pacman->pos.nextDirection = DIR_DOWN;
    if (IsKeyPressed(KEY_LEFT)) pacman->pos.nextDirection = DIR_LEFT;
    if (IsKeyPressed(KEY_RIGHT)) pacman->pos.nextDirection = DIR_RIGHT;

    pacman->moveTimer -= dt;
    if (pacman->moveTimer > 0.0f) return PACMAN_EVENT_NONE;

    const int dr[4] = {0,1,0,-1};
    const int dc[4] = {1,0,-1,0};

    int tryRow = pacman->pos.row + dr[pacman->pos.nextDirection];
    int tryCol = pacman->pos.col + dc[pacman->pos.nextDirection];

    // Só vira se puder
    if (CanMoveTo(map, tryRow, tryCol)) {
        pacman->pos.direction = pacman->pos.nextDirection;
    }

    PacmanEvent ev = MovePacman(pacman, map);
    pacman->moveTimer = pacman->moveDelay;

    return ev;
}

// -------------------------------------
// FANTASMAS
// -------------------------------------

void InitGhost(Ghost *ghost, int startRow, int startCol, Color color) {
    ghost->pos.row = startRow;
    ghost->pos.col = startCol;
    ghost->pos.direction = DIR_LEFT;
    ghost->pos.nextDirection = DIR_LEFT;

    /* CORREÇÃO: setar pos.moving (não ghost->moving) */
    ghost->pos.moving = true;

    ghost->alive = true;
    ghost->mode = SCATTER;
    ghost->color = color;

    ghost->speed = 0.35f;       // tempo entre passos (maior = mais lento)
    ghost->moveTimer = 0.0f;
    ghost->frightenedTimer = 0.0f;
}


static int CountAvailableDirections(Map *map, int r, int c, int avail[4]) {
    const int dr[4] = {0,1,0,-1};
    const int dc[4] = {1,0,-1,0};

    int count = 0;
    for (int d = 0; d < 4; d++) {
        int nr = r + dr[d];
        int nc = c + dc[d];
        if (IsWalkable(GetTile(map, nr, nc))) {
            avail[d] = 1;
            count++;
        } else {
            avail[d] = 0;
        }
    }
    return count;
}

void MoveGhost(Ghost *ghost, Map *map) {
    const int dr[4] = {0,1,0,-1};
    const int dc[4] = {1,0,-1,0};

    int r = ghost->pos.row;
    int c = ghost->pos.col;

    int fr = r + dr[ghost->pos.direction];
    int fc = c + dc[ghost->pos.direction];

    if (IsWalkable(GetTile(map, fr, fc))) {
        ghost->pos.row = fr;
        ghost->pos.col = fc;
        return;
    }

    int avail[4] = {0};
    int cnt = CountAvailableDirections(map, r, c, avail);
    if (cnt == 0) return;

    int reverse = (ghost->pos.direction + 2) % 4;

    int choices[4];
    int n = 0;

    for (int d = 0; d < 4; d++)
        if (avail[d] && d != reverse)
            choices[n++] = d;

    if (n == 0)
        for (int d = 0; d < 4; d++)
            if (avail[d]) choices[n++] = d;

    int pick = choices[GetRandomValue(0, n - 1)];
    ghost->pos.direction = pick;
    ghost->pos.row += dr[pick];
    ghost->pos.col += dc[pick];
}

void UpdateGhost(Ghost *ghost, Map *map, Pacman *pacman, float dt) {
    (void)pacman; // atualmente não usado
    TileType t = GetTile(map, ghost->pos.row, ghost->pos.col);

    if (t == PORTAL) {
        int or, oc;
        FindOtherPortal(map, ghost->pos.row, ghost->pos.col, &or, &oc);
        if (or >= 0) {
            ghost->pos.row = or;
            ghost->pos.col = oc;
        }
    }

    ghost->moveTimer -= dt;
    if (ghost->moveTimer > 0.0f) return;

    MoveGhost(ghost, map);
    ghost->moveTimer = ghost->speed;
}
