#ifndef ENTITIES_H
#define ENTITIES_H

#include "raylib.h"
#include "map.h"
#include <stdbool.h>

typedef enum {
    DIR_RIGHT = 0,
    DIR_DOWN = 1,
    DIR_LEFT = 2,
    DIR_UP = 3

} Direction;  // Direções possíveis

typedef struct {
    int row;
    int col;
    Direction direction;
    Direction nextDirection;  // Próxima direção desejada
    bool moving;
} Position;

typedef struct {
    Position pos;
    int lives;
    bool alive;
    int score;
} Pacman;

typedef enum {
    SCATTER,
    CHASE,
    FRIGHTENED,

} GhostMode;

typedef struct {
    Position pos;
    GhostMode mode;
    Color color; // Cores dos fantasmas
    bool alive;
    float speed; // Velocidade do fantasma
    float frightenedTimer; // Tempo do modo assustado
} Ghost;

// Funções do Pacman
void InitPacman(Pacman *pacman, int startRow, int startCol);
void UpdatePacman(Pacman *pacman, Map *map);
void MovePacman(Pacman *pacman, Map *map);

// Funções dos fantasmas
void InitGhost(Ghost *ghost, int startRow, int startCol, Color color);
void UpdateGhost(Ghost *ghost, Map *map, Pacman *pacman);
void MoveGhost(Ghost *ghost, Map *map);

#endif

