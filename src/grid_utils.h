#pragma once 
#include "structs.h"

typedef struct game GAME;
typedef struct figure_state FIGURE_STATE;
void spawn_figure(GAME* game, FIGURE_STATE* prev, int xpos_spawn, int ypos_spawn);
void color_block(GAME *game, int y, int x, int color);
void add_shadow_block(GAME *game, int y, int x, int color);
void shift_line(GAME *game, int y_from, int y_to);
void clear_line(GAME *game, int y);
void draw(GAME *game, int x, int y, char *currPiece, int color, int size);
void draw_shadow(GAME *game, int x, int y, char *currPiece, int color, int size);
int get_distance_higher(GAME *game, int y, int x, char* currPiece);
int can_place(GAME *game, int y, int x, char* currPiece, int size);
int get_distance_lower(GAME *game, int y, int x, char* currPiece, int size);
void shift_lines(GAME *game, int ypos);

