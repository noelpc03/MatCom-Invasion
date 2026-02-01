#ifndef SCORE_H
#define SCORE_H

#include "types.h"
#include "game.h"

// High score management
void add_new_score(Score new_score);
void save_high_scores(const char *filename);
void load_high_scores(const char *filename);

// Saved game management
void save_game(const char *filename);
void load_game(const char *filename);

#endif // SCORE_H
