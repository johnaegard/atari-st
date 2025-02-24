#ifndef MAZE_H
#define MAZE_H

#include <16bittypes.h>

#define LINE_SIZE_BYTES 160
#define CELL_WIDTH_BYTES 16
#define HWALL_END_SPRITES_Y 30
#define HWALL_START_SPRITES_Y 70
#define HWALL_FULL_SPRITE_Y 110

#define MAZE_ERASE_MODE false
#define MAZE_DRAW_MODE true

typedef struct {
  word viewport_height_px;
  word viewport_width_px;
  word viewport_width_bytes;
  word cell_size_px;
  word hwall_end_sprites_y;
  word hwall_start_sprites_y;
  word hwall_full_sprite_y;
  word line_size_bytes;
} MazeRenderConf;

typedef struct {
  word** walls;
  word width_cells;
  word height_cells;
} Maze;

Maze generate_maze(word height, word width);
void render_hwalls(bool mode, Maze *maze, MazeRenderConf *mrcptr, 
  word cx, word cy, Page* page, Image* sprites, 
  bool log, FILE* logfile);
void render_vwallz(bool mode, Maze *maze, MazeRenderConf *maze_render_conf,
  word cx, word cy, Page* page, Image* sprites, 
  bool log, FILE* logfile);

#endif