#ifndef DRAW_H
#define DRAW_H

#include "types.h"

// Logo and menu strings
extern const char *menu_logo[6];
extern const char *menu_logo_game_over[6];
extern const char *menu_logo_pause[6];
extern const char *item_start_new_game[2];
extern const char *item_start_saved_game[2];
extern const char *item_best_scores[2];
extern const char *item_exit[2];

// Drawing functions
void draw_logo(int h, int w);
void draw_bullets_player(void);
void draw_aliens(void);
void draw_player(void);
void draw_start_screen(int *free);
void draw_game_over_screen(void);
void draw_pause(void);
void draw_load_error(void);
void draw_saving_error(void);
void draw_select_screen(void);
void draw_high_scores(void);

// Helper functions
int str_len(const char *str);
int get_logo_w_size(void);
void auxiliar_draw(int v, const char *item[2], int index);

#endif // DRAW_H
