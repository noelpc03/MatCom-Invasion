#include <ncurses.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string.h>

#define DELAY 60000
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
    info,
    pause_game,
    see_scores
} state;

state actual_state = start;

#define EXIT (actual_state == out)
#define START (actual_state == start)
#define RUN (actual_state == run)
#define GAME_OVER (actual_state == game_over)
#define PAUSE (actual_state == pause_game)
#define SEE_SCORES (actual_state == see_scores)

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
typedef struct
{
    Alien aliens[100];    // Estructura que representa los aliens
    Bullets bullets[100]; // Estructura que representa las balas
    int score;
    Player player;
} GameState;

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
int count = 0;
#define vk_enter 10
<<<<<<< Updated upstream
=======
#define MAX_HIGH_SCORES 10
int high_scores[MAX_HIGH_SCORES] = {0};
GameState game_state;
>>>>>>> Stashed changes

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

<<<<<<< Updated upstream
const char *item_start_game[2] = {
    "> START GAME <",
    "start game",
=======
const char *item_start_new_game[2] = {
    "> START NEW GAME <",
    "start new game",
>>>>>>> Stashed changes
};

// Item info
const char *item_start_saved_game[2] = {
    "> START SAVED GAME <",
    "start saved game",
};

const char *item_best_scores[2] = {
    "> BEST SCORES <",
    "best scores",
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

<<<<<<< Updated upstream
=======
void draw_pause()
{
    clear();
    attron(COLOR_PAIR(5));
    mvprintw(LINES / 2 - 2, COLS / 2 - 10, "Exit Menu");
    attroff(COLOR_PAIR(5));
    mvprintw(LINES / 2, COLS / 2 - 10, "Press 's' to Return to Start Screen");
    mvprintw(LINES / 2 + 1, COLS / 2 - 10, "Press 'q' to Quit");
    mvprintw(LINES / 2 + 2, COLS / 2 - 10, "Press 'c' to Save and Quit");
    mvprintw(LINES / 2 + 3, COLS / 2 - 10, "Score: %d", score);
    mvprintw(LINES / 2 + 4, COLS / 2 - 10, "High Score: %d", high_score);
    refresh();
}
void draw_high_scores(int high_scores[], int num_scores)
{
    clear(); // Limpiar la pantalla antes de mostrar los puntajes

    mvprintw(LINES / 2 - 2, COLS / 2 - 10, "High Scores");

    for (int i = 0; i < num_scores; i++)
    {
        if (high_scores[i] < 0)
        {
            mvprintw(LINES / 2 + i, COLS / 2 - 10, "%d. %d", i + 1, "---");
        }
        else
        {
            mvprintw(LINES / 2 + i, COLS / 2 - 10, "%d. %d", i + 1, high_scores[i]);
        }
    }

    mvprintw(LINES - 2, (COLS / 2) - 10, "Press 's' to return to Start Menu");
    refresh(); // Refrescar la pantalla para mostrar los cambios
}

>>>>>>> Stashed changes
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
<<<<<<< Updated upstream
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
=======
            aliens[i].y += 1;

            // // Cambiar dirección si el alien alcanza el borde de la pantalla
            // if ((aliens[i].x >= COLS - 3 && aliens[i].direction == 1) || (aliens[i].x <= 0 && aliens[i].direction == -1))
            // {
            //     aliens[i].direction *= -1;
            //     aliens[i].y += 2; // Mover hacia abajo cuando cambia de dirección
            // }
        }
        else
        {
            // Intentar activar un nuevo alien con 1% de probabilidad
            if (rand() % 100 < 1)
            {
                int spawn_x = rand() % (COLS - 3); // Generar una posición aleatoria para el nuevo alien

                // Verificar si hay colisión con otros aliens activos
                int collision = 0;
                for (int j = 0; j < NUMBER_ALIENS; j++)
                {
                    if (aliens[j].active &&
                        spawn_x >= aliens[j].x &&
                        spawn_x < aliens[j].x + 3 &&
                        aliens[j].y == 3)
                    {
                        collision = 1;
                        break;
                    }
                }

                // Si no hay colisión, activar el nuevo alien
                if (!collision)
                {
                    aliens[i].y = 3;
                    aliens[i].x = spawn_x;
                    aliens[i].active = 1;
                    aliens[i].direction = 1;
                }
>>>>>>> Stashed changes
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
<<<<<<< Updated upstream

=======
>>>>>>> Stashed changes
    collision_bullets();
    collision_player();
}

<<<<<<< Updated upstream
=======
// Funciones para guardar y cargar juego

void save_game(GameState *state)
{
    FILE *file = fopen("saved_game.bin", "wb");
    if (file != NULL)
    {
        fwrite(state, sizeof(GameState), 1, file);
        fclose(file);
    }
    else
    {
        printf("Error al guardar la partida.\n");
    }
}

int load_game(GameState *state)
{
    FILE *file = fopen("saved_game.bin", "rb");
    if (file != NULL)
    {
        fread(state, sizeof(GameState), 1, file);
        fclose(file);
        return 1; // Éxito
    }
    else
    {
        printf("No se pudo cargar la partida guardada.\n");
        return 0; // Error
    }
}
void pass_info(GameState state)
{
    for (int i = 0; i < 100; i++)
    {
        aliens[i] = state.aliens[i];
    }
    for (int i = 0; i < 100; i++)
    {
        bullets[i] = state.bullets[i];
    }
    player = state.player;
    score = state.score;
}

// Funciones que controlan los high scores

void add_new_score(int new_score)
{
    // Buscar la posición correcta para el nuevo puntaje
    for (int i = 0; i < MAX_HIGH_SCORES; i++)
    {
        if (new_score > high_scores[i])
        {
            // Desplazar puntajes más bajos hacia abajo
            for (int j = MAX_HIGH_SCORES - 1; j > i; j--)
            {
                high_scores[j] = high_scores[j - 1];
            }
            // Insertar el nuevo puntaje en la posición correcta
            high_scores[i] = new_score;
            return; // Salir de la función después de insertar el nuevo puntaje
        }
    }
}

void save_high_scores(const char *filename)
{
    FILE *file = fopen(filename, "w");
    if (file == NULL)
    {
        perror("Error al abrir el archivo para guardar puntajes");
        return;
    }
    for (int i = 0; i < MAX_HIGH_SCORES; i++)
    {
        fprintf(file, "%d\n", high_scores[i]);
    }
    fclose(file);
}

void load_high_scores(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        perror("Error al abrir el archivo para cargar puntajes");
        return;
    }
    for (int i = 0; i < MAX_HIGH_SCORES; i++)
    {
        if (fscanf(file, "%d", &high_scores[i]) != 1)
        {
            high_scores[i] = 0; // Si no hay más datos, inicializar a 0
        }
    }
    fclose(file);
}

>>>>>>> Stashed changes
void *development_game(void *args)
{

    int speed = 0;

    while (!EXIT)
    {

        lock_master_mutex;

        if (START)
        {
<<<<<<< Updated upstream
            // init_game();
            //  draw_logo(h, w);
            //  dibujar la pantalla de inicio
=======
            load_high_scores("high_scores.txt");
            high_score = high_scores[0];
>>>>>>> Stashed changes
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
            add_new_score(score);
            save_high_scores("high_scores.txt");
        }
        else if (PAUSE)
        {
            draw_pause();
            add_new_score(score);
            save_high_scores("high_scores.txt");
        }
        else if (SEE_SCORES)
        {
            draw_high_scores(high_scores, 10);
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

            if (menu_item >= 3)
                menu_item = 3;
            if (menu_item <= 0)
                menu_item = 0;

            draw_logo(h, w);

            int select_start_new_game = menu_item == 0 ? 0 : 1;
            mvprintw(h / 2 - logo_h_size + 9, w / 2 - str_len(item_start_new_game[select_start_new_game]) / 2, item_start_new_game[select_start_new_game]);

            // Item saved game
            int select_start_saved_game = menu_item == 1 ? 0 : 1;
            mvprintw(h / 2 - logo_h_size + 11, w / 2 - str_len(item_start_saved_game[select_start_saved_game]) / 2, item_start_saved_game[select_start_saved_game]);

            // Item best scores
            int select_best_scores = menu_item == 2 ? 0 : 1;
            mvprintw(h / 2 - logo_h_size + 12, w / 2 - str_len(item_best_scores[select_best_scores]) / 2, item_best_scores[select_best_scores]);

            // Item exit
            int select_exit = menu_item == 3 ? 0 : 1;
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
<<<<<<< Updated upstream
                    // init_game();
=======
>>>>>>> Stashed changes
                    break;
                case 1: // Cargar partida guardada
                    int load = load_game(&game_state);
                    if (load)
                    {
                        pass_info(game_state);
                    }
                    actual_state = run; 
                    break;
                case 2:
                    actual_state = see_scores;
                case 3:
                    actual_state = out;
                    break;
                }
            }

            if (ch == 'q')
                actual_state = pause_game;

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
                actual_state = PAUSE;
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
<<<<<<< Updated upstream
=======
            }
        }

        else if (PAUSE)
        {
            ch = getch();
            switch (ch)
            {
            case 'q':
                actual_state = out;
                break;
            case 's':
                actual_state = start;
                break;
            case 'c': // Salir y guardar
                save_game(&game_state);
                actual_state = out;
                break;
            case 'x': // Guardar y volver a inicio
                save_game(&game_state);
                actual_state = start;
                break;
            }
        }
        else if (see_scores)
        {
            ch = getch();
            switch (ch)
            {
            case 's':
                actual_state = start;
                break;
>>>>>>> Stashed changes
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