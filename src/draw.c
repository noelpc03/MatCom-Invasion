#include <ncurses.h>
#include <string.h>
#include <stdio.h>
#include "draw.h"
#include "game.h"
#include "config.h"

const char *menu_logo[6] = {
    "8888ba.88ba             dP                                   dP                                   oo                   ",
    "88   8b   8b            88                                   88                                                          ",
    "88   88   88 .d8888b. d8888P .d8888b. .d8888b. 88d8b.d8b.    88 88d888b. dP  dP .d8888b. .d8888b. dP .d8888b. 88d888b. ",
    "88   88   88 88    88   88   88       88    88 88  88  88    88 88    88 88  d8 88    88 Y8ooooo. 88 88    88 88    88 ",
    "88   88   88 88    88   88   88       88    88 88  88  88    88 88    88 88  88 88    88       88 88 88    88 88    88 ",
    "dP   dP   dP .888888.   dP   .88888P. .d8888b. dP  dP  dP    dP dP    dP 8888P. .88888P8 .88888P. dP .888888. dP    dP "
};

const char *menu_logo_game_over[6] = {
    ".88888.                                   .88888.      ",
    "d8'   `88                                 d8'   `8b                     ",
    "88        .d8888b. 88d8b.d8b. .d8888b.    88     88 dP   .dP .d8888b. 88d888b. ",
    "88   YP88 88'  `88 88'`88'`88 88ooood8    88     88 88   d8' 88ooood8 88'  `88 ",
    "Y8.   .88 88.  .88 88  88  88 88.  ...    Y8.   .8P 88 .88'  88.  ... 88       ",
    "`88888'  `88888P8 dP  dP  dP `88888P'     `8888P'  8888P'   `88888P' dP       "
};

const char *menu_logo_pause[6] = {
    "888888ba    ",
    "88    `8b      ",
    "a88aaaa8P' .d8888b. dP    dP .d8888b. .d8888b. ",
    "88        88'  `88 88    88 Y8ooooo. 88ooood8 ",
    "88        88.  .88 88.  .88       88 88.  ... ",
    "dP        `88888P8 `88888P' `88888P' `88888P' "
};

const char *item_start_new_game[2] = {
    "> START NEW GAME <",
    "start new game"
};

const char *item_start_saved_game[2] = {
    "> START SAVED GAME <",
    "start saved game"
};

const char *item_best_scores[2] = {
    "> BEST SCORES <",
    "best scores"
};

const char *item_exit[2] = {
    "> EXIT <",
    "exit"
};

int str_len(const char *str) {
    int size = 0;
    while (*str++)
        ++size;
    return size;
}

int logo_h_size = sizeof(menu_logo) / sizeof(menu_logo[0]);
int logo_w_size = 1;

int get_logo_w_size(void) {
    int max_width = 1;
    for (int i = 0; i < logo_h_size; i++) {
        int len = str_len(menu_logo[i]);
        if (len > max_width) {
            max_width = len;
        }
    }
    return max_width;
}

void draw_logo(int h, int w) {
    (void)h; // Unused parameter
    if (logo_w_size == 1) {
        logo_w_size = get_logo_w_size() / 2;
    }
    
    attron(COLOR_PAIR(1));
    for (int i = 0; i < 6; i++) {
        mvprintw(5 + i, w / 2 - logo_w_size, "%s", menu_logo[i]);
    }
    attroff(COLOR_PAIR(1));
}

void draw_bullets_player(void) {
    for (int i = 0; i < NUMBER_BULLETS + 5; i++) {
        if (bullets[i].active) {
            attron(COLOR_PAIR(3));
            mvprintw(bullets[i].y, bullets[i].x, "|");
            attroff(COLOR_PAIR(3));
        }
    }
}

void draw_aliens(void) {
    for (int i = 0; i < NUMBER_ALIENS + 5; i++) {
        if (aliens[i].active) {
            attron(COLOR_PAIR(4));
            mvprintw(aliens[i].y - 1, aliens[i].x, "{@}");
            mvprintw(aliens[i].y, aliens[i].x, "/\"\\");
            attroff(COLOR_PAIR(4));
        }
    }
}

void draw_player(void) {
    attron(COLOR_PAIR(1));
    mvprintw(player.y, player.x, "  ^  ");
    mvprintw(player.y + 1, player.x, " mAm ");
    mvprintw(player.y + 2, player.x, "mAmAm");
    attroff(COLOR_PAIR(1));
}

void auxiliar_draw(int v, const char *item[2], int index) {
    if (!v) {
        attron(COLOR_PAIR(3));
        mvprintw(LINES / 2 + index, COLS / 2 - 20, "%s", item[v]);
        attroff(COLOR_PAIR(3));
    } else {
        mvprintw(LINES / 2 + index, COLS / 2 - 20, "%s", item[v]);
    }
}

void draw_start_screen(int *free) {
    clear();
    
    if (*free) {
        snprintf(score.name, sizeof(score.name), "                   ");
        *free = 0;
    }
    
    mvprintw(LINES / 2, COLS / 2 - 20, "Enter your name and press Space : %s", score.name);
    mvprintw(LINES / 2 + 2, COLS / 2 - 20, "High Score: %d", high_score);
    
    attron(COLOR_PAIR(1));
    for (int i = 0; i < 6; i++) {
        mvprintw(LINES / 2 - 9 + i, COLS / 2 - 55, "%s", menu_logo[i]);
    }
    attroff(COLOR_PAIR(1));
    
    int v = (menu_item == 0) ? 0 : 1;
    auxiliar_draw(v, item_start_new_game, 4);
    
    v = (menu_item == 1) ? 0 : 1;
    auxiliar_draw(v, item_start_saved_game, 6);
    
    v = (menu_item == 2) ? 0 : 1;
    auxiliar_draw(v, item_best_scores, 8);
    
    v = (menu_item == 3) ? 0 : 1;
    auxiliar_draw(v, item_exit, 10);
    
    refresh();
}

void draw_game_over_screen(void) {
    clear();
    attron(COLOR_PAIR(4));
    
    for (int i = 0; i < 6; i++) {
        mvprintw(LINES / 2 - 9 + i, COLS / 2 - 55, "%s", menu_logo_game_over[i]);
    }
    
    attroff(COLOR_PAIR(4));
    
    const char *item_return_start[2] = {
        "> RETURN START <",
        "return start"
    };
    const char *item_quit[2] = {
        "> Quit <",
        "quit"
    };
    
    int v = (menu_game_over == 0) ? 0 : 1;
    auxiliar_draw(v, item_return_start, 0);
    
    v = (menu_game_over == 1) ? 0 : 1;
    auxiliar_draw(v, item_quit, 2);
    
    mvprintw(LINES / 2 + 4, COLS / 2 - 20, "Score: %d", score.score);
    mvprintw(LINES / 2 + 6, COLS / 2 - 20, "High Score: %d", high_score);
    refresh();
}

void draw_pause(void) {
    clear();
    attron(COLOR_PAIR(5));
    for (int i = 0; i < 6; i++) {
        mvprintw(LINES / 2 - 9 + i, COLS / 2 - 30, "%s", menu_logo_pause[i]);
    }
    attroff(COLOR_PAIR(5));
    
    const char *item_return_start[2] = {
        "> RETURN START <",
        "return start"
    };
    const char *item_quit[2] = {
        "> Quit <",
        "quit"
    };
    const char *item_save_quit[2] = {
        "> SAVE AND QUIT<",
        "save and quit"
    };
    const char *item_save_ret[2] = {
        "> SAVE AND RETURN<",
        "save and return"
    };
    const char *item_continue[2] = {
        "> CONTNUE<",
        "continue"
    };
    
    int v = (menu_pause == 0) ? 0 : 1;
    auxiliar_draw(v, item_return_start, 0);
    
    v = (menu_pause == 1) ? 0 : 1;
    auxiliar_draw(v, item_quit, 2);
    
    v = (menu_pause == 2) ? 0 : 1;
    auxiliar_draw(v, item_save_quit, 4);
    
    v = (menu_pause == 3) ? 0 : 1;
    auxiliar_draw(v, item_save_ret, 6);
    
    v = (menu_pause == 4) ? 0 : 1;
    auxiliar_draw(v, item_continue, 8);
    
    mvprintw(LINES / 2 + 10, COLS / 2 - 20, "Score: %d", score.score);
    mvprintw(LINES / 2 + 12, COLS / 2 - 20, "High Score: %d", high_score);
    refresh();
}

void draw_load_error(void) {
    clear();
    attron(COLOR_PAIR(4));
    mvprintw(LINES / 2 - 2, COLS / 2 - 10, "Error");
    attroff(COLOR_PAIR(4));
    mvprintw(LINES / 2 - 1, COLS / 2 - 10, "Error trying to load game. It seems there are no saved files.");
    mvprintw(LINES / 2 + 1, COLS / 2 - 10, "Press 's' to Return to Start Screen");
    mvprintw(LINES / 2 + 2, COLS / 2 - 10, "Press 'q' to Quit");
    refresh();
}

void draw_saving_error(void) {
    clear();
    attron(COLOR_PAIR(4));
    mvprintw(LINES / 2 - 2, COLS / 2 - 10, "Error");
    attroff(COLOR_PAIR(4));
    mvprintw(LINES / 2 - 1, COLS / 2 - 10, "Error trying to save game. It was not possible to find the usb.");
    mvprintw(LINES / 2 + 1, COLS / 2 - 10, "Press 's' to Return to Start Screen");
    mvprintw(LINES / 2 + 2, COLS / 2 - 10, "Press 'q' to Quit");
    mvprintw(LINES / 2 + 3, COLS / 2 - 10, "Press 'u' to go back to Exit Menu");
    refresh();
}

void draw_select_screen(void) {
    clear();
    attron(COLOR_PAIR(1));
    mvprintw(LINES / 2 - 2, COLS / 2 - 10, "Select Game");
    attroff(COLOR_PAIR(1));
    mvprintw(LINES / 2, COLS / 2 - 10, "Press 'p' to Load Saved Game");
    mvprintw(LINES / 2 + 1, COLS / 2 - 10, "Press 'u' to Load Game from USB");
    mvprintw(LINES / 2 + 2, COLS / 2 - 10, "Press 's' to Return to Start Screen");
    mvprintw(LINES / 2 + 3, COLS / 2 - 10, "Press 'q' to Quit");
    mvprintw(LINES / 2 + 4, COLS / 2 - 10, "High Score: %d", high_score);
    refresh();
}

void draw_high_scores(void) {
    clear();
    mvprintw(LINES / 2 - 7, COLS / 2 - 10, "High Scores");
    
    for (int i = 0; i < MAX_HIGH_SCORES; i++) {
        if (high_scores[i].score < 0) {
            mvprintw(LINES / 2 + i - 4, COLS / 2 - 10, "%d. ---", i + 1);
        } else {
            if (i < 3) {
                attron(COLOR_PAIR(6));  // Gold
            } else if (i < 6) {
                attron(COLOR_PAIR(7));  // Silver
            } else {
                attron(COLOR_PAIR(8));  // Bronze
            }
            mvprintw(LINES / 2 + i - 4, COLS / 2 - 10, "%d. %d", i + 1, high_scores[i].score);
            mvprintw(LINES / 2 + i - 4, COLS / 2 + 2, "%s ", high_scores[i].name);
            mvprintw(LINES / 2 + i - 4, COLS / 2 + 12, "%s ", high_scores[i].date);
            attroff(COLOR_PAIR(8));
            attroff(COLOR_PAIR(7));
            attroff(COLOR_PAIR(6));
        }
    }
    
    mvprintw(LINES - 4, COLS / 2 - 10, "Press 's' to return to Start Menu");
    refresh();
}
