#include "game_of_life.h"

void init_game(Game *game)
{
    int row = 0;
    int col;

    while (row < HEIGHT) {
        col = 0;
        while (col < WIDTH) {
            game->field[row][col] = DEAD;
            game->initial[row][col] = DEAD;
            col++;
        }
        row++;
    }
    game->generation = 0;
    game->alive_count = 0;
    game->speed_ms = SPEED_DEFAULT_MS;
    game->paused = 0;
}

int load_field(Game *game, FILE *src)
{
    int row = 0;
    int col = 0;
    int c;

    while (row < HEIGHT) {
        c = fgetc(src);
        if (c == EOF)
            return (1);
        if (c != '\n') {
            if (c == CHAR_ALIVE)
                game->field[row][col] = ALIVE;
            else
                game->field[row][col] = DEAD;
            col++;
            if (col == WIDTH) {
                col = 0;
                row++;
            }
        }
    }
    row = 0;
    while (row < HEIGHT) {
        col = 0;
        while (col < WIDTH) {
            game->initial[row][col] = game->field[row][col];
            col++;
        }
        row++;
    }
    game->alive_count = count_alive(game);
    return (0);
}

void restart_game(Game *game)
{
    int row = 0;
    int col;

    while (row < HEIGHT) {
        col = 0;
        while (col < WIDTH) {
            game->field[row][col] = game->initial[row][col];
            col++;
        }
        row++;
    }
    game->generation = 0;
    game->alive_count = count_alive(game);
    game->paused = 0;
}

int count_neighbors(const Game *game, int row, int col)
{
    int count = 0;
    int dr = -1;
    int dc;
    int nr;
    int nc;

    while (dr <= 1) {
        dc = -1;
        while (dc <= 1) {
            if (dr != 0 || dc != 0) {
                nr = (row + dr + HEIGHT) % HEIGHT;
                nc = (col + dc + WIDTH) % WIDTH;
                count += game->field[nr][nc];
            }
            dc++;
        }
        dr++;
    }
    return (count);
}

int next_cell_state(int state, int neighbors)
{
    if (state == ALIVE)
        return (neighbors == 2 || neighbors == 3);
    return (neighbors == 3);
}

int count_alive(const Game *game)
{
    int row = 0;
    int col;
    int total = 0;

    while (row < HEIGHT) {
        col = 0;
        while (col < WIDTH) {
            total += game->field[row][col];
            col++;
        }
        row++;
    }
    return (total);
}

void update_field(Game *game)
{
    int next[HEIGHT][WIDTH];
    int row = 0;
    int col;

    while (row < HEIGHT) {
        col = 0;
        while (col < WIDTH) {
            next[row][col] = next_cell_state(
                game->field[row][col],
                count_neighbors(game, row, col));
            col++;
        }
        row++;
    }
    row = 0;
    while (row < HEIGHT) {
        col = 0;
        while (col < WIDTH) {
            game->field[row][col] = next[row][col];
            col++;
        }
        row++;
    }
    game->generation++;
    game->alive_count = count_alive(game);
}
