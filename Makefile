CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11
LDFLAGS = -lncursesw

NAME = game_of_life
SRC = main.c game_of_life.c game_loop.c
OBJ = $(SRC:.c=.o)
HDR = game_of_life.h

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME) $(LDFLAGS)

%.o: %.c $(HDR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
