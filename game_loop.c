#if defined(_WIN32)
# include <ncurses/ncurses.h>
#else
# include <ncurses.h>
#endif

#include "game_of_life.h"

#define SPEED_STEP_MS 50
#define KEY_QUIT      ' '
#define KEY_QUIT_ALT  'q'
#define KEY_PAUSE     'p'
#define KEY_RESTART   'r'
#define KEY_FASTER    'a'
#define KEY_SLOWER    'z'

#define CP_ALIVE 1
#define CP_DEAD  2
#define CP_STATS 3
#define CP_HINT  4
#define CP_FRAME 5

static void init_screen(void)
{
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
}

static void close_screen(void)
{
    endwin();
}

static void init_colors(void)
{
    if (has_colors()) {
        start_color();
        use_default_colors();
        init_pair(CP_ALIVE, COLOR_GREEN,  -1);
        init_pair(CP_DEAD,  COLOR_BLUE,   -1);
        init_pair(CP_STATS, COLOR_CYAN,   -1);
        init_pair(CP_HINT,  COLOR_WHITE,  -1);
        init_pair(CP_FRAME, COLOR_YELLOW, -1);
    }
}

static void show_intro(void)
{
    int mid_y = HEIGHT / 2 + 1;
    int mid_x = WIDTH / 2;

    clear();
    attron(COLOR_PAIR(CP_FRAME) | A_BOLD);
    mvprintw(mid_y - 4, mid_x - 18, "+--------------------------------+");
    mvprintw(mid_y - 3, mid_x - 18, "|         GAME OF LIFE           |");
    mvprintw(mid_y - 2, mid_x - 18, "+--------------------------------+");
    attroff(COLOR_PAIR(CP_FRAME) | A_BOLD);

    attron(COLOR_PAIR(CP_HINT));
    mvprintw(mid_y + 0, mid_x - 22, "Rules: a live cell with 2 or 3 neighbors");
    mvprintw(mid_y + 1, mid_x - 22, "survives; a dead cell with exactly 3");
    mvprintw(mid_y + 2, mid_x - 22, "neighbors becomes alive.");
    mvprintw(mid_y + 4, mid_x - 22, "The field is wrapped as a torus:");
    mvprintw(mid_y + 5, mid_x - 22, "left edge touches right, top touches bottom.");
    attroff(COLOR_PAIR(CP_HINT));

    attron(COLOR_PAIR(CP_STATS) | A_BOLD);
    mvprintw(mid_y + 8, mid_x - 12, "Press any key...");
    attroff(COLOR_PAIR(CP_STATS) | A_BOLD);

    refresh();
    timeout(-1);
    getch();
    timeout(0);
}

static void draw_frame(void)
{
    int i;

    attron(COLOR_PAIR(CP_FRAME));
    mvaddch(0, 0, ACS_ULCORNER);
    i = 1;
    while (i <= WIDTH) {
        mvaddch(0, i, ACS_HLINE);
        i++;
    }
    mvaddch(0, WIDTH + 1, ACS_URCORNER);

    i = 1;
    while (i <= HEIGHT) {
        mvaddch(i, 0, ACS_VLINE);
        mvaddch(i, WIDTH + 1, ACS_VLINE);
        i++;
    }

    mvaddch(HEIGHT + 1, 0, ACS_LLCORNER);
    i = 1;
    while (i <= WIDTH) {
        mvaddch(HEIGHT + 1, i, ACS_HLINE);
        i++;
    }
    mvaddch(HEIGHT + 1, WIDTH + 1, ACS_LRCORNER);
    attroff(COLOR_PAIR(CP_FRAME));
}

void draw_field(const Game *game)
{
    int row = 0;
    int col;

    while (row < HEIGHT) {
        col = 0;
        while (col < WIDTH) {
            if (game->field[row][col] == ALIVE) {
                attron(COLOR_PAIR(CP_ALIVE) | A_BOLD);
                mvaddch(row + 1, col + 1, CHAR_ALIVE);
                attroff(COLOR_PAIR(CP_ALIVE) | A_BOLD);
            } else {
                attron(COLOR_PAIR(CP_DEAD));
                mvaddch(row + 1, col + 1, CHAR_DEAD);
                attroff(COLOR_PAIR(CP_DEAD));
            }
            col++;
        }
        row++;
    }
}

void draw_status(const Game *game)
{
    attron(COLOR_PAIR(CP_STATS) | A_BOLD);
    mvprintw(HEIGHT + 3, 1,
        "Gen: %-6d  Alive: %-6d  Delay: %-5d ms  %s   ",
        game->generation, game->alive_count, game->speed_ms,
        game->paused ? "[PAUSED]" : "        ");
    attroff(COLOR_PAIR(CP_STATS) | A_BOLD);

    attron(COLOR_PAIR(CP_HINT));
    mvprintw(HEIGHT + 4, 1,
        "[A] faster  [Z] slower  [P] pause  [R] restart  [Q/Space] quit");
    attroff(COLOR_PAIR(CP_HINT));

    refresh();
}

void change_speed(Game *game, int delta)
{
    game->speed_ms -= delta * SPEED_STEP_MS;
    if (game->speed_ms < SPEED_MIN_MS)
        game->speed_ms = SPEED_MIN_MS;
    if (game->speed_ms > SPEED_MAX_MS)
        game->speed_ms = SPEED_MAX_MS;
}

static void handle_key(Game *game, int key)
{
    if (key == KEY_FASTER || key == KEY_FASTER - 32)
        change_speed(game, 1);
    if (key == KEY_SLOWER || key == KEY_SLOWER - 32)
        change_speed(game, -1);
    if (key == KEY_PAUSE || key == KEY_PAUSE - 32)
        game->paused = !game->paused;
    if (key == KEY_RESTART || key == KEY_RESTART - 32)
        restart_game(game);
}

void game_loop(Game *game)
{
    int key = ERR;

    init_screen();
    init_colors();
    show_intro();

    while (key != KEY_QUIT && key != KEY_QUIT_ALT) {
        clear();
        draw_frame();
        draw_field(game);
        draw_status(game);

        if (!game->paused)
            update_field(game);

        timeout(game->speed_ms);
        key = getch();
        handle_key(game, key);
    }
    close_screen();
}
