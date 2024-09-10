#include <ncurses.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <dirent.h>
#include <ctype.h>
#include <sys/stat.h>
#include <time.h>

#pragma region GLOBAL VARIABLES
#define DELAY 90000 // Tiempo de espera entre actualizaciones en microsegundos

pthread_mutex_t master_mutex = PTHREAD_MUTEX_INITIALIZER;
#define lock_master_mutex pthread_mutex_lock(&master_mutex)
#define unlock_master_mutex pthread_mutex_unlock(&master_mutex)

typedef enum
{
    start,
    run,
    game_over,
    out,
    info,
    pause_game,
    see_scores,
    error,
    select_game,
    saving_error
} state;

state actual_state = start;

#define EXIT (actual_state == out)
#define START (actual_state == start)
#define RUN (actual_state == run)
#define GAME_OVER (actual_state == game_over)
#define PAUSE (actual_state == pause_game)
#define SEE_SCORES (actual_state == see_scores)
#define ERROR (actual_state == error)
#define SELECT_GAME (actual_state == select_game)
#define SAVING_ERROR (actual_state == saving_error)

typedef struct
{
    int x, y, active;
} Bullets;

typedef struct
{
    int x, y, active, type;
    int index_frame;
} Alien;

typedef struct
{
    int x, y, lives;
} Player;

typedef struct
{
    int last_time, active;
    Alien alien;
} Frame;

typedef struct
{
    int score;
    char name[20];
    char date[40];
} Score;

#define NUMBER_BULLETS 10
#define NUMBER_ALIENS 80
#define NUMBER_FRAMES 40
#define MAX_HIGH_SCORES 10
Bullets bullets[NUMBER_BULLETS + 5];
Alien aliens[NUMBER_ALIENS + 5];
Frame frame_page[NUMBER_FRAMES];
Player player;
Score high_scores[MAX_HIGH_SCORES];
int ptr_type_alien; // puntero a la proxima posicion desocupada
int ptr_type_bullet;

int menu_item = 0;
int menu_game_over = 0;
int menu_pause = 0;

Score score;
int high_score = 0;
int count = 0;


#pragma endregion

#pragma region DRAW
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
    "`88888'  `88888P8 dP  dP  dP `88888P'     `8888P'  8888P'   `88888P' dP       ",

};

const char *menu_logo_pause[6] = {
    "888888ba    ",
    "88    `8b      ",
    "a88aaaa8P' .d8888b. dP    dP .d8888b. .d8888b. ",
    "88        88'  `88 88    88 Y8ooooo. 88ooood8 ",
    "88        88.  .88 88.  .88       88 88.  ... ",
    "dP        `88888P8 `88888P' `88888P' `88888P' ",

};

const char *item_start_new_game[2] = {
    "> START NEW GAME <",
    "start new game",
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
        mvprintw(5 + i, w / 2 - logo_w_size, menu_logo[i]);
    }
    attroff(COLOR_PAIR(1));
}

void draw_bullets_player()
{
    for (int i = 0; i < NUMBER_BULLETS + 5; i++)
    {
        if (bullets[i].active)
        {
            attron(COLOR_PAIR(3));
            mvprintw(bullets[i].y, bullets[i].x, "|");
            attroff(COLOR_PAIR(3));
        }
    }
}

void draw_aliens()
{
    for (int i = 0; i < NUMBER_ALIENS + 5; i++)
    {
        if (aliens[i].active)
        {
            attron(COLOR_PAIR(4));
            mvprintw(aliens[i].y - 1, aliens[i].x, "{@}");
            mvprintw(aliens[i].y, aliens[i].x, "/\"\\");
            attroff(COLOR_PAIR(4));
        }
    }
}

void draw_player()
{

    attron(COLOR_PAIR(1));
    mvprintw(player.y, player.x, "  ^  ");
    mvprintw(player.y + 1, player.x, " mAm ");
    mvprintw(player.y + 2, player.x, "mAmAm");
    attroff(COLOR_PAIR(1));

    refresh();
    getch();

    endwin();
}

void auxiliar_draw(int v, const char *item[2], int index)
{

    if (!v)
    {
        attron(COLOR_PAIR(3));
        mvprintw(LINES / 2 + index, COLS / 2 - 20, item[v]);
        attroff(COLOR_PAIR(3));
    }
    else
    {
        mvprintw(LINES / 2 + index, COLS / 2 - 20, item[v]);
    }
}

// Muestra la pantalla de inicio con instrucciones para comenzar o salir
void draw_start_screen(int *free)
{
    clear();

    if (*free)
    {
        snprintf(score.name, sizeof(score.name), "                   ");
        *free = 0;
    }
    // attron(COLOR_PAIR(6));
    mvprintw(LINES / 2, COLS / 2 - 20, "Enter your name and press Space : %s", score.name);
    mvprintw(LINES / 2 + 2, COLS / 2 - 20, "High Score: %d", high_score);
    // attroff(COLOR_PAIR(6));

    attron(COLOR_PAIR(1));
    for (int i = 0; i < 6; i++)
    {
        mvprintw(LINES / 2 - 9 + i, COLS / 2 - 55, menu_logo[i]);
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
// Muestra la pantalla de fin del juego con puntuación y opciones
void draw_game_over_screen()
{
    clear();
    attron(COLOR_PAIR(4));

    for (int i = 0; i < 6; i++)
    {
        mvprintw(LINES / 2 - 9 + i, COLS / 2 - 55, menu_logo_game_over[i]);
    }

    attroff(COLOR_PAIR(4));

    const char *item_return_start[2] = {
        "> RETURN START <",
        "return start",
    };
    const char *item_quit[2] = {
        "> Quit <",
        "quit",
    };

    int v = (menu_game_over == 0) ? 0 : 1;

    auxiliar_draw(v, item_return_start, 0);
    v = (menu_game_over == 1) ? 0 : 1;

    auxiliar_draw(v, item_quit, 2);
    mvprintw(LINES / 2 + 4, COLS / 2 - 20, "Score: %d", score.score);
    mvprintw(LINES / 2 + 6, COLS / 2 - 20, "High Score: %d", high_score);
    refresh();
}

void draw_pause()
{
    clear();
    attron(COLOR_PAIR(5));
    for (int i = 0; i < 6; i++)
    {
        mvprintw(LINES / 2 - 9 + i, COLS / 2 - 30, menu_logo_pause[i]);
    }
    attroff(COLOR_PAIR(5));

    const char *item_return_start[2] = {
        "> RETURN START <",
        "return start",
    };
    const char *item_quit[2] = {
        "> Quit <",
        "quit",
    };
    const char *item_save_quit[2] = {
        "> SAVE AND QUIT<",
        "save and quit",
    };
    const char *item_save_ret[2] = {
        "> SAVE AND RETURN<",
        "save and return",
    };
    const char *item_continue[2] = {
        "> CONTNUE<",
        "continue",
    };

    int v = (menu_pause == 0) ? 0 : 1;

    auxiliar_draw(v, item_return_start, 0);

    v = (menu_pause == 1) ? 0 : 1;

    auxiliar_draw(v, item_quit, 2);

    v = (menu_pause == 2) ? 0 : 1;

    auxiliar_draw(v, item_save_quit, 4);

    v = (menu_pause == 3) ? 0 : 1;

    auxiliar_draw(v, item_save_ret, 6);

    // mvprintw(LINES / 2 + 3, COLS / 2 - 10, "Press 'x' to Save and Return to Start Screen");
    //  mvprintw(LINES / 2 + 4, COLS / 2 - 10, "Press 'v' to Save in USB and Return to Start Screen");
    //  mvprintw(LINES / 2 + 5, COLS / 2 - 10, "Press 'b' to Save in USB and Quit");
    v = (menu_pause == 4) ? 0 : 1;

    auxiliar_draw(v, item_continue, 8);

    mvprintw(LINES / 2 + 10, COLS / 2 - 20, "Score: %d", score.score);
    mvprintw(LINES / 2 + 12, COLS / 2 - 20, "High Score: %d", high_score);
    refresh();
}

void draw_load_error()
{
    clear();
    attron(COLOR_PAIR(4));
    mvprintw(LINES / 2 - 2, COLS / 2 - 10, "Error");
    attroff(COLOR_PAIR(4));
    mvprintw(LINES / 2 - 1, COLS / 2 - 10, "Error trying to load game. It seems there are no saved files.");
    mvprintw(LINES / 2 + 1, COLS / 2 - 10, "Press 's' to Return to Start Screen");
    mvprintw(LINES / 2 + 1, COLS / 2 + 2, "%s", score.name);
    mvprintw(LINES / 2 + 2, COLS / 2 - 10, "Press 'q' to Quit");
    refresh();
}

void draw_saving_error()
{
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
void draw_select_screen()
{
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

void draw_high_scores()
{
    clear(); // Limpiar la pantalla antes de mostrar los puntajes

    mvprintw(LINES / 2 - 7, COLS / 2 - 10, "High Scores");

    for (int i = 0; i < MAX_HIGH_SCORES; i++)
    {
        if (high_scores[i].score < 0)
        {
            mvprintw(LINES / 2 + i - 4, COLS / 2 - 10, "%d. %d", i + 1, "---");
        }
        else
        {
            if (i < 3)
            {
                attron(COLOR_PAIR(6)); // Oro para los primeros 3
            }
            else if (i < 6)
            {
                attron(COLOR_PAIR(7)); // Plata para los siguientes 3
            }
            else
            {
                attron(COLOR_PAIR(8)); // Bronce para el resto
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
    refresh(); // Refrescar la pantalla para mostrar los cambios
}
// Funciones que controlan los high scores
#pragma endregion

#pragma region SCORES
void add_new_score(Score new_score)
{
    // Obtener la fecha actual
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    snprintf(new_score.date, sizeof(new_score.date), "%02d-%02d-%04d_%02d:%02d:%02d",
             tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900,
             tm.tm_hour, tm.tm_min, tm.tm_sec);

    // Buscar la posición correcta para el nuevo puntaje
    for (int i = 0; i < MAX_HIGH_SCORES; i++)
    {

        if (new_score.score > high_scores[i].score)
        {
            // Desplazar puntajes más bajos hacia abajo
            for (int j = MAX_HIGH_SCORES - 1; j > i; j--)
            {
                high_scores[j].score = high_scores[j - 1].score;
                strcpy(high_scores[j].name, high_scores[j - 1].name);
                strcpy(high_scores[j].date, high_scores[j - 1].date);
            }
            // Insertar el nuevo puntaje en la posición correcta
            high_scores[i].score = new_score.score;
            strcpy(high_scores[i].name, new_score.name);
            strcpy(high_scores[i].date, new_score.date);

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
        fprintf(file, "%d\n", high_scores[i].score); // Guardar el score
        fprintf(file, "%s\n", high_scores[i].name); // Guardar el nombre
        fprintf(file, "%s\n", high_scores[i].date); // Guardar la fecha
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
        if (fscanf(file, "%d", &high_scores[i].score) != 1)
        {
            high_scores[i].score = 0; // Si no hay más datos, inicializar a 0
            snprintf(high_scores[i].name, sizeof(high_scores[i].name), "PlayerX");
            snprintf(high_scores[i].date, sizeof(high_scores[i].date), "N/A");
        }
        else if (fscanf(file, "%s", high_scores[i].name) != 1)
        {
            snprintf(high_scores[i].name, sizeof(high_scores[i].name), "PlayerX");
            snprintf(high_scores[i].date, sizeof(high_scores[i].date), "N/A");
        }
        else if (fscanf(file, "%s", high_scores[i].date) != 1)
        {
            snprintf(high_scores[i].date, sizeof(high_scores[i].date), "N/A");
        }
    }
    fclose(file);
}

// Funciones para guardar y cargar juego

#pragma endregion

#pragma region SAVED_GAMES

void save_game(const char *filename)
{
    FILE *file = fopen(filename, "wb");
    if (file == NULL)
    {
        printf("Error al abrir el archivo para guardar la partida.\n");
        return;
    }

    // Guarda las balas
    fwrite(bullets, sizeof(Bullets), NUMBER_BULLETS, file);

    // Guarda los aliens
    fwrite(aliens, sizeof(Alien), NUMBER_ALIENS, file);

    // Guarda el jugador
    fwrite(&player, sizeof(Player), 1, file);

    // Guarda el score
    fwrite(&score, sizeof(Score), 1, file);

    fclose(file);
    printf("Partida guardada exitosamente.\n");
}

void load_game(const char *filename)
{
    FILE *file = fopen(filename, "rb");
    if (file == NULL)
    {
        printf("Error al abrir el archivo para cargar la partida.\n");
        return;
    }

    // Carga las balas
    fread(bullets, sizeof(Bullets), NUMBER_BULLETS, file);

    // Carga los aliens
    fread(aliens, sizeof(Alien), NUMBER_ALIENS, file);

    // Carga el jugador
    fread(&player, sizeof(Player), 1, file);

    // Carga el score
    fread(&score, sizeof(Score), 1, file);

    fclose(file);
    printf("Partida cargada exitosamente.\n");
}

// Función para detectar si estamos en WSL
// int isWSL()
// {
//     FILE *file = fopen("/proc/version", "r");
//     if (file == NULL)
//     {
//         return 0;
//     }

//     char buffer[256];
//     fgets(buffer, sizeof(buffer), file);
//     fclose(file);

//     return strstr(buffer, "Microsoft") != NULL;
// }

// // Función para listar dispositivos USB en Arch Linux usando `stat()` en lugar de `d_type`
// void listUSBDevicesArch(char usbDevices[][256], int *numDevices)
// {
//     const char *usbMountPath = "/run/media";
//     DIR *dir = opendir(usbMountPath);
//     if (dir == NULL)
//     {
//         printf("No se puede abrir el directorio %s\n", usbMountPath);
//         return;
//     }

//     struct dirent *entry;
//     *numDevices = 0;

//     while ((entry = readdir(dir)) != NULL)
//     {
//         if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
//         {
//             char userPath[256];
//             snprintf(userPath, sizeof(userPath), "%s/%s", usbMountPath, entry->d_name);

//             DIR *userDir = opendir(userPath);
//             if (userDir != NULL)
//             {
//                 struct dirent *usbEntry;
//                 while ((usbEntry = readdir(userDir)) != NULL)
//                 {
//                     if (strcmp(usbEntry->d_name, ".") != 0 && strcmp(usbEntry->d_name, "..") != 0)
//                     {
//                         char fullPath[512];
//                         snprintf(fullPath, sizeof(fullPath), "%s/%s", userPath, usbEntry->d_name);

//                         struct stat fileStat;
//                         if (stat(fullPath, &fileStat) == 0 && S_ISDIR(fileStat.st_mode))
//                         {
//                             snprintf(usbDevices[*numDevices], 256, "%s", fullPath);
//                             (*numDevices)++;
//                         }
//                     }
//                 }
//                 closedir(userDir);
//             }
//         }
//     }
//     closedir(dir);
// }

// // Función para listar dispositivos USB en WSL usando `stat()` en lugar de `d_type`
// void listUSBDevicesWSL(char usbDevices[][256], int *numDevices)
// {
//     const char *mntPath = "/mnt";
//     DIR *dir = opendir(mntPath);
//     if (dir == NULL)
//     {
//         printf("No se puede abrir el directorio %s\n", mntPath);
//         return;
//     }

//     struct dirent *entry;
//     *numDevices = 0;

//     while ((entry = readdir(dir)) != NULL)
//     {
//         if (strlen(entry->d_name) == 1 && entry->d_name[0] >= 'a' && entry->d_name[0] <= 'z')
//         {
//             char devicePath[256];
//             snprintf(devicePath, sizeof(devicePath), "%s/%s", mntPath, entry->d_name);

//             struct stat fileStat;
//             if (stat(devicePath, &fileStat) == 0 && S_ISDIR(fileStat.st_mode))
//             {
//                 snprintf(usbDevices[*numDevices], 256, "%s", devicePath);
//                 (*numDevices)++;
//             }
//         }
//     }

//     closedir(dir);
// }

// // Función principal para listar dispositivos USB dependiendo del entorno
// void listUSBDevices(char usbDevices[][256], int *numDevices)
// {
//     if (isWSL())
//     {
//         listUSBDevicesWSL(usbDevices, numDevices);
//     }
//     else
//     {
//         listUSBDevicesArch(usbDevices, numDevices);
//     }
// }

// // Funciones de guardado y carga
// void saveGameToUSB()
// {
//     char usbDevices[10][256];
//     int numDevices = 0;

//     listUSBDevices(usbDevices, &numDevices);

//     if (numDevices == 0)
//     {
//         printf("No se detectaron dispositivos USB.\n");
//         return;
//     }

//     // Usa la primera USB disponible
//     char filePath[256];
//     snprintf(filePath, sizeof(filePath), "%s/save_game.dat", usbDevices[0]);

//     FILE *file = fopen(filePath, "wb");
//     if (file == NULL)
//     {
//         printf("Error al abrir el archivo para guardar en la USB.\n");
//         return;
//     }

//     // Guardar los datos del juego
//     fwrite(bullets, sizeof(Bullets), NUMBER_BULLETS, file);
//     fwrite(aliens, sizeof(Alien), NUMBER_ALIENS, file);
//     fwrite(&player, sizeof(Player), 1, file);
//     fwrite(&score, sizeof(score), 1, file);
//     fclose(file);
//     printf("Partida guardada exitosamente en %s\n", filePath);
//     saved = true;
// }

// void loadGameFromUSB()
// {
//     char usbDevices[10][256];
//     int numDevices = 0;

//     listUSBDevices(usbDevices, &numDevices);

//     if (numDevices == 0)
//     {
//         printf("No se detectaron dispositivos USB.\n");
//         return;
//     }

//     // Revisa en cada USB conectada si existe el archivo de guardado
//     for (int i = 0; i < numDevices; i++)
//     {
//         char filePath[256];
//         snprintf(filePath, sizeof(filePath), "%s/save_game.dat", usbDevices[i]);

//         // Verifica si el archivo existe
//         struct stat buffer;
//         if (stat(filePath, &buffer) == 0)
//         {
//             // Archivo encontrado, cargar partida
//             FILE *file = fopen(filePath, "rb");
//             if (file == NULL)
//             {
//                 printf("Error al abrir el archivo %s para cargar la partida.\n", filePath);
//                 continue;
//             }

//             fread(bullets, sizeof(Bullets), NUMBER_BULLETS, file);
//             fread(aliens, sizeof(Alien), NUMBER_ALIENS, file);
//             fread(&player, sizeof(Player), 1, file);
//             fread(&score, sizeof(score), 1, file);
//             fclose(file);
//             printf("Partida cargada exitosamente desde %s\n", filePath);
//             loaded = true;
//             return;
//         }
//     }

//     printf("No se encontró el archivo de guardado en ninguna USB.\n");
// }

#pragma endregion

#pragma region UPDATE_GAME

void new_next_fit_alien()
{
    // buscar la proxima posicion desocupada por ptr_type_alien y devolver el valor de ptr_type
    int i = (ptr_type_alien + 1) % (NUMBER_ALIENS + 5);

    for (int j = i; j != ptr_type_alien; j = (j + 1) % (NUMBER_ALIENS + 5))
    {

        if (!aliens[j].active)
        {
            ptr_type_alien = j;
            return;
        }
    }
}

int algorthim_lru(Alien alien)
{
    // itera por cada frames y determina la que lleva mas tiempo desocupada para ubicar el nuevo alien
    int max_occuped_time = 0;
    int index = 0;

    for (int i = 0; i < NUMBER_FRAMES; i++)
    {
        if (!frame_page[i].active)
        {
            frame_page[i].active = 1;
            frame_page[i].last_time = 1;
            frame_page[i].alien = alien;
            return 3 * i;
        }
        if (max_occuped_time < frame_page[i].last_time)
        {
            max_occuped_time = frame_page[i].last_time;
            index = i;
        }
    }

    frame_page[index].active = 1;
    frame_page[index].last_time = 1;
    frame_page[index].alien = alien;
    return 3 * index;
}

void update_bullets()
{
    for (int i = 0; i < NUMBER_BULLETS + 5; i++)
    {
        if (bullets[i].active)
        {
            bullets[i].y--;
            if (bullets[i].y < 3)
            {
                bullets[i].active = 0;
            }
        }
    }
}

void update_score(int aliens_type, int finish)
{

    if (finish)
    {
        score.score -= 2 * aliens_type;
    }
    else
    {
        score.score += 4 * aliens_type;
    }
}

void update_aliens()
{
    int number_aliens_active = 0;

    // if (aliens[5].active )
    // {
    //     aliens[5].bullet_alien.active=1;
    //     aliens[5].bullet_alien.y +=2;
    //     aliens[5].bullet_alien.x=aliens[5].x;
    // }

    for (int i = 0; i < NUMBER_ALIENS + 5; i++)
    {

        if (aliens[i].active)
        {
            number_aliens_active += 1;
            aliens[i].y+= (i%4)+1;
            if (aliens[i].y >= LINES - 1)
            {
                aliens[i].active = 0;
                update_score(aliens[i].type, 1);
            }
        }
    }

    for (int i = number_aliens_active; i <= NUMBER_ALIENS; i++)
    {
        if (rand() % 100 < 5)
        {
            aliens[ptr_type_alien].active = 1;
            aliens[ptr_type_alien].x = algorthim_lru(aliens[ptr_type_alien]);
            aliens[ptr_type_alien].index_frame = aliens[ptr_type_alien].x / 3;
            aliens[ptr_type_alien].y = 3;
            aliens[ptr_type_alien].type = ptr_type_alien;
            new_next_fit_alien(); // mover el ptr_type_alien
        }
    }

    if (number_aliens_active > NUMBER_ALIENS)
    {
        printf("Ha ocurrido un error");
    }
}

void update_frames()
{

    for (int i = 0; i < NUMBER_FRAMES; i++)
    {
        if (frame_page[i].active)
        {
            frame_page[i].last_time += 1;
        }
    }
}

void check_collisions()
{
    for (int i = 0; i < NUMBER_BULLETS + 5; i++)
    {
        if (bullets[i].active)
        {
            for (int j = 0; j < NUMBER_ALIENS + 5; j++)
            {
                if (aliens[j].active)
                {

                    int first[6] = {aliens[j].x, aliens[j].x + 1, aliens[j].x + 2, aliens[j].x, aliens[j].x + 1, aliens[j].x + 2};
                    int second[6] = {aliens[j].y, aliens[j].y, aliens[j].y, aliens[j].y - 1, aliens[j].y - 1, aliens[j].y - 1};

                    for (int k = 0; k < 6; k++)
                    {
                        if (bullets[i].x == first[k] && bullets[i].y == second[k])
                        {
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

    int first_player[9] = {player.x + 2, player.x + 1, player.x + 2, player.x + 3, player.x, player.x + 1, player.x + 2, player.x + 3, player.x + 4};
    int second_player[9] = {player.y, player.y + 1, player.y + 1, player.y + 1, player.y + 2, player.y + 2, player.y + 2, player.y + 2, player.y + 2};

    int occuped = 0;
    for (int i = 0; i < NUMBER_ALIENS + 5; i++)
    {
        if (occuped)
            break;

        if (aliens[i].active)
        {
            int first_alien[6] = {aliens[i].x, aliens[i].x + 1, aliens[i].x + 2, aliens[i].x, aliens[i].x + 1, aliens[i].x + 2};
            int second_alien[6] = {aliens[i].y, aliens[i].y, aliens[i].y, aliens[i].y - 1, aliens[i].y - 1, aliens[i].y - 1};

            for (int k = 0; k < 6; k++)
            {
                if (occuped)
                    break;
                for (int m = 0; m < 9; m++)
                {
                    if (first_alien[k] == first_player[m] && second_alien[k] == second_player[m])
                    {
                        aliens[i].active = 0;
                        frame_page[aliens[i].index_frame].active = 0;
                        player.lives--;
                        occuped = 1;
                        break;
                    }
                }
            }
        }
    }
}

#pragma endregion

#pragma region DEVELOPMENT_GAME
// Inicializa el juego, reseteando variables y posicionando al jugador y elementos en sus estados iniciales
void init_game()
{
    player.x = COLS / 2;
    player.y = LINES - 9;
    score.score = 0;
    player.lives = 3;
    ptr_type_alien = 0;
    ptr_type_bullet = 0;
    // Desactiva todos los proyectiles y enemigos al inicio de una nueva partida
    for (int i = 0; i < NUMBER_BULLETS + 5; i++)
    {
        bullets[i].active = 0;
    }
    for (int i = 0; i < NUMBER_ALIENS + 5; i++)
    {
        aliens[i].active = 0;
    }
    for (int i = 0; i < NUMBER_FRAMES; i++)
    {

        frame_page[i].active = 0;
    }
}

// Bucle principal del juego, controla el estado del juego y actualiza la pantalla según el estado actual
void *development_game(void *arg)
{
    int move_enemy = 0; // controla el mover de los enemigos
    int mask = 0;       // bool para evitar el error de cargar el high_score mas de una vez
    snprintf(score.name, sizeof(score.name), "");
    int free = 0;
    while (!EXIT)
    {
        lock_master_mutex;
        // Dependiendo del estado, muestra la pantalla de inicio, actualiza el juego o la pantalla de fin de juego
        if (START)
        {
            mask = 0;
            draw_start_screen(&free);
            load_high_scores("/home/jabel/Proyecto_SO/MatCom-Invasion/high_scores.txt");
            high_score = high_scores[0].score;
        }
        else if (RUN)
        {

            free = 1;
            update_bullets();

            if (!move_enemy)
            {
                update_aliens();
                update_frames();
                // update_bullets_enemy();
            }

            check_collisions();

            if (player.lives == 0)
            {
                actual_state = game_over;

                if (score.score > high_score)
                {
                    high_score = score.score;
                }
            }

            clear();
            draw_player();

            mvprintw(1, 2, "HP: %d", player.lives);
            mvprintw(1, COLS / 2 - 2, "Score: %d", score.score);
            mvprintw(1, COLS - 17, "High Score: %d", high_score);

            draw_bullets_player();
            // draw_bullets_enemy();
            draw_aliens();

            refresh();
        }
        else if (GAME_OVER)
        {
            free = 1;
            mask += 1;
            draw_game_over_screen();

            if (mask == 1)
            {
                add_new_score(score);
                save_high_scores("/home/jabel/Proyecto_SO/MatCom-Invasion/high_scores.txt");
            }
        }
        else if (PAUSE)
        {
            free = 1;
            mask += 2;
            draw_pause();
            if (mask == 2)
            {
                add_new_score(score);
                save_high_scores("/home/jabel/Proyecto_SO/MatCom-Invasion/high_scores.txt");
            }
        }
        else if (SEE_SCORES)
        {
            free = 1;
            draw_high_scores();
        }
        // else if (SELECT_GAME)
        // {
        //     draw_select_screen();
        // }
        // else if (ERROR)
        // {
        //     draw_load_error();
        // }
        // else if (SAVING_ERROR)
        // {
        //     draw_saving_error();
        // }

        unlock_master_mutex;

        usleep(DELAY);
        move_enemy = (move_enemy + 1) % 10;
    }
    return NULL;
}

#pragma endregion

#pragma region INPUT_HANDLER

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

void next_fit_bullet()
{

    int i = (ptr_type_bullet + 1) % (NUMBER_BULLETS + 5);

    for (int j = i; j != ptr_type_bullet; j = (j + 1) % (NUMBER_BULLETS + 5))
    {

        if (!bullets[j].active)
        {
            ptr_type_bullet = j;
            return;
        }
    }
}

void shoot()
{
    int number_bullets = 0;

    for (int i = 0; i < NUMBER_BULLETS + 5; i++)
    {
        if (bullets[i].active)
            number_bullets += 1;
    }
    if (number_bullets < NUMBER_BULLETS)
    {
        bullets[ptr_type_bullet].x = player.x + 2;
        bullets[ptr_type_bullet].y = player.y - 1;
        bullets[ptr_type_bullet].active = 1;
        next_fit_bullet();
    }
}

// Maneja la entrada del usuario para controlar el juego
void *input_handler(void *arg)
{
    int index = 0;
    int ch;
    int free = 0;
    while (!EXIT)
    {
        ch = getch();
        lock_master_mutex;

        if (START)
        {
            if (free)
            {
                index = 0;
                free = 0;
            }
            if (ch == KEY_DOWN)
                menu_item = (menu_item + 1) % 4;
            else if (ch == KEY_UP)
                menu_item = ((menu_item - 1) + 4) % 4;

            else if (ch == ' ')
            {
                switch (menu_item)
                {
                case 0:
                    actual_state = run;
                    init_game();
                    break;

                case 1:
                    actual_state = run;
                    init_game();
                    load_game("saved_game.dat");
                    break;

                case 2:
                    actual_state = see_scores;
                    break;

                case 3:
                    actual_state = out;
                    break;
                }
            }
            else if (isprint(ch))
            {
                score.name[index++] = ch;
            }
        }
        else if (RUN)
        {
            free = 1;
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
                actual_state = pause_game;
                break;
            }
        }
        else if (GAME_OVER)
        {
            free = 1;
            // if (ch == 's')
            // {
            //     actual_state = start;
            // }
            // else if (ch == 'q')
            // {
            //     actual_state = out;
            // }
            if (ch == KEY_DOWN)
                menu_game_over = (menu_game_over + 1) % 2;
            else if (ch == KEY_UP)
                menu_game_over = ((menu_game_over - 1) + 2) % 2;

            else if (ch == ' ')
            {
                switch (menu_game_over)
                {
                case 0:
                    actual_state = start;
                    break;

                case 1:
                    actual_state = out;
                    break;
                }
            }
        }
        else if (PAUSE)
        {
            // switch (ch)
            // {
            // case 'q':
            //     actual_state = out;
            //     break;
            // case 's':
            //     actual_state = start;
            //     break;
            // case 'c':
            //     save_game("saved_game.dat");
            //     actual_state = out;
            //     break;
            // case 'x': // Guardar y volver a inicio
            //     save_game("saved_game.dat");
            //     actual_state = start;
            //     break;
            // // case 'v':
            // //     saveGameToUSB();
            // //     if (!saved)
            // //         actual_state = saving_error;

            // //     else
            // //     {
            // //         actual_state = start;
            // //         saved = false;
            // //     }
            // //     break;
            // // case 'b': // Guardar en USB y salir
            // //     saveGameToUSB();
            // //     if (!saved)
            // //     {
            // //         actual_state = saving_error;
            // //     }
            // //     else
            // //     {
            // //         actual_state = out;
            // //         saved = false;
            // //     }
            // //     break;
            // case 'u': // Volver al juego
            //     actual_state = run;
            //     break;
            // }
            if (ch == KEY_DOWN)
                menu_pause = (menu_pause + 1) % 5;
            else if (ch == KEY_UP)
                menu_pause = ((menu_pause - 1) + 5) % 5;

            else if (ch == ' ')
            {
                switch (menu_pause)
                {
                case 0:
                    actual_state = start;
                    break;

                case 1:
                    actual_state = out;
                    break;
                case 2:
                    save_game("saved_game.dat");
                    actual_state = out;
                    break;
                case 3:
                    save_game("saved_game.dat");
                    actual_state = start;
                    break;
                case 4:
                    actual_state = run;
                    break;
                }
            }
        }
        // else if (ERROR)
        // {
        //     if (ch == 's')
        //     {
        //         actual_state = start;
        //     }
        //     else if (ch == 'q')
        //     {
        //         actual_state = out;
        //     }
        // }
        // else if (SAVING_ERROR)
        // {
        //     if (ch == 's')
        //     {
        //         actual_state = start;
        //     }
        //     else if (ch == 'q')
        //     {
        //         actual_state = out;
        //     }
        //     else if (ch == 'u')
        //     {
        //         actual_state = pause_game;
        //     }
        // }

        else if (SEE_SCORES)
        {
            free = 1;
            if (ch == 's')
            {
                actual_state = start;
            }
        }
        // else if (SELECT_GAME)
        // {
        //     if (ch == 's')
        //     {
        //         actual_state = start;
        //     }
        //     if (ch == 'p') // Cargar partida desde la memoria principal
        //     {
        //         actual_state = run;
        //         init_game();
        //         load_game("saved_game.dat");
        //         if (!loaded)
        //         {
        //             actual_state = error;
        //         }
        //         else
        //         {
        //             loaded = false;
        //         }
        //     }
        //     if (ch == 'q')
        //     {
        //         actual_state = out;
        //     }
        //     if (ch == 'u') // Cargar juego desde una usb
        //     {
        //         actual_state = run;
        //         init_game();
        //         loadGameFromUSB();
        //         if (!loaded)
        //         {
        //             actual_state = error;
        //         }
        //         else
        //         {
        //             loaded = false;
        //         }
        //     }
        // }

        unlock_master_mutex;
    }
    return NULL;
}

#pragma endregion

#pragma region MAIN

#define COLOR_GOLD 18
#define COLOR_SILVER 19
#define COLOR_BRONZE 20

int main()
{
    srand(time(NULL));
    initscr();
    noecho();
    curs_set(FALSE);
    keypad(stdscr, TRUE);
    timeout(0);
    start_color();

    init_color(COLOR_GOLD, 700, 550, 0);     // Oro
    init_color(COLOR_SILVER, 800, 800, 800); // Plata
    init_color(COLOR_BRONZE, 600, 400, 200); // Bronce

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