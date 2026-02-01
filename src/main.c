#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include "game.h"
#include "input.h"
#include "config.h"

int main(void) {
    srand(time(NULL));
    initscr();
    noecho();
    curs_set(FALSE);
    keypad(stdscr, TRUE);
    timeout(0);
    start_color();
    
    init_color(COLOR_GOLD, 700, 550, 0);      // Gold
    init_color(COLOR_SILVER, 800, 800, 800);  // Silver
    init_color(COLOR_BRONZE, 600, 400, 200);  // Bronze
    
    init_pair(1, COLOR_BLUE, COLOR_BLACK);
    init_pair(2, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(3, COLOR_GREEN, COLOR_BLACK);
    init_pair(4, COLOR_RED, COLOR_BLACK);
    init_pair(5, COLOR_YELLOW, COLOR_BLACK);
    init_pair(6, COLOR_GOLD, COLOR_BLACK);
    init_pair(7, COLOR_SILVER, COLOR_BLACK);
    init_pair(8, COLOR_BRONZE, COLOR_BLACK);
    
    pthread_t game_thread, input_thread;
    
    pthread_create(&game_thread, NULL, development_game, NULL);
    pthread_create(&input_thread, NULL, input_handler, NULL);
    
    pthread_join(game_thread, NULL);
    pthread_join(input_thread, NULL);
    
    endwin();
    pthread_mutex_destroy(&master_mutex);
    
    return 0;
}
