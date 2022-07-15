#include "quoridor.h"
#include "Vector.h"
#include "agent.h"
#include "moves.h"
#include <stdio.h>
#include <time.h>


bool is_terminal_state(Board board)
{
    return (board->player.i == 0) || (board->enemy.i == board->dimension - 1);
}

static ActionPair create_pair(int evaluation)
{
    ActionPair to_return = malloc(sizeof(*to_return));
    to_return->action = malloc(sizeof(*to_return->action));
    to_return->evaluation = evaluation;
    return to_return;
}

static void free_pair(ActionPair pair)
{
    free(pair->action);
    free(pair);
}

extern char maximizing;
extern char minimizing;
extern double time_left;
extern struct timespec requestStart;
size_t prunes = 0;
ActionPair alpha_beta(Board board, uint32_t depth, uint8_t agent, int alpha, int beta, EvaluationFunction eval)
{
    // agent -> 1 max, 0 min
    struct timespec requestEnd;
    clock_gettime(CLOCK_REALTIME, &requestEnd);
    // Calculate time it took
    double accum = ( requestEnd.tv_sec - requestStart.tv_sec ) + ( requestEnd.tv_nsec - requestStart.tv_nsec ) / BILLION;

    // Time's up, return NULL 
    if (time_left <= accum)
        return NULL;

    // Reached depth limit
    if (depth == 0 || is_terminal_state(board))
        return create_pair(eval(board) + depth);

    Vector actions = get_legal_actions(board, agent ? maximizing : minimizing);

    uint8_t next_agent = !agent;

    ActionPair pair = create_pair(agent ? -INFINITY - 1 : INFINITY + 1);
    
    for (size_t i = 0, size = vector_size(actions); i < size; i++)
    {
        if ((agent == 1 && pair->evaluation >= beta) || (agent == 0 && pair->evaluation <= alpha))
        {
            prunes++;
            break;
        }

        Action* action = vector_get_at(actions, i);
        play_action(board, *action);

        ActionPair to_return = alpha_beta(board, depth - 1, next_agent, alpha, beta, eval);

        undo_action(board, *action);

        // Time's up, return NULL 
        if (to_return == NULL)
        {
            vector_destroy(actions);
            return NULL;
        }

        *to_return->action = *action;

        if (agent == 1 && pair->evaluation < to_return->evaluation)
        {
            free_pair(pair);
            pair = to_return;
            if (alpha < pair->evaluation)
                alpha = pair->evaluation;
        }
        else if (agent == 0 && pair->evaluation > to_return->evaluation)
        {
            free_pair(pair);
            pair = to_return;
            if (beta > pair->evaluation)
                beta = pair->evaluation;
        }
        else
            free_pair(to_return);
    }

    vector_destroy(actions);

    return pair;

}

