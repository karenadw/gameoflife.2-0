#ifndef GAME_OF_LIFE_H
#define GAME_OF_LIFE_H

#include <stdio.h>

#define WIDTH 80
#define HEIGHT 25

#define ALIVE 1
#define DEAD 0

#define CHAR_ALIVE '#'
#define CHAR_DEAD  '.'

#define SPEED_MIN_MS 50
#define SPEED_MAX_MS 1000
#define SPEED_DEFAULT_MS 200

/*
 * Состояние игры.
 * field       - текущее поколение
 * initial     - начальная конфигурация (для рестарта)
 * generation  - номер поколения
 * alive_count - число живых клеток
 * speed_ms    - задержка между поколениями
 * paused      - 0 = играем, 1 = пауза
 */
typedef struct {
    int field[HEIGHT][WIDTH];
    int initial[HEIGHT][WIDTH];
    int generation;
    int alive_count;
    int speed_ms;
    int paused;
} Game;

void init_game(Game *game);
int  load_field(Game *game, FILE *src);
void restart_game(Game *game);

int  count_neighbors(const Game *game, int row, int col);
int  next_cell_state(int state, int neighbors);
int  count_alive(const Game *game);
void update_field(Game *game);

void draw_field(const Game *game);
void draw_status(const Game *game);
void change_speed(Game *game, int delta);

void game_loop(Game *game);

#endif
