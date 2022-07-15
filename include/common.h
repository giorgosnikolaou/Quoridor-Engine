#pragma once

#include <inttypes.h>
#include <stdlib.h>
#include "Vector.h"

#define INFINITY 999999
#define BILLION 1E9

typedef uint8_t** Cells;

typedef enum
{
    PLACE,
    MOVE
} action_id;

typedef struct position
{
    uint32_t i;
    uint32_t j;
} Position;


typedef struct board
{
    uint32_t dimension;
    size_t max_lettering;
    char** letter_numbering;

    uint32_t enemy_walls;
    uint32_t player_walls;

    uint32_t walls;

    Position player;
    Position enemy;

    Cells nodes;
    Cells hor_walls;
    Cells ver_walls;

    // Vector of actions
    Vector history;

} * Board;


typedef struct
{
    uint32_t new_i;
    uint32_t new_j;
    uint32_t old_i;
    uint32_t old_j;
} action_move;

typedef struct
{
    uint32_t x;
    uint32_t y;
    uint8_t orientation;
    uint8_t dist_from_enemy;
} action_wall;

typedef union
{
    action_wall wall;
    action_move move;
} action_info;

typedef struct action
{
    char agent;
    action_id action;
    
    action_info info;

} Action ;


typedef int (*EvaluationFunction)(Board board);

typedef enum
{
    NAME,
    KNOWN_COMMANDS,
    LIST_COMMANDS,
    QUIT,
    BOARDSIZE,
    CLEAR_BOARD,
    WALLS,
    PLAYMOVE,
    PLAYWALL,
    GENMOVE,
    UNDO,
    WINNER,
    SHOWBOARD,
} CommandType;

typedef struct
{
    CommandType type;
    Vector arguments;
} * Command;



typedef struct
{
    Board board;
    char* name;
} * Engine;



