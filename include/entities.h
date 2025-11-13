#ifndef ENTITIES_H
#define ENTITIES_H

#include "raylib.h"

typedef struct {
    int row;
    int col;
    Vector2 position;
    int direction; // 0: direita, 1:baixo, 2: Esquerda, 3: Cima
    int nextDirection;
    bool moving;

}Position;

typedef struct {
    Position pos;
    int lives;
    bool alive;

}Pacman;

typedef enum {
    SCATTER,
    CHASE,
    FRIGHTENED

}GhostMode;

typedef struct {
    Posisiton pos;
    GhostMoode mode;
    Color color;
    bool alive;
    float speed;
    float frightenedTimer;

}Ghost;

//Funções dos personagens

void InitPacman(Pacman *pacman, intstarRow, int startCol);
void UpdatePacman(Pacman *pacman, Map *map);
void MovePacman(Pacman *pacman, Map *map);

void InitGhost(Ghost *ghost, int starRow, int startCol, Color color);
void UpdateGhost(Ghost *ghost, Map *map, Pacman *pacman);
void MoveGhost(Ghost *ghost, Map *map);

#endif