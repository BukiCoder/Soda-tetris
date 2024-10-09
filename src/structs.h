#pragma once
#include <ncurses.h>
#include <time.h>
enum
{
    WIDTH = 10,
    HEIGHT = 20,
    R_WIDTH_SCORE = 22,
    R_HEIGHT_SCORE = 5,
    DEF_NUM_BLCKS = 4,
    Y_SPAWN_POS = 6,
    X_SPAWN_POS = 3,
    ERASE_COLOR = 1
};


struct game
{
       int is_game_over;
       int score;
       int speed;
       char grid[HEIGHT + 2][WIDTH + 2];
       char vmax[WIDTH + 2];
       char fill[HEIGHT + 2];
       WINDOW* main_window;
       WINDOW* score_window;
       time_t start_time;
};

struct figure_state
{
       int ypos;
       int xpos;
       int curr_figure;
       int rotation;
       int shadow_xpos;
       int shadow_ypos;//HEIGHT

};
