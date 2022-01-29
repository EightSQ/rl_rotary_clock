rotary_clock: main.c
	$(CC) -g -O0 -Wall -Werror -Wextra -Wpedantic `pkg-config --libs --cflags raylib` $^ -o $@
