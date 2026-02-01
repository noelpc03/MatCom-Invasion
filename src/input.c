#include <ncurses.h>
#include <ctype.h>
#include "input.h"
#include "game.h"
#include "score.h"
#include "config.h"

void move_player(int direction) {
    switch (direction) {
    case KEY_LEFT:
        if (player.x > 0) {
            player.x--;
        }
        break;
    case KEY_RIGHT:
        if (player.x < COLS - 5) {
            player.x++;
        }
        break;
    case KEY_UP:
        if (player.y > 0) {
            player.y--;
        }
        break;
    case KEY_DOWN:
        if (player.y < LINES - 3) {
            player.y++;
        }
        break;
    }
}

void next_fit_bullet(void) {
    int i = (ptr_type_bullet + 1) % (NUMBER_BULLETS + 5);
    
    for (int j = i; j != ptr_type_bullet; j = (j + 1) % (NUMBER_BULLETS + 5)) {
        if (!bullets[j].active) {
            ptr_type_bullet = j;
            return;
        }
    }
}

void shoot(void) {
    int number_bullets = 0;
    
    for (int i = 0; i < NUMBER_BULLETS + 5; i++) {
        if (bullets[i].active)
            number_bullets += 1;
    }
    if (number_bullets < NUMBER_BULLETS) {
        bullets[ptr_type_bullet].x = player.x + 2;
        bullets[ptr_type_bullet].y = player.y - 1;
        bullets[ptr_type_bullet].active = 1;
        next_fit_bullet();
    }
}

void *input_handler(void *arg) {
    (void)arg; // Unused parameter
    int index = 0;
    int ch;
    int free = 0;
    
    while (!EXIT) {
        ch = getch();
        lock_master_mutex;
        
        if (START) {
            if (free) {
                index = 0;
                free = 0;
            }
            if (ch == KEY_DOWN)
                menu_item = (menu_item + 1) % 4;
            else if (ch == KEY_UP)
                menu_item = ((menu_item - 1) + 4) % 4;
            else if (ch == ' ') {
                switch (menu_item) {
                case 0:
                    actual_state = STATE_RUN;
                    init_game();
                    break;
                case 1:
                    actual_state = STATE_RUN;
                    init_game();
                    load_game(SAVED_GAME_FILE);
                    break;
                case 2:
                    actual_state = STATE_SEE_SCORES;
                    break;
                case 3:
                    actual_state = STATE_OUT;
                    break;
                }
            } else if (isprint(ch)) {
                score.name[index++] = ch;
            }
        } else if (RUN) {
            free = 1;
            switch (ch) {
            case 'a':
            case KEY_LEFT:
                move_player(KEY_LEFT);
                break;
            case 'd':
            case KEY_RIGHT:
                move_player(KEY_RIGHT);
                break;
            case 'w':
            case KEY_UP:
                move_player(KEY_UP);
                break;
            case 's':
            case KEY_DOWN:
                move_player(KEY_DOWN);
                break;
            case ' ':
                shoot();
                break;
            case 'q':
                actual_state = STATE_PAUSE;
                break;
            }
        } else if (GAME_OVER) {
            free = 1;
            if (ch == KEY_DOWN)
                menu_game_over = (menu_game_over + 1) % 2;
            else if (ch == KEY_UP)
                menu_game_over = ((menu_game_over - 1) + 2) % 2;
            else if (ch == ' ') {
                switch (menu_game_over) {
                case 0:
                    actual_state = STATE_START;
                    break;
                case 1:
                    actual_state = STATE_OUT;
                    break;
                }
            }
        } else if (PAUSE) {
            if (ch == KEY_DOWN)
                menu_pause = (menu_pause + 1) % 5;
            else if (ch == KEY_UP)
                menu_pause = ((menu_pause - 1) + 5) % 5;
            else if (ch == ' ') {
                switch (menu_pause) {
                case 0:
                    actual_state = STATE_START;
                    break;
                case 1:
                    actual_state = STATE_OUT;
                    break;
                case 2:
                    save_game(SAVED_GAME_FILE);
                    actual_state = STATE_OUT;
                    break;
                case 3:
                    save_game(SAVED_GAME_FILE);
                    actual_state = STATE_START;
                    break;
                case 4:
                    actual_state = STATE_RUN;
                    break;
                }
            }
        } else if (SEE_SCORES) {
            free = 1;
            if (ch == 's') {
                actual_state = STATE_START;
            }
        }
        
        unlock_master_mutex;
    }
    return NULL;
}
