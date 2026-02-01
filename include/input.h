#ifndef INPUT_H
#define INPUT_H

// Input handling thread
void *input_handler(void *arg);

// Player control functions
void move_player(int direction);
void shoot(void);
void next_fit_bullet(void);

#endif // INPUT_H
