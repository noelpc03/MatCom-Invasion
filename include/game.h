#ifndef GAME_H
#define GAME_H

#include "types.h"
#include "config.h"
#include <pthread.h>

// Global game state
extern GameState actual_state;
extern pthread_mutex_t master_mutex;

// Mutex macros
#define lock_master_mutex pthread_mutex_lock(&master_mutex)
#define unlock_master_mutex pthread_mutex_unlock(&master_mutex)

// Global game variables
extern Bullet bullets[NUMBER_BULLETS + 5];
extern Alien aliens[NUMBER_ALIENS + 5];
extern Frame frame_page[NUMBER_FRAMES];
extern Player player;
extern Score high_scores[MAX_HIGH_SCORES];
extern Score score;
extern int high_score;
extern int count;

// Menu state
extern int menu_item;
extern int menu_game_over;
extern int menu_pause;

// Pointers for resource allocation
extern int ptr_type_alien;
extern int ptr_type_bullet;

// Game initialization and main loop
void init_game(void);
void *development_game(void *arg);

// Update functions
void update_bullets(void);
void update_aliens(void);
void update_frames(void);
void update_score(int aliens_type, int finish);
void check_collisions(void);

// Alien management
void new_next_fit_alien(void);
int algorithm_lru(Alien alien);

#endif // GAME_H
