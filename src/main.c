#define _XOPEN_SOURCE 700
#include "grid_utils.h"
#include <errno.h>
#include <time.h>
#include "figure_move.h"
#include <curses.h>
#include <locale.h>
#include "winutils.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>
#include <ncurses.h>

typedef struct game GAME;
typedef struct figure_state FIGURE_STATE;





pthread_t timer;

void init()
{
    setlocale(LC_ALL, "");
    srand(time(NULL)); // Initialization, should only be called once.
    initscr();         /* Start curses mode 		*/
    cbreak();
    nodelay(stdscr, TRUE);
    noecho();
    keypad(stdscr, TRUE);
    start_color();
    curs_set(0);
}

enum 
{
    ORANGE_R = 765,
    ORANGE_G = 280 + 140,
    ORANGE_B = 0,
};

void init_colors()
{
    if(can_change_color())
    {
        init_color(9, ORANGE_R, ORANGE_G, ORANGE_B);
        init_pair(3, 9, 9);
        init_pair(3 + 8, 9, COLOR_BLACK);
    }
    else
    {
        init_pair(3, COLOR_WHITE, COLOR_WHITE);
        init_pair(3 + 8, COLOR_WHITE, COLOR_BLACK);
    }

    init_pair(0, COLOR_WHITE, COLOR_BLACK);
    init_pair(1, COLOR_BLACK, COLOR_BLACK);
    init_pair(2, COLOR_CYAN, COLOR_CYAN);
    init_pair(4, COLOR_MAGENTA, COLOR_MAGENTA);
    init_pair(5, COLOR_YELLOW, COLOR_YELLOW);
    init_pair(6, COLOR_BLUE, COLOR_BLUE);
    init_pair(7, COLOR_GREEN, COLOR_GREEN);
    init_pair(8, COLOR_RED, COLOR_RED);

    init_pair(2 + 8, COLOR_CYAN, COLOR_BLACK);
    init_pair(5 + 8, COLOR_YELLOW, COLOR_BLACK);
    init_pair(7 + 8, COLOR_GREEN, COLOR_BLACK);
    init_pair(4 + 8, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(6 + 8, COLOR_BLUE, COLOR_BLACK);
    init_pair(8 + 8, COLOR_RED, COLOR_BLACK);

}

void set_main_border(GAME* game)
{
   for (int i = 0; i < HEIGHT + 2; i++)
    {
        mvwaddwstr(game->main_window, i, 1, L"\u2588");
        mvwaddwstr(game->main_window, i, (WIDTH + 2) * 2 - 2, L"\u2588");
        game->grid[i][0] = 1;
        game->grid[i][WIDTH + 1] = 1;
    }

    for (int i = 1; i < WIDTH * 2 + 3; i++)
    {
        mvwaddwstr(game->main_window, 0, i, L"\u2584");
        mvwaddwstr(game->main_window, HEIGHT + 1, i, L"\u2580");
        game->grid[HEIGHT + 1][i >> 1] = 1;
    }
}


void* print_time(void* arg)
{
    GAME* game= (GAME*)(arg);
    while(!game->is_game_over){
        time_t diff = time(NULL) - game->start_time;
        char buffer[20];

        struct tm curr_time = {0};;
        gmtime_r(&diff, &curr_time);
        strftime(buffer, 20, "%H:%M:%S", &curr_time);
 
        sleep(1);
        mvwprintw(game->score_window, 1, 2, "%s %s", "Time:  ",  buffer);
        wrefresh(game->score_window);
    }
    return NULL;
}

int64_t millis()
{
    struct timespec now;
    timespec_get(&now, TIME_UTC);
    return ((int64_t) now.tv_sec) * 1000 + ((int64_t) now.tv_nsec) / 1000000;
}
int main(int argc, char *argv[])
{
    init();
    init_colors();
    
   
    int in_ch;
    int starty = (LINES - HEIGHT - 2) / 2;	/* Calculating for a center placement */
    int startx = (COLS -  WIDTH * 2 - 4) / 2;	/* of the window		*/


    WINDOW *main_win = create_newwin(HEIGHT + 2, WIDTH * 2 + 4, starty, startx);
    

    WINDOW *score_win = create_newwin(R_HEIGHT_SCORE, R_WIDTH_SCORE, starty, startx + WIDTH * 2 + 4 + 4);
    box(score_win, 0, 0);

    mvwprintw(score_win, 1, 2, "%s", "Time:   00:00:00");
    mvwprintw(score_win, 2, 2, "%s %d", "Score: ", 0);

    refresh();
    wrefresh(main_win);
    wrefresh(score_win);

    GAME curr_game = {0};
    curr_game.main_window = main_win;
    curr_game.speed = 800;
    curr_game.score_window = score_win;
    memset(&curr_game.vmax, 21, WIDTH + 2);
  
    set_main_border(&curr_game);
   
    pthread_create(&timer, NULL, print_time, &curr_game);
 
    curr_game.start_time = time(NULL);

    FIGURE_STATE curr_figure = {0};
    
LL:
 
    spawn_figure(&curr_game, &curr_figure, X_SPAWN_POS, Y_SPAWN_POS);
   
    wrefresh(curr_game.main_window);
    int64_t curr_mills = millis();
    int direction = 0;

    while ((in_ch = getch()) != KEY_F(1) && !curr_game.is_game_over)

    {

        if (millis() - curr_mills > curr_game.speed)
        {
            curr_mills = millis();
            moveDown(&curr_game, &curr_figure);
        }

        switch (in_ch)
        {
        case KEY_LEFT:
            if (direction == -1)
            {
                napms(50);
            }
            moveLeft(&curr_game, &curr_figure);
            direction = 1;
            napms(35);
            flushinp();
            break;
        case KEY_RIGHT:
            if (direction == 1)
            {
                napms(50);
            }
            moveRight(&curr_game, &curr_figure);
            direction = -1;
            napms(35);
            flushinp();
            break;

        case KEY_DOWN:

            flushinp();
            moveDown(&curr_game, &curr_figure);

            napms(30);

            break;
        case KEY_UP:

            rotate(&curr_game, &curr_figure);

            break;
        case ' ':
            drop(&curr_game, &curr_figure);
            break;
        }
    }

    if(curr_game.is_game_over)
    {
        WINDOW *over_win = create_newwin(7, WIDTH * 2 + 20, starty + HEIGHT / 2, startx - 9);
        box(over_win, 0, 0);
        mvwprintw(over_win, 1, 2, "%s", "Game over! Are you want play again?");
        mvwaddch(over_win, 4, 2, 'Y' | A_UNDERLINE);
        mvwprintw(over_win, 4, 3, "%s", "es!");
        mvwaddch(over_win, 4, WIDTH * 2 + 13, 'N' | A_UNDERLINE);
        mvwprintw(over_win, 4, WIDTH * 2 + 14, "%s", "o :(");


        wrefresh(over_win);
        while((in_ch = getch()) != KEY_F(1))
        {
            switch(in_ch)
            {
            case 'n':
                endwin();
                return 0;
            case 'y':
                // destroy_win(main_win);
                // refresh();
                pthread_join(timer, NULL);
                werase(over_win);
                wrefresh(over_win);
                werase(curr_game.main_window);
                destroy_win(over_win);
                refresh();
                curr_figure.xpos = X_SPAWN_POS;
                curr_figure.ypos = Y_SPAWN_POS;
                curr_game.score = 0;
                curr_game.is_game_over = 0;
                wclear(score_win);
                box(score_win, 0, 0);
                mvwprintw(score_win, 1, 2, "%s", "Time:   00:00:00");
                mvwprintw(score_win, 2, 2, "%s", "Score:  0");
                wrefresh(score_win);
                memset(curr_game.vmax, 21, WIDTH + 2);
                memset(curr_game.grid, 0, (HEIGHT + 2) * (WIDTH + 2));
                memset(curr_game.fill, 0, HEIGHT + 2);
                curr_game.speed = 800;
                wattrset(curr_game.main_window, COLOR_PAIR(0));
                set_main_border(&curr_game);
                curr_game.start_time = time(NULL);
                pthread_create(&(timer), NULL, print_time, &curr_game);
                wrefresh(curr_game.main_window);
                refresh();
                goto LL;


                break;
            }

        }
    }
    endwin();

    getch();
    return 0;
}


