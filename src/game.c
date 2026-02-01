#include <ncurses.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "game.h"
#include "draw.h"
#include "score.h"
#include "config.h"

// Global game state
GameState actual_state = STATE_START;
pthread_mutex_t master_mutex = PTHREAD_MUTEX_INITIALIZER;

// Game entities
Bullet bullets[NUMBER_BULLETS + 5];
Alien aliens[NUMBER_ALIENS + 5];
Frame frame_page[NUMBER_FRAMES];
Player player;
Score high_scores[MAX_HIGH_SCORES];
Score score;

// Game variables
int high_score = 0;
int count = 0;
int menu_item = 0;
int menu_game_over = 0;
int menu_pause = 0;
int ptr_type_alien = 0;
int ptr_type_bullet = 0;

void new_next_fit_alien(void) {
    int i = (ptr_type_alien + 1) % (NUMBER_ALIENS + 5);
    
    for (int j = i; j != ptr_type_alien; j = (j + 1) % (NUMBER_ALIENS + 5)) {
        if (!aliens[j].active) {
            ptr_type_alien = j;
            return;
        }
    }
}

int algorithm_lru(Alien alien) {
    int max_occupied_time = 0;
    int index = 0;
    
    for (int i = 0; i < NUMBER_FRAMES; i++) {
        if (!frame_page[i].active) {
            frame_page[i].active = 1;
            frame_page[i].last_time = 1;
            frame_page[i].alien = alien;
            return 3 * i;
        }
        if (max_occupied_time < frame_page[i].last_time) {
            max_occupied_time = frame_page[i].last_time;
            index = i;
        }
    }
    
    frame_page[index].active = 1;
    frame_page[index].last_time = 1;
    frame_page[index].alien = alien;
    return 3 * index;
}

void update_bullets(void) {
    for (int i = 0; i < NUMBER_BULLETS + 5; i++) {
        if (bullets[i].active) {
            bullets[i].y--;
            if (bullets[i].y < 3) {
                bullets[i].active = 0;
            }
        }
    }
}

void update_score(int aliens_type, int finish) {
    if (finish) {
        score.score -= 2 * aliens_type;
    } else {
        score.score += 4 * aliens_type;
    }
}

void update_aliens(void) {
    int number_aliens_active = 0;
    
    for (int i = 0; i < NUMBER_ALIENS + 5; i++) {
        if (aliens[i].active) {
            number_aliens_active += 1;
            aliens[i].y += (i % 4) + 1;
            if (aliens[i].y >= LINES - 1) {
                aliens[i].active = 0;
                update_score(aliens[i].type, 1);
            }
        }
    }
    
    for (int i = number_aliens_active; i <= NUMBER_ALIENS; i++) {
        if (rand() % 100 < 5) {
            aliens[ptr_type_alien].active = 1;
            aliens[ptr_type_alien].x = algorithm_lru(aliens[ptr_type_alien]);
            aliens[ptr_type_alien].index_frame = aliens[ptr_type_alien].x / 3;
            aliens[ptr_type_alien].y = 3;
            aliens[ptr_type_alien].type = ptr_type_alien;
            new_next_fit_alien();
        }
    }
    
    if (number_aliens_active > NUMBER_ALIENS) {
        printf("An error has occurred");
    }
}

void update_frames(void) {
    for (int i = 0; i < NUMBER_FRAMES; i++) {
        if (frame_page[i].active) {
            frame_page[i].last_time += 1;
        }
    }
}

void check_collisions(void) {
    // Check bullet-alien collisions
    for (int i = 0; i < NUMBER_BULLETS + 5; i++) {
        if (bullets[i].active) {
            for (int j = 0; j < NUMBER_ALIENS + 5; j++) {
                if (aliens[j].active) {
                    int first[6] = {aliens[j].x, aliens[j].x + 1, aliens[j].x + 2, 
                                    aliens[j].x, aliens[j].x + 1, aliens[j].x + 2};
                    int second[6] = {aliens[j].y, aliens[j].y, aliens[j].y, 
                                     aliens[j].y - 1, aliens[j].y - 1, aliens[j].y - 1};
                    
                    for (int k = 0; k < 6; k++) {
                        if (bullets[i].x == first[k] && bullets[i].y == second[k]) {
                            bullets[i].active = 0;
                            update_score(aliens[j].type, 0);
                            aliens[j].active = 0;
                            frame_page[aliens[j].index_frame].active = 0;
                            break;
                        }
                    }
                }
            }
        }
    }
    
    // Check player-alien collisions
    int first_player[9] = {player.x + 2, player.x + 1, player.x + 2, player.x + 3, 
                           player.x, player.x + 1, player.x + 2, player.x + 3, player.x + 4};
    int second_player[9] = {player.y, player.y + 1, player.y + 1, player.y + 1, 
                            player.y + 2, player.y + 2, player.y + 2, player.y + 2, player.y + 2};
    
    int occupied = 0;
    for (int i = 0; i < NUMBER_ALIENS + 5; i++) {
        if (occupied)
            break;
        
        if (aliens[i].active) {
            int first_alien[6] = {aliens[i].x, aliens[i].x + 1, aliens[i].x + 2, 
                                  aliens[i].x, aliens[i].x + 1, aliens[i].x + 2};
            int second_alien[6] = {aliens[i].y, aliens[i].y, aliens[i].y, 
                                   aliens[i].y - 1, aliens[i].y - 1, aliens[i].y - 1};
            
            for (int k = 0; k < 6; k++) {
                if (occupied)
                    break;
                for (int m = 0; m < 9; m++) {
                    if (first_alien[k] == first_player[m] && second_alien[k] == second_player[m]) {
                        aliens[i].active = 0;
                        frame_page[aliens[i].index_frame].active = 0;
                        player.lives--;
                        occupied = 1;
                        break;
                    }
                }
            }
        }
    }
}

void init_game(void) {
    player.x = COLS / 2;
    player.y = LINES - 9;
    score.score = 0;
    player.lives = 3;
    ptr_type_alien = 0;
    ptr_type_bullet = 0;
    
    // Deactivate all bullets and aliens
    for (int i = 0; i < NUMBER_BULLETS + 5; i++) {
        bullets[i].active = 0;
    }
    for (int i = 0; i < NUMBER_ALIENS + 5; i++) {
        aliens[i].active = 0;
    }
    for (int i = 0; i < NUMBER_FRAMES; i++) {
        frame_page[i].active = 0;
    }
}

void *development_game(void *arg) {
    (void)arg; // Unused parameter
    int move_enemy = 0;
    int mask = 0;
    score.name[0] = '\0';
    int free = 0;
    
    while (!EXIT) {
        lock_master_mutex;
        
        if (START) {
            mask = 0;
            draw_start_screen(&free);
            load_high_scores(HIGH_SCORES_FILE);
            high_score = high_scores[0].score;
        } else if (RUN) {
            free = 1;
            update_bullets();
            
            if (!move_enemy) {
                update_aliens();
                update_frames();
            }
            
            check_collisions();
            
            if (player.lives == 0) {
                actual_state = STATE_GAME_OVER;
                if (score.score > high_score) {
                    high_score = score.score;
                }
            }
            
            clear();
            draw_player();
            
            mvprintw(1, 2, "HP: %d", player.lives);
            mvprintw(1, COLS / 2 - 2, "Score: %d", score.score);
            mvprintw(1, COLS - 17, "High Score: %d", high_score);
            
            draw_bullets_player();
            draw_aliens();
            
            refresh();
        } else if (GAME_OVER) {
            free = 1;
            mask += 1;
            draw_game_over_screen();
            
            if (mask == 1) {
                add_new_score(score);
                save_high_scores(HIGH_SCORES_FILE);
            }
        } else if (PAUSE) {
            free = 1;
            mask += 2;
            draw_pause();
            if (mask == 2) {
                add_new_score(score);
                save_high_scores(HIGH_SCORES_FILE);
            }
        } else if (SEE_SCORES) {
            free = 1;
            draw_high_scores();
        }
        
        unlock_master_mutex;
        
        usleep(DELAY);
        move_enemy = (move_enemy + 1) % 10;
    }
    return NULL;
}
