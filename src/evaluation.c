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
        // Take into account enemy distance to goal
        sum -= w;
        sum += b;

        // If enemy very close to goal give a little penalty
        if (b <= 3)
            sum -= (4 - b);

        // Having much less walls than the enemy is a very disadvantageous position
        if (board->enemy_walls > board->player_walls)
            sum -= (board->enemy_walls - board->player_walls) >> 1;

    }
    else
    {
        sum -= b;
        sum += w;

        if (w <= 3)
            sum -= (4 - w);
            
        if (board->player_walls > board->enemy_walls)
            sum -= (board->player_walls - board->enemy_walls) >> 1;
    }


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
