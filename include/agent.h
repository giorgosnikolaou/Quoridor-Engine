#pragma once

#include "quoridor.h"
#include "Vector.h"

typedef struct
{
    Action* action;
    int evaluation;
} * ActionPair;


bool is_terminal_state(Board board);
ActionPair alpha_beta(Board board, uint32_t depth, uint8_t agent, int alpha, int beta, EvaluationFunction eval);
