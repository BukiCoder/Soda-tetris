
#define _XOPEN_SOURCE 700
#include "structs.h"
#include <ncurses.h>
#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>

typedef struct game GAME;
typedef struct figure_state FIGURE_STATE;

extern char pieces[8][4][8];

void color_block(GAME *game, int y, int x, int color_index)
{
    mvwchgat(game->main_window, y, x << 1, 2, COLOR_PAIR(color_index) | A_REVERSE, color_index, NULL);
}

void add_shadow_block(GAME* game, int y, int x, int color)
{
    wattron(game->main_window, COLOR_PAIR(color + 8));
    mvwaddwstr(game->main_window, y, x << 1, L"\u2592\u2592");

}
void shift_line(GAME* game, int y_from, int y_to)
{
    game->fill[y_to] = game->fill[y_from];

    for (int i = 1; i < WIDTH + 1; i++)
    {
        color_block(game, y_to, i, game->grid[y_from][i] + 1);
        game->grid[y_to][i] = game->grid[y_from][i];
    }
}

void clear_line(GAME* game, int y)
{
    for (int i = 1; i < WIDTH - 1; i++)
    {
        (game->grid)[y][i] = 0;
        color_block(game, y, i, ERASE_COLOR);
    }
}



void draw(GAME* game, int x, int y, char *curr_piece, int color, int size)
{
    for (int i = 0; i < size; i++)
    {
        color_block(game, y + curr_piece[i * 2 + 1], x + curr_piece[i * 2], color);
    }
}

void draw_shadow(GAME* game, int x, int y, char *curr_piece, int color, int size)
{
    for (int i = 0; i < size; i++)
    {
        add_shadow_block(game, y + curr_piece[i * 2 + 1], x + curr_piece[i * 2], color);
    }
}

int get_distance_higher(GAME* game, int y, int x, char* curr_piece)
{
    int min_height = WIDTH * 2 + 2;
    for (int i = 0; i < 4; i++)
    {
        int height = game->vmax[x + curr_piece[i * 2]] - (y + curr_piece[i * 2 + 1]) ;
       
        if (min_height > height)
        {
            min_height = height;
        }
    }
    return min_height;
}

int can_place(GAME* game, int y, int x, char* curr_piece, int size)
{
    for (int i = 0; i < size; i++)
    {
        if (game->grid[y + curr_piece[i * 2 + 1]][x + curr_piece[i * 2]] != 0)
        {
            return 0;
        }
    }
    return 1;
}

int get_distance_lower(GAME* game, int y, int x, char* curr_piece, int size)
{
    int min = INT_MAX;
    for(int i = 0; i < size; i++)
    {
        int typos = y + curr_piece[i * 2 + 1];
        int txpos = x + curr_piece[i * 2];
        for(int j = typos; j < HEIGHT + 2; j++)
        {
            if(game->grid[j][txpos] != 0)
            {
                if(min > j - typos)
                {
                    min = j - typos;
                }
                break;
            }

        }
    }
    return min;
}



void shift_lines(GAME* game, int ypos)
{
    char shift_lines_m[4];
    int shn = 0;

    for (int k = 3; k > -3; k--)
    {
        if ((ypos + k < HEIGHT + 2) && (ypos + k > -1) && game->fill[ypos + k] == WIDTH)
        {

            shift_lines_m[shn] = ypos + k;
            shn++;
        }
    }

    for (int k = 0; k < shn - 1; k++)
    {
        for (int kk = shift_lines_m[k] - 1; kk > shift_lines_m[k + 1]; kk--)
        {
            shift_line(game, kk, kk + k + 1);
        }
    }

    if (shn > 0)
    {
        for (int kk = shift_lines_m[shn - 1] - 1; kk > 0; kk--)
        {
            shift_line(game, kk, kk + shn);
        }
    }
    for (int k = 0; k < WIDTH + 2; k++)
    {
        game->vmax[k] += shn;
    }

    game->score += shn * 100;
    if(game->speed > 50) game->speed -= 10;

    mvwprintw(game->score_window, 2, 2, "%s %d", "Score: ", game->score);
    wrefresh(game->score_window);

}

void spawn_figure(GAME* game, FIGURE_STATE* prev, int xpos_spawn, int ypos_spawn)
{
    prev->ypos = xpos_spawn;
    prev->xpos = ypos_spawn;
    prev->shadow_ypos = HEIGHT;
    prev->rotation = 0;
    int randn = 1 + rand() % 7;
    if(prev->curr_figure == randn) randn = 1 + rand() % 7;
    prev->curr_figure = randn; // Returns a pseudo-random integer between 0 and RAND_MAX.

    
    char* curr_piece = pieces[prev->curr_figure][prev->rotation];
    if(!can_place(game, prev->ypos, prev->xpos, curr_piece, DEF_NUM_BLCKS))
    {
        game->is_game_over = 1;
        return;
    }

    prev->shadow_ypos = prev->ypos + get_distance_higher(game, prev->ypos, prev->xpos, curr_piece) - 1;

    draw_shadow(game, prev->xpos, prev->shadow_ypos, curr_piece, prev->curr_figure + 1, DEF_NUM_BLCKS);
    draw(game, prev->xpos, prev->ypos, curr_piece, prev->curr_figure + 1, DEF_NUM_BLCKS);
}
