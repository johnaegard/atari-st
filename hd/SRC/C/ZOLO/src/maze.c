#include <16bittypes.h>
#include <stddef.h>
#include <stdbool.h>
#include <image.h>
#include <maze.h>
#include <mint/osbind.h>

const word HWALL_NO_SPRITE = 0;
const word HWALL_END_SPRITE = 1;
const word HWALL_START_SPRITE = 2;
const word HWALL_FULL_SPRITE = 3;

const byte zeroes[32] = {};

Maze generate_maze(word height_cells, word width_cells) {
  srand(time(NULL));

  word** map_data = (word**)Malloc(height_cells * sizeof(word*));
  if (map_data == NULL) {
    printf("Memory allocation failed!\n");
    exit(1);
  }
  for (int r = 0; r < height_cells; r++) {
    map_data[r] = (word*)Malloc(width_cells * sizeof(word));
    if (map_data[r] == NULL) {
      printf("Memory allocation failed!\n");
      exit(1);
    }
    for (int c = 0; c < width_cells; c++) {
      word roll = (rand() & 0XF);
      if (roll <= 3) {
        map_data[r][c] = roll;
      }
      else {
        map_data[r][c] = 0;
      }
      map_data[r][c] = 3;
    }
  }

  // make edges
  for (int r = 0; r < height_cells; r++) {
    map_data[r][width_cells - 1] = 1;
    map_data[r][0] = 1;
  }
  for (int c = 0; c < width_cells; c++) {
    map_data[0][c] = 2;
    map_data[height_cells - 1][c] = 2;
  }

  map_data[0][0] = 3;
  map_data[0][height_cells - 1] = 3;

  // map_data[3][2] = 2;
  // map_data[3][5] = 1;
  // map_data[4][4] = 2;
  map_data[2][2] = 3;
  // map_data[5][7] = 2;
  // map_data[7][3] = 2;
  // map_data[7][4] = 2;
  // map_data[7][5] = 2;
  // map_data[7][7] = 1;

  Maze maze = {
    .walls = map_data,
    .height_cells = height_cells,
    .width_cells = width_cells
  };

  return maze;
}



void render_vwallz(bool draw_mode, Maze* maze, MazeRenderConf* mrc, 
  word cx, word cy, Page* page, Image* sprites, 
  bool log, FILE* logfile) {

  if (draw_mode == MAZE_ERASE_MODE) {
    cx = page->last_cx;
    cy = page->last_cy;
  }

  addr screenbase_addr = (addr) page->base;
  addr spritebase_addr = (addr) sprites->base;
  addr dest_addr;

  signed short start_row = (cy - mrc->viewport_height_px / 2) / mrc->cell_size_px;
  signed short end_row = 2 + (cy + mrc->viewport_height_px / 2) / mrc->cell_size_px;
  signed short start_col = -1 + (cx - mrc->viewport_width_px / 2) / mrc->cell_size_px;
  signed short end_col = (cx + mrc->viewport_width_px / 2) / mrc->cell_size_px;
  signed short topleft_x = cx - (mrc->viewport_width_px / 2);
  signed short topleft_y = cy - (mrc->viewport_height_px / 2);
  signed short screen_yoffset = (topleft_y > 0) ? 
                                (-1 * (cy % mrc->cell_size_px)) :
                                -1 * (topleft_y % mrc->cell_size_px);

  word cx_mod = cx % 32;
  word vwall_src_y = (16 - (cx_mod % 16)) % 16;
  addr vwall_src_addr = (draw_mode == true) ? spritebase_addr + (vwall_src_y * LINE_SIZE_BYTES) : (addr)zeroes;

  // TODO LOL
  signed short col_offset_bytes = (topleft_x < -79) ? 16 :
    (topleft_x < -63) ? 0 :
    (topleft_x < -47) ? 16 :
    (topleft_x < -31) ? 0 :
    (topleft_x < -15) ? 16 :
    (topleft_x < 0) ? 0 :
    (cx_mod == 0) ? 0 :
    (cx_mod >= 16) ? 0 : -16;
  word vwall_chunk_offset_bytes = (cx_mod > 0 && cx_mod <= 16) ? 8 : 0;

  word screen_row = 0;
  signed short start_yoff_px ;
  signed long start_yoff_bytes;
  signed long end_yoff_bytes;
  addr start_dest_addr;
  addr end_dest_addr;

  for (signed short maze_row = start_row; maze_row < end_row; maze_row++) {
    signed short screen_col = 0;
    if (maze_row < 0 || maze_row >= maze->height_cells) {
      screen_row++;
      continue;
    }

    start_yoff_bytes = ((screen_row * mrc->cell_size_px) + screen_yoffset) * LINE_SIZE_BYTES;
    end_yoff_bytes   = start_yoff_bytes + (mrc->cell_size_px * LINE_SIZE_BYTES);
    end_dest_addr    = (end_yoff_bytes <= (LINE_SIZE_BYTES * mrc->viewport_height_px)) ? 
                       screenbase_addr + end_yoff_bytes :      
                       screenbase_addr + (LINE_SIZE_BYTES * mrc->viewport_height_px) ;
    addr foo         = (start_yoff_bytes < 0) ? screenbase_addr : screenbase_addr + start_yoff_bytes;

    for (signed short maze_col = start_col; maze_col <= end_col; maze_col++) {
      if (maze_col < 0 || maze_col >= maze->width_cells) {
        screen_col++;
        continue;
      }
      if ((maze->walls[maze_row][maze_col] & 1) == 1) {
        word screen_col_offset_bytes = screen_col * CELL_WIDTH_BYTES;
        signed short vwall_xoff = screen_col_offset_bytes + col_offset_bytes + vwall_chunk_offset_bytes;

        if (vwall_xoff < 0 || vwall_xoff >= mrc->viewport_width_bytes) {
          screen_col++;
          continue;
        }
        start_dest_addr = foo + vwall_xoff;
        for (addr dest_addr = start_dest_addr; dest_addr < end_dest_addr; dest_addr = dest_addr + LINE_SIZE_BYTES) {
          memcpy((void*)dest_addr, (void*)vwall_src_addr, 2);
        }

      }
      screen_col++;
    }
    screen_row++;
  }

  if (draw_mode == true) {
    page->last_cx = cx;
    page->last_cy = cy;
  }
}

void render_hwalls(bool draw_mode, Maze* maze, MazeRenderConf *mrc, 
  word cx, word cy, Page* page, Image* sprites, 
  bool log, FILE* logfile) {

  if (draw_mode == MAZE_ERASE_MODE) {
    cx = page->last_cx;
    cy = page->last_cy;
  }

  addr screenbase_addr = (addr) page->base;
  addr spritebase_addr = (addr) sprites->base;
  addr dest_addr;

  word cx_mod             = cx % 32;
  signed short start_row  = (cy - mrc->viewport_height_px / 2) / mrc->cell_size_px;
  signed short end_row    = 2 + (cy + mrc->viewport_height_px / 2) / mrc->cell_size_px;
  signed short start_col  = -1 + (cx - mrc->viewport_width_px / 2) / mrc->cell_size_px;
  signed short end_col    = (cx + mrc->viewport_width_px / 2) / mrc->cell_size_px;
  signed short topleft_x  = cx - (mrc->viewport_width_px / 2);
  signed short topleft_y  = cy - (mrc->viewport_height_px / 2);

  signed short screen_yoffset = (topleft_y > 0) ? (-1 * (cy % mrc->cell_size_px)) : 
                                   -1 * (topleft_y % mrc->cell_size_px);

  // TODO LOL
  signed short col_offset_bytes = (topleft_x < -79) ? 16 :
    (topleft_x < -63) ? 0 :
    (topleft_x < -47) ? 16 :
    (topleft_x < -31) ? 0 :
    (topleft_x < -15) ? 16 :
    (topleft_x < 0) ? 0 :
    (cx_mod == 0) ? 0 :
    (cx_mod >= 16) ? 0 : -16;
  // fprintf(logfile, "B\n");
  // fflush(logfile);

  word screen_row = 0;
  for (signed short maze_row = start_row; maze_row < end_row; maze_row++) {
    signed short screen_col = 0;
    if (maze_row < 0 || maze_row >= maze->height_cells) {
      screen_row++;
      continue;
    }
    for (signed short maze_col = start_col; maze_col <= end_col; maze_col++) {
      if (maze_col < 0 || maze_col >= maze->width_cells) {
        screen_col++;
        continue;
      }
      // fprintf(logfile, "C\n");
      // fflush(logfile);

      bool prev_cell_has_hwall = (maze_col <= 0) ? false :
        (screen_col == -1) ? false :
        ((maze->walls[maze_row][maze_col - 1] & 2) == 2);
      // fprintf(logfile, "D\n");
      // fflush(logfile);

      bool this_cell_has_hwall = (maze_col >= maze->width_cells - 1) ? false :
        ((maze->walls[maze_row][maze_col] & 2) == 2);
      // fprintf(logfile, "E\n");
      // fflush(logfile);

      word hwall_sprite_type;

      if (this_cell_has_hwall) {
        if (cx_mod == 0) {
          hwall_sprite_type = HWALL_FULL_SPRITE;
        }
        else if (prev_cell_has_hwall) {
          hwall_sprite_type = HWALL_FULL_SPRITE;
        }
        else {
          hwall_sprite_type = HWALL_START_SPRITE;
        }
      }
      else {
        if (prev_cell_has_hwall) {
          hwall_sprite_type = HWALL_END_SPRITE;
        }
        else {
          hwall_sprite_type = HWALL_NO_SPRITE;
        }
      }

      word hwall_src_y;

      if (hwall_sprite_type != HWALL_NO_SPRITE) {
        if (hwall_sprite_type == HWALL_END_SPRITE) {
          hwall_src_y = HWALL_END_SPRITES_Y + cx_mod;
        }
        else if (hwall_sprite_type == HWALL_START_SPRITE) {
          hwall_src_y = HWALL_START_SPRITES_Y + cx_mod;
        }
        else if (hwall_sprite_type == HWALL_FULL_SPRITE) {
          hwall_src_y = HWALL_FULL_SPRITE_Y;
        }
        else {
          perror("wtf?");
          exit(1);
        }

        signed short hwall_screen_col_offset_bytes = screen_col * CELL_WIDTH_BYTES;
        signed short hwall_xoffset_bytes = hwall_screen_col_offset_bytes + col_offset_bytes;
        signed short hwall_yoffset_bytes = ((screen_row * mrc->cell_size_px) + screen_yoffset) * LINE_SIZE_BYTES;
        dest_addr = screenbase_addr + hwall_yoffset_bytes + hwall_xoffset_bytes;

        // if (log) {
        //   fprintf(log_file,
        //     "    hwall draw_mode=%d, maze_row=%d, maze_col=%d, screen_row=%d, screen_col=%d\n",
        //     mode, maze_row, maze_col, screen_row, screen_col);
        //   fprintf(log_file,
        //     "          prev_cell_has_hwall=%d,this_cell_has_hwall=%d, hwall_sprite_type=%d, cx_mod=%d, hwall_src_y=%d\n",
        //     prev_cell_has_hwall, this_cell_has_hwall, hwall_sprite_type, cx_mod, hwall_src_y);
        //   fprintf(log_file,
        //     "          hwall_screen_col_offset_bytes=%d, col_offset_bytes=%d hwall_xoffset_bytes=%d\n",
        //     hwall_screen_col_offset_bytes, col_offset_bytes, hwall_xoffset_bytes);
        //   fprintf(log_file,
        //     "          screen_row=%d, screen_yoffset=%d, hwall_yoffset_bytes=%d, dest_addr=%d\n",
        //     screen_row, screen_yoffset, hwall_yoffset_bytes, dest_addr);
        // }
        if (hwall_xoffset_bytes >= 0 &&
          hwall_xoffset_bytes < (mrc->viewport_width_bytes) &&
          hwall_yoffset_bytes >= 0 &&
          hwall_yoffset_bytes <= (mrc->viewport_height_px) * LINE_SIZE_BYTES) {
            addr hwall_src_addr = (draw_mode == true) ? spritebase_addr + (hwall_src_y * LINE_SIZE_BYTES) : (addr)zeroes;
            memcpy((void*)dest_addr, (void*)hwall_src_addr, 16);
        }
      }
      screen_col++;
    }
    screen_row++;
  }

  if (draw_mode == true) {
    page->last_cx = cx;
    page->last_cy = cy;
  }
}
