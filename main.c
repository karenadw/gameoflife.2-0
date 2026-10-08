#include "game_of_life.h"

int main(void)
{
    Game game;

    init_game(&game);
    if (load_field(&game, stdin) != 0)
        return (1);
    game_loop(&game);
    return (0);
}
