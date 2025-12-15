#ifndef ENTITIES_H
#define ENTITIES_H

#include "raylib.h"
#include "map.h"
#include <stdbool.h>

// Direções possíveis (usadas em Position.direction / nextDirection) 
typedef enum {
    DIR_RIGHT = 0,
    DIR_DOWN  = 1,
    DIR_LEFT  = 2,
    DIR_UP    = 3
} Direction;

typedef struct {
    int row;
    int col;
    Direction direction;      // direção atual 
    Direction nextDirection;  // direção desejada (input do jogador) 
    bool moving;
} Position;

// Eventos que o Pacman pode gerar ao mover-se 
typedef enum {
    PACMAN_EVENT_NONE,
    PACMAN_EVENT_PELLET,
    PACMAN_EVENT_POWER_PELLET
} PacmanEvent;

// Estrutura do Pacman 
typedef struct {
    Position pos;
    int lives;
    bool alive;
    int score;

    // Controle de movimentação por tempo:
    // moveTimer decrementa com dt; quando <= 0, Pacman dá um passo e moveTimer = moveDelay.
    // moveDelay é o intervalo (em segundos) entre passos — maior = mais lento.
    float moveTimer;
    float moveDelay;
} Pacman;

// Modos do fantasma 
typedef enum {
    SCATTER,
    CHASE,
    FRIGHTENED
} GhostMode;

// Estrutura do fantasma
// 'speed' aqui é como o tempo entre passos (segundos). 
// Valores menores -> movimentos mais rápidos. 
typedef struct {
    Position pos;
    GhostMode mode;
    Color color;
    bool alive;

    float speed;           // intervalo entre passos (segundos) 
    float frightenedTimer;  // tempo restante em estado assustado (segundos) 
    float moveTimer;       // timer interno para controlar passos 
} Ghost;

// Funções do Pacman 
void InitPacman(Pacman *pacman, int startRow, int startCol);
PacmanEvent UpdatePacman(Pacman *pacman, Map *map, float dt);
PacmanEvent MovePacman(Pacman *pacman, Map *map);

// Funções dos fantasmas 
void InitGhost(Ghost *ghost, int startRow, int startCol, Color color);
void UpdateGhost(Ghost *ghost, Map *map ,Pacman *pacman, float dt);
void MoveGhost(Ghost *ghost, Map *map);

#endif 
