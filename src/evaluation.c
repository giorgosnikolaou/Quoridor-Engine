#include <stdlib.h>
#include "evaluation.h"

#include "moves.h"

static uint32_t abs_value(int value)
{
    if (value < 0)
        return -value;
    
    return value;
}

extern char maximizing;
size_t evaluations = 0;
int evaluate(Board board)
{
    // The better the state for the maximizing player the larger the evaluation
    // Evaluation must be in [-INFINITY, INFINITY]
    evaluations++;

    if (board->player.i == 0)
        return (maximizing == 'W') ? INFINITY : -INFINITY;
    else if (board->enemy.i == board->dimension - 1)
        return (maximizing == 'W') ? -INFINITY : INFINITY;

    uint32_t sum = 0;

    uint8_t b;
    uint8_t w;

    get_distance(board, &b, &w);

    if (maximizing == 'W')
    {
        sum -= w;
        sum += b;

        // If enemy very close to goal give a little penalty
        if (board->enemy.i + 3 >= board->dimension)
            sum -= (board->enemy.i + 3 - board->dimension);

        // Having much less walls than the enemy is a very disadvantageous position
        if (board->enemy_walls > board->player_walls)
            sum -= (board->enemy_walls - board->player_walls) >> 1;

    }
    else
    {
        sum -= b;
        sum += w;

        if (board->player.i <= 3)
            sum -= board->player.i;
            
        if (board->player_walls > board->enemy_walls)
            sum -= (board->player_walls - board->enemy_walls) >> 1;
    }

    // if (board->player_walls + board->enemy_walls <= board->walls)
    //     sum -= (abs_value(board->player.i - board->enemy.i) + abs_value(board->player.j - board->enemy.j)) >> 2;

    return sum;
}

// Trivial evaluation function
int tevaluate(Board board)
{
    evaluations++;

    if (board->player.i == 0)
        return (maximizing == 'W') ? INFINITY : -INFINITY;
    else if (board->enemy.i == board->dimension - 1)
        return (maximizing == 'W') ? -INFINITY : INFINITY;
    
    return 1;
}
