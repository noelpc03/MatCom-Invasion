#ifndef TYPES_H
#define TYPES_H

// Game states
typedef enum {
    STATE_START,
    STATE_RUN,
    STATE_GAME_OVER,
    STATE_OUT,
    STATE_INFO,
    STATE_PAUSE,
    STATE_SEE_SCORES,
    STATE_ERROR,
    STATE_SELECT_GAME,
    STATE_SAVING_ERROR
} GameState;

// State checking macros
#define EXIT (actual_state == STATE_OUT)
#define START (actual_state == STATE_START)
#define RUN (actual_state == STATE_RUN)
#define GAME_OVER (actual_state == STATE_GAME_OVER)
#define PAUSE (actual_state == STATE_PAUSE)
#define SEE_SCORES (actual_state == STATE_SEE_SCORES)
#define ERROR (actual_state == STATE_ERROR)
#define SELECT_GAME (actual_state == STATE_SELECT_GAME)
#define SAVING_ERROR (actual_state == STATE_SAVING_ERROR)

// Game entities
typedef struct {
    int x, y, active;
} Bullet;

typedef struct {
    int x, y, active, type;
    int index_frame;
} Alien;

typedef struct {
    int x, y, lives;
} Player;

typedef struct {
    int last_time, active;
    Alien alien;
} Frame;

typedef struct {
    int score;
    char name[20];
    char date[40];
} Score;

#endif // TYPES_H
