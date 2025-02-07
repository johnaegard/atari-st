#ifndef MAZE_H
#define MAZE_H

#include <16bittypes.h>
#define MAZE_HEIGHT_CELLS 32
#define MAZE_WIDTH_CELLS 32
#define CELL_SIZE_PX 32
#define VIEWPORT_WIDTH_PX 224
#define VIEWPORT_WIDTH_BYTES 112
#define VIEWPORT_HEIGHT_PX 192
#define LINE_SIZE_BYTES 160
#define CELL_WIDTH_BYTES 16
#define HWALL_END_SPRITES_Y 30
#define HWALL_START_SPRITES_Y 70
#define HWALL_FULL_SPRITE_Y 110

#define ERASE_MODE false
#define DRAW_MODE true;

typedef struct {
  word viewport_height_px;
  word viewport_width_px;
  word cell_size_px;
  word hwall_end_sprites_y;
  word hwall_start_sprites_y;
  word hwall_full_sprite_y;
  word bytes_per_line;
} MazeRenderConf;

typedef struct {
  word** walls;
  word width_cells;
  word height_cells;
} Maze;

Maze generate_maze(word height, word width);
void render_maze(bool mode, Maze *maze, word cx, word cy, Page* page, Image* sprites, 
  bool log, FILE* logfile);

#endif