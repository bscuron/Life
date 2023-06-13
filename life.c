#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <ncurses.h>
#include <time.h>
#define PROBABILITY 0.2
#define DELAY 10000

int quit = 0;

struct winsize get_win() {
    struct winsize ws;
    if(ioctl(STDIN_FILENO, TIOCGWINSZ, &ws) == -1) {
        fprintf(stderr, "ERROR: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    return ws;
}

void init(int *board, int rows, int cols) {
    for(int i = 0; i < rows; i++) {
        for(int j = 0;j < cols; j++) {
            board[i * cols + j] = (float)rand()/RAND_MAX <= PROBABILITY ? 1 : 0;
        }
    }
}

void ncurses_deinit() {
    endwin();
}

void ncurses_init() {
    initscr();
    cbreak();
    nodelay(stdscr, true);
    noecho();
    intrflush(stdscr, false);
    keypad(stdscr, true);
    curs_set(0);
    if(has_colors() == FALSE) {
        ncurses_deinit();
        fprintf(stderr, "ERROR: your terminal does not support colors\n");
        exit(EXIT_FAILURE);
	}
	start_color();
    init_pair(1, COLOR_GREEN, COLOR_GREEN);
}

void render(int *board, int rows, int cols) {
    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < cols; j++) {
            if(!board[i * cols + j]) {
                use_default_colors();
                mvprintw(i, j, "%c", ' ');
            } else {
                attron(COLOR_PAIR(1));
                mvprintw(i, j, "%c", ' ');
                attroff(COLOR_PAIR(1));
            }
        }
    }
    refresh();
}

void compute_generation(int *board, int rows, int cols) {
    int tboard[rows * cols];
    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < cols; j++) {
            int k = i * cols + j, n = 0;
            if(i > 0 && board[(i - 1) * cols + j]) n++;
            if(i > 0 && j < cols - 1 && board[(i - 1) * cols + (j + 1)]) n++;
            if(j < cols - 1 && board[i * cols + (j + 1)]) n++;
            if(i < rows - 1 && j < cols - 1 && board[(i + 1) * cols + (j + 1)]) n++;
            if(i < rows - 1 && board[(i + 1) * cols + j]) n++;
            if(i < rows - 1 && j > 0 && board[(i + 1) * cols + (j - 1)]) n++;
            if(j > 0 && board[i * cols + (j - 1)]) n++;
            if(i > 0 && j > 0 && board[(i - 1) * cols + (j - 1)]) n++;
            
            switch(board[k]) {
            case 1:
                if(n == 2 || n == 3) tboard[k] = 1;
                else tboard[k] = 0;
                break;
            case 0:
                if(n == 3) tboard[k] = 1;
                else tboard[k] = 0;
                break;
            }
        }
    }
    memcpy(board, tboard, sizeof(int) * rows * cols);
}

void handle_keypress(int *board, int rows, int cols) {
    char c = wgetch(stdscr);
    switch(c) {
    case 'q':
        quit = 1;
        break;
    case ' ':
        init(board, rows, cols);
        break;
    }
}

int main(void) {
    struct winsize ws = get_win();
    int rows = ws.ws_row, cols = ws.ws_col;
    int board[rows * cols];
    init(board, rows, cols);

    ncurses_init();
    srand(time(NULL));
    while(!quit) {
        render(board, rows, cols);
        compute_generation(board, rows, cols);
        handle_keypress(board, rows, cols);
        usleep(DELAY);
    }
    ncurses_deinit();
    return 0;
}
