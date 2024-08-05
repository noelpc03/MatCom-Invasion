#include <ncurses.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string.h>

#define DELAY 30000
pthread_mutex_t master_mutex = PTHREAD_MUTEX_INITIALIZER;

#define lock_master_mutex pthread_mutex_lock(&master_mutex)
#define unlock_master_mutex pthread_mutex_unlock(&master_mutex)

int w, h;

typedef enum
{
    start,
    run,
    game_over,
    out,
    info
} state;

state actual_state = start;

#define EXIT (actual_state == out)
#define START (actual_state == start)
#define RUN (actual_state == run)
#define GAME_OVER (actual_state == game_over)

typedef struct
{
    int x, y, active;
} Bullets;

typedef struct
{
    int x, y, active, direction, speed;
} Alien;

typedef struct
{
    int x, y, lives;
} Player;

// typedef struct{
//     int score;
//     Player player;
// }Game;
#define NUMBER_BULLETS 100
#define NUMBER_ALIENS 100
Bullets bullets[NUMBER_BULLETS];
Alien aliens[NUMBER_ALIENS];
Player player;
// Game game;
int score;
int high_score = 0;
int ch = 0;
#define vk_enter 10

// Funciones de Dibujo

const char *menu_logo[6] = {

    "8888ba.88ba             dP                                   dP                                     oo                   ",
    "88  `8b  `8b            88                                   88                                                          ",
    "88   88   88 .d8888b. d8888P .d8888b. .d8888b. 88d8b.d8b.    88 88d888b. dP   .dP .d8888b. .d8888b. dP .d8888b. 88d888b. ",
    "88   88   88 88'  `88   88   88'  `"
    " 88'  `88 88'`88'`88    88 88'  `88 88   d8' 88'  `88 Y8ooooo. 88 88'  `88 88'  `88 ",
    "88   88   88 88.  .88   88   88.  ... 88.  .88 88  88  88    88 88    88 88 .88'  88.  .88       88 88 88.  .88 88    88 ",
    "dP   dP   dP `88888P8   dP   `88888P' `88888P' dP  dP  dP    dP dP    dP 8888P'   `88888P8 `88888P' dP `88888P' dP    dP "

};

const char *item_start_game[2] = {
    "> START GAME <",
    "start game",
};

// Item info
const char *item_info[2] = {
    "> INFO <",
    "info",
};

// Item exit
const char *item_exit[2] = {
    "> EXIT <",
    "exit",
};

int str_len(const char *str)
{
    int size = 0;
    while (*str++)
        ++size;
    return size;
}

int logo_h_size = sizeof(menu_logo) / sizeof(menu_logo[0]);

// // Get logo len
int get_logo_w_size(void)
{
    int logo_w_size = 1;

    for (int i = 0; i < logo_h_size; i++)
    {
        int len = str_len(menu_logo[i]);
        if (len > logo_w_size)
        {
            logo_w_size = str_len(menu_logo[i]);
        }
    }
    return logo_w_size;
}

// Draw the logo

int logo_w_size = 1;
void draw_logo(int h, int w)
{
    // Get w size
    if (logo_w_size == 1)
    {
        logo_w_size = get_logo_w_size() / 2;
    }

    // Draw
    attron(COLOR_PAIR(1));
    for (int i = 0; i < 6; i++)
    {
        mvprintw(5 + i /* Logo Y pos */, w / 2 - logo_w_size, menu_logo[i]);
    }
    attroff(COLOR_PAIR(1));
}

void draw_game_over()
{
    clear();
    attron(COLOR_PAIR(4));
    mvprintw(LINES / 2 - 2, COLS / 2 - 10, "Game Over");
    attroff(COLOR_PAIR(4));
    mvprintw(LINES / 2, COLS / 2 - 10, "Press 's' to Return to Start Screen");
    mvprintw(LINES / 2 + 1, COLS / 2 - 10, "Press 'q' to Quit");
    mvprintw(LINES / 2 + 2, COLS / 2 - 10, "Score: %d", score);
    mvprintw(LINES / 2 + 3, COLS / 2 - 10, "High Score: %d", high_score);
    refresh();
}

void draw_ship(int x, int y)
{
    mvprintw(y, x, "  ^  ");
    mvprintw(y + 1, x, " mAm ");
    mvprintw(y + 2, x, "mAmAm");
}

void draw_player()
{
    attron(COLOR_PAIR(1));
    draw_ship(player.x, player.y);
    attroff(COLOR_PAIR(1));

    refresh();
    getch();

    endwin();
}

void draw_bullets()
{
    for (int i = 0; i < NUMBER_BULLETS; i++)
    {
        if (bullets[i].active)
        {
            attron(COLOR_PAIR(3));
            mvprintw(bullets[i].y, bullets[i].x, "|");
            attroff(COLOR_PAIR(3));
        }
    }
}

void draw_alien(int x, int y)
{
    mvprintw(y - 1, x, "{@}");
    mvprintw(y, x, "/\"\\");
}

void draw_aliens()
{
    for (int i = 0; i < NUMBER_ALIENS; i++)
    {
        if (aliens[i].active)
        {
            attron(COLOR_PAIR(4));
            draw_alien(aliens[i].x, aliens[i].y);
            attroff(COLOR_PAIR(4));
        }
    }
}

// FUNCIONES DE ACTUALIZACION

void update_bullets()
{

    for (int i = 0; i < NUMBER_BULLETS; i++)
    {
        if (bullets[i].active)
        {
            bullets[i].y--;

            if (bullets[i].y == 0)
            {
                bullets[i].active = 0;
            }
        }
    }
}

void update_aliens() // ARREGLAR!!!
{
    for (int i = 0; i < NUMBER_ALIENS; i++)
    {
        if (aliens[i].active)
        {
            aliens[i].x += aliens[i].direction;
            if ((aliens[i].x == COLS - 3 && aliens[i].direction == 1) || (aliens[i].x == 0 && aliens[i].direction == -1))
            {
                aliens[i].direction *= -1;
                aliens[i].y++;
                aliens[i].y++;
            }
        }
        else
        {
            bool free = true;
            for (int k = 0; k < NUMBER_ALIENS; k++)
            {
                if (aliens[k].y == 3 && aliens[k].x == 0 || aliens[k].y == 4 && aliens[k].x == 0 || aliens[k].y == 5 && aliens[k].x == 0 || aliens[k].y == 6 && aliens[k].x == 0)
                {
                    free = false;
                }
            }
            if (free)
            {
                aliens[i].y = 3;
                aliens[i].x = 0;
                aliens[i].active = 1;
                aliens[i].direction = 1;
            }
            free = true;
        }
    }
}

void collision_player()
{
    for (int i = 0; i < NUMBER_ALIENS; i++)
    {
        if ((aliens[i].x == player.x && aliens[i].y == player.y && aliens[i].active) || (aliens[i].y == LINES - 1))
        {
            player.lives--;
            aliens[i].active = 0;
        }
    }
}
void collision_bullets()
{

    for (int i = 0; i < NUMBER_BULLETS; i++)
    {
        if (!bullets[i].active)
            continue;

        for (int j = 0; j < NUMBER_ALIENS; j++)
        {
            if (aliens[j].active)
            {
                for (int x = aliens[j].x; x < aliens[j].x + 3; x++)
                {
                    for (int y = aliens[j].y - 1; y < aliens[j].y + 2; y++)
                    {
                        if (bullets[i].x == x && bullets[i].y == y)
                        {
                            aliens[j].active = 0;
                            bullets[i].active = 0;
                            score += 73;
                            break;
                        }
                    }
                }
            }
        }
    }
}

void check_collisions()
{

    collision_bullets();
    collision_player();
}

void *development_game(void *args)
{

    int speed = 0;

    while (!EXIT)
    {

        lock_master_mutex;

        if (START)
        {
            // init_game();
            //  draw_logo(h, w);
            //  dibujar la pantalla de inicio
        }
        else if (RUN)
        {
            // el juego
            update_bullets();
            if (!speed)
            {
                update_aliens();
            }

            check_collisions();
            if (score > high_score)
            {
                high_score = score;
            }

            if (player.lives <= 0)
            {
                actual_state = game_over;
            }
            clear();
            draw_player();
            draw_bullets();
            draw_aliens();

            mvprintw(0, 2, "Score: %d", score);
            mvprintw(0, COLS / 2 - 6, "High Score: %d", high_score);
            mvprintw(0, COLS - 10, "LIVES: %d", player.lives);
            // mvprintw()
            refresh();
        }
        else if (GAME_OVER)
        {

            draw_game_over();
        }

        unlock_master_mutex;
        usleep(DELAY); // espera el proximo ciclo
        speed = (speed + 1) % 10;
    }

    return NULL;
}

void init_game()
{
    player.x = COLS / 2;
    player.y = LINES - 9;
    score = 0;
    player.lives = 3;

    for (int i = 0; i < NUMBER_BULLETS; i++)
    {
        bullets[i].active = 0;
    }
    for (int i = 0; i < NUMBER_ALIENS; i++)
    {
        aliens[i].active = 0;
    }
}

void move_player(int direction)
{

    switch (direction)
    {
    case KEY_LEFT:
        if (player.x > 0)
        {
            player.x--;
        }
        break;
    case KEY_RIGHT:
        if (player.x < COLS - 5)
        {
            player.x++;
        }
        break;
    case KEY_UP:
        if (player.y > 0)
        {
            player.y--;
        }
        break;
    case KEY_DOWN:
        if (player.y < LINES - 3)
        {
            player.y++;
        }
        break;
    }
}

void shoot()
{
    for (int i = 0; i < NUMBER_BULLETS; i++)
    {
        if (!bullets[i].active)
        {
            bullets[i].x = player.x + 2;
            bullets[i].y = player.y;
            bullets[i].active = 1;
            break;
        }
    }
}

void *input_handler(void *args)
{

    while (!EXIT)
    {

        lock_master_mutex;

        if (START)
        {
            // SetColor();
            getmaxyx(stdscr, h, w);

            static int menu_item = 0;
            if (ch == KEY_UP)
                menu_item--;
            if (ch == KEY_DOWN)
                menu_item++;

            if (menu_item >= 2)
                menu_item = 2;
            if (menu_item <= 0)
                menu_item = 0;

            draw_logo(h, w);

            int select_start_game = menu_item == 0 ? 0 : 1;
            mvprintw(h / 2 - logo_h_size + 9, w / 2 - str_len(item_start_game[select_start_game]) / 2, item_start_game[select_start_game]);

            // Item info
            int select_info = menu_item == 1 ? 0 : 1;
            mvprintw(h / 2 - logo_h_size + 11, w / 2 - str_len(item_info[select_info]) / 2, item_info[select_info]);

            // Item exit
            int select_exit = menu_item == 2 ? 0 : 1;
            mvprintw(h / 2 - logo_h_size + 13, w / 2 - str_len(item_exit[select_exit]) / 2, item_exit[select_exit]);

            attron(COLOR_PAIR(1));
            box(stdscr, 0, 0);
            attron(COLOR_PAIR(1));

            if (ch == vk_enter)
            {
                init_game();
                switch (menu_item)
                {
                case 0:
                    actual_state = run;
                    // init_game();
                    break;
                case 1:
                    actual_state = info;
                    break;
                case 2:
                    actual_state = out;
                    break;
                }
            }

            if (ch == 'q')
                actual_state = start;

            // Get key pressed
            ch = wgetch(stdscr);
            napms(100);
            ch = wgetch(stdscr);

            // Clear
            erase();
        }
        else if (RUN)
        {
            ch = getch();
            switch (ch)
            {
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
                actual_state = game_over;
                break;
            }
        }

        else if (GAME_OVER)
        {
            ch = getch();
            if (ch == 's')
            {
                actual_state = start;
            }
            else if (ch == 'q')
            {
                actual_state = out;
            }
        }

        unlock_master_mutex;
    }

    return NULL;
}

int main()
{

    srand(time(NULL)); // Inicializa la semilla para generar números aleatorios
    initscr();         // Inicia el modo ncurses
    keypad(stdscr, TRUE);
    savetty();
    cbreak();
    noecho();        // Desactiva el eco de teclado
    curs_set(FALSE); // Oculta el cursor
    timeout(0);      // Configura getch para ser no bloqueante
    leaveok(stdscr, 1);
    start_color(); // iniciar color

    // Definir pares de colores
    init_pair(1, COLOR_BLUE, COLOR_BLACK);
    init_pair(2, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(3, COLOR_GREEN, COLOR_BLACK);
    init_pair(4, COLOR_RED, COLOR_BLACK);
    init_pair(5, COLOR_YELLOW, COLOR_BLACK);

    pthread_t game_thread, input_thread;

    pthread_create(&game_thread, NULL, development_game, NULL);
    pthread_create(&input_thread, NULL, input_handler, NULL);

    pthread_join(game_thread, NULL);
    pthread_join(input_thread, NULL);

    endwin();

    pthread_mutex_destroy(&master_mutex);

    return 0;
}