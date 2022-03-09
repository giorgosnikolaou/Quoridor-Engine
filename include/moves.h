#pragma once

#include "common.h"
#include "Vector.h"

bool audit_place_wall(Board board, uint32_t i, uint32_t j, uint8_t vertical, char agent);

void play_action(Board board, Action action);
void undo_action(Board board, Action action);

Vector get_legal_actions(Board board, char agent);

void get_distance(Board board, uint8_t* b, uint8_t* w);






