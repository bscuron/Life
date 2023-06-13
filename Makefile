CC=gcc
CFLAGS=-Wall -Werror -pedantic -lncurses

life: life.c
	$(CC) $(CFLAGS) -o life life.c
