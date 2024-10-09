
#include "grid_utils.h"
#include "pallets.h"
#include <ncurses.h>

typedef struct game GAME;
typedef struct figure_state FIGURE_STATE;

void moveSide(GAME* game, FIGURE_STATE* figure, int add, char moveSideOn_curr[8][4][8], char moveSideOff_curr[8][4][8])
{
    char* currOn = moveSideOn_curr[figure->curr_figure][figure->rotation];
    char* currOff = moveSideOff_curr[figure->curr_figure][figure->rotation];
    char* currPiece = pieces[figure->curr_figure][figure->rotation];

    int xpos = figure->xpos;
    int ypos = figure->ypos;
    int curr_figure = figure->curr_figure;

    if(!can_place(game, ypos, xpos + add, currOn, moveSideNum[curr_figure][figure->rotation]) )
    {
        return;
    }

    draw(game, xpos, ypos, currOff, 1, moveSideNum[curr_figure][figure->rotation]);
    draw(game, xpos, figure->shadow_ypos, currPiece, 1, DEF_NUM_BLCKS);

    figure->xpos += add;
    
    figure->shadow_ypos = ypos + get_distance_higher(game, ypos, figure->xpos, currPiece) - 1;

    if(figure->shadow_ypos < ypos)
    {
        figure->shadow_ypos = ypos + get_distance_lower(game, ypos, figure->xpos,  currPiece, DEF_NUM_BLCKS) - 1;
    }
    if(figure->shadow_ypos != ypos) draw_shadow(game, figure->xpos, figure->shadow_ypos, currPiece, curr_figure + 1, DEF_NUM_BLCKS);

    draw(game, figure->xpos, ypos, currPiece, curr_figure + 1, DEF_NUM_BLCKS);

    wrefresh(game->main_window);

}
void moveRight(GAME* game, FIGURE_STATE* figure)
{
    moveSide(game, figure, 1, moveSideOn, moveSideOff);
}

void moveLeft(GAME* game, FIGURE_STATE* figure)
{
    moveSide(game, figure, -1, moveSideOff, moveSideOn);
}

void rotate(GAME* game, FIGURE_STATE* figure)
{
    int xpos = figure->xpos;
    int ypos = figure->ypos;
    int curr_figure = figure->curr_figure;
    int r_temp = (figure->rotation + 1) % 4;
    char* temp_piece = pieces[curr_figure][r_temp];
    char* curr_piece = pieces[curr_figure][figure->rotation];

    if(!can_place(game, ypos, xpos, temp_piece, DEF_NUM_BLCKS) )
    {
        return;
    }
    figure->rotation = r_temp;
    draw(game, xpos, ypos, curr_piece, 1, DEF_NUM_BLCKS);
    draw(game, xpos, figure->shadow_ypos, curr_piece, 1, DEF_NUM_BLCKS);

    figure->shadow_ypos = ypos + get_distance_higher(game, ypos, xpos, temp_piece) - 1;

    if(figure->shadow_ypos < ypos)
    {
        figure->shadow_ypos = ypos + get_distance_lower(game, ypos, xpos,  temp_piece, DEF_NUM_BLCKS) - 1;
    }

    if(figure->shadow_ypos != ypos) draw_shadow(game, xpos, figure->shadow_ypos, temp_piece, curr_figure + 1, DEF_NUM_BLCKS);
    draw(game, xpos, ypos, temp_piece, curr_figure + 1, DEF_NUM_BLCKS);

    wrefresh(game->main_window);
}

void drop(GAME* game, FIGURE_STATE* figure)
{
    int xpos = figure->xpos;
    int ypos = figure->ypos;
    int curr_figure = figure->curr_figure;
    char* curr_piece = pieces[curr_figure][figure->rotation];

    draw(game, xpos, ypos, curr_piece, 1, 4);

    ypos = figure->shadow_ypos;
    draw(game, xpos, ypos, curr_piece, curr_figure + 1, DEF_NUM_BLCKS);


    for (int i = 0; i < 4; i++)
    {
        game->fill[ypos + curr_piece[i * 2 + 1]]++;
        game->vmax[xpos + curr_piece[i * 2]] = (game->vmax[xpos + curr_piece[i * 2]] > (ypos + curr_piece[i * 2 + 1]))? (ypos + curr_piece[i * 2 + 1]) : game->vmax[xpos + curr_piece[i * 2]];
        game->grid[ypos + curr_piece[i * 2 + 1]][xpos + curr_piece[i * 2]] = curr_figure;
    }

    shift_lines(game, ypos);
    spawn_figure(game, figure, X_SPAWN_POS, Y_SPAWN_POS);
    wrefresh(game->main_window);
}

void moveDown(GAME* game, FIGURE_STATE* figure)
{
   
    int xpos = figure->xpos;
    int ypos = figure->ypos;
    int curr_figure = figure->curr_figure;
    int rotation = figure->rotation;
    int down_num = moveDownNum[curr_figure][rotation];
    char* curr_downOn = moveDownOn[curr_figure][rotation];
    char* curr_downOff = moveDownOff[curr_figure][rotation];
    char* curr_piece = pieces[curr_figure][rotation];


    if (!can_place(game, ypos + 1, xpos, curr_downOn, down_num))
    {
        
        for (int i = 0; i < 4; i++)
        {
            game->fill[ypos + curr_piece[i * 2 + 1]]++;
            game->vmax[xpos + curr_piece[i * 2]] = (game->vmax[xpos + curr_piece[i * 2]] > (ypos + curr_piece[i * 2 + 1]))? (ypos + curr_piece[i * 2 + 1]) : game->vmax[xpos + curr_piece[i * 2]];
            game->grid[ypos + curr_piece[i * 2 + 1]][xpos + curr_piece[i * 2]] = curr_figure;
        }

        shift_lines(game, ypos);
        spawn_figure(game, figure, X_SPAWN_POS, Y_SPAWN_POS);
    }
    else
    {
        draw(game, xpos, ypos, curr_downOff, 1, down_num);
        figure->ypos++;
        draw(game, xpos, ypos + 1, curr_downOn, curr_figure + 1, down_num);
    }

    wrefresh(game->main_window);
}