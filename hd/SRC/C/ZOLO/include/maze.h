#ifndef MAZE_H
#define MAZE_H

#include <16bittypes.h>

#define CELL_SIZE_PX 32
#define MAZE_WIDTH_CELLS 32
#define MAZE_HEIGHT_CELLS 32
#define VIEWPORT_WIDTH_PX 224
#define VIEWPORT_WIDTH_BYTES 112
#define VIEWPORT_HEIGHT_PX 192
#define LINE_SIZE_BYTES 160
#define CELL_WIDTH_BYTES 16
#define HWALL_END_SPRITES_Y 30
#define HWALL_START_SPRITES_Y 70
#define HWALL_FULL_SPRITE_Y 110

typedef struct 
{
  word viewport_height_px;
  word viewport_width_px;
  word cell_size_px;
  word hwall_end_sprites_y;
  word hwall_start_sprites_y;
  word hwall_full_sprite_y;
  word bytes_per_line;
} MazeRenderConf;

typedef struct {
  word maze_width_cells;
  word maze_height_cells;
} Mazeconf;

word** generate_maze(int rows, int cols);
void render_maze(bool mode, word** maze, word cx, word cy, Base* screenbase, void* spritebase, bool log);

#endif