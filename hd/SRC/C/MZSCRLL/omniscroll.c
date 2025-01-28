#include <osbind.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define SCREEN_SIZE_BYTES 32000
#define COL_HEIGHT_PX 200
#define COLS 5
#define CELL_SIZE_PX 32
#define CELL_WIDTH_BYTES 16
#define LINE_SIZE_BYTES 160
#define HORIZ_WALL_BYTES 32
#define CHUNK_SIZE_BYTES 16
#define MAZE_WIDTH 32
#define MAZE_HEIGHT 32
#define VIEWPORT_WIDTH 224
#define VIEWPORT_WIDTH_BYTES 112
#define VIEWPORT_HEIGHT 191
#define KEYBOARD 2
#define KEY_UP 72
#define KEY_DOWN 80
#define KEY_LEFT 75
#define KEY_RIGHT 77
#define KEY_ESC 1
#define KEY_L 38
#define KEY_D 32

#define HWALL_NO_SPRITE 0
#define HWALL_END_SPRITE 1
#define HWALL_START_SPRITE 2
#define HWALL_FULL_SPRITE 3

#define DRAW_MODE true
#define ERASE_MODE false

#define HWALL_END_SPRITES_Y 30
#define HWALL_START_SPRITES_Y 70
#define HWALL_FULL_SPRITE_Y 110

typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned long addr;

typedef struct {
  void* aligned_ptr;
  void* original_ptr;
} AlignedBuffer;

typedef struct {
  word* palette;
  word* base;
} Screen;

typedef struct {
  void* base;
  word last_cx;
  word last_cy;
} Base;

FILE* log_file;
Base tmpbase;
const word zeroes_arry[32] = { 0 };
void* zeroes = (void*)zeroes_arry;

AlignedBuffer new_aligned_buffer(size_t size) {
  void* original_ptr = (void*)Malloc(size + 255);

  if (!original_ptr) {
    return (AlignedBuffer) { NULL, NULL };  // Return NULL if malloc fails
  }
  memset(original_ptr, 0, size + 255);
  unsigned long addr = (unsigned long)original_ptr;
  unsigned long aligned_addr = (addr + 255) & ~255;

  AlignedBuffer buffer = { (void*)aligned_addr, original_ptr };
  fprintf(log_file, "Original pointer: %p\n", buffer.original_ptr);
  fprintf(log_file, "Aligned pointer:  %p\n", buffer.aligned_ptr);

  return buffer;
}
void free_aligned_buffer(AlignedBuffer buffer) { free(buffer.original_ptr); }
void get_current_palette(word* palette) {
  for (byte i = 0; i < 16; i++) {
    palette[i] = Setcolor(i, -1);
    // printf("color %d = %04X\n",i,palette[i]);
    Setcolor(i, palette[i]);
  }
}
void dump_degas_file(word* palette, void* base) {
  FILE* file = fopen("DUMP.PI1", "wb");
  if (!file) {
    perror("Error opening file");
    exit(EXIT_FAILURE);
  }
  // low-res word
  size_t written = fwrite(zeroes, 2, 1, file);
  if (written != 1) {
    perror("Failed to write header data");
    fclose(file);
    return;
  }
  // palette
  written = fwrite(palette, 32, 1, file);
  if (written != 1) {
    perror("Failed to write all palette data");
    fclose(file);
    return;
  }
  // screen
  written = fwrite(base, 32000, 1, file);
  if (written != 1) {
    perror("Failed to write all screen data");
    fclose(file);
    return;
  }
  // color cycle
  written = fwrite(zeroes, 32, 1, file);
  if (written != 1) {
    perror("Failed to write all screen data");
    fclose(file);
    return;
  }
  fclose(file);
}
Screen read_degas_file(const char* filename) {
  Screen screen;  // Struct to hold the arrays

  screen.palette = (word*)Malloc(16 * sizeof(word));
  screen.base = (word*)Malloc(16000 * sizeof(word));

  // Open the file in binary mode
  FILE* file = fopen(filename, "rb");
  if (!file) {
    perror("Error opening file");
    exit(EXIT_FAILURE);
  }

  // Skip the first word
  if (fseek(file, sizeof(word), SEEK_SET) != 0) {
    perror("Error seeking in file");
    fclose(file);
    exit(EXIT_FAILURE);
  }
  if (fread(screen.palette, sizeof(word), 16, file) != 16) {
    perror("Error reading palette data");
    fclose(file);
    exit(EXIT_FAILURE);
  }
  if (fread(screen.base, sizeof(word), 16000, file) != 16000) {
    perror("Error reading bitmap data");
    fclose(file);
    exit(EXIT_FAILURE);
  }
  fclose(file);
  Setpalette(screen.palette);
  return screen;
}
Screen copy_screen(void* addr) {
  Screen screen;
  screen.palette = (word*)Malloc(16 * sizeof(word));
  screen.base = (word*)Malloc(16000 * sizeof(word));
  get_current_palette(screen.palette);
  memcpy(screen.base, addr, 32000);
  return screen;
}
void put_screen(Screen screen, void* buffer) {
  Setpalette(screen.palette);
  memcpy(buffer, screen.base, 32000);
}
void free_screen(Screen screen) {
  free(screen.base);
  free(screen.palette);
}
void clear_screen(Screen screen) { memset(screen.base, 0, 32000); }
void swap_pages(Base* logbase, Base* physbase) {
  tmpbase = *physbase;
  *physbase = *logbase;
  *logbase = tmpbase;
  Setscreen(logbase->base, physbase->base, -1);
}
void the_end(clock_t start, clock_t end, void* physbase, Screen original_screen, word frames) {
  double total_time = (double)(end - start) / CLOCKS_PER_SEC;
  double fps = frames / total_time;

  put_screen(original_screen, physbase);
  Setscreen(physbase, physbase, -1);

  fprintf(log_file, "%d frames\n%f seconds\n%f fps\n", frames, total_time, fps);
  if (fclose(log_file) != 0) {
    perror("Error closing log file");
  }
}
word** generate_maze(int rows, int cols) {
  srand(time(NULL));

  word** map_data = (word**)Malloc(rows * sizeof(word*));
  if (map_data == NULL) {
    printf("Memory allocation failed!\n");
    exit(1);
  }
  for (int r = 0; r < rows; r++) {
    map_data[r] = (word*)Malloc(cols * sizeof(word));
    if (map_data[r] == NULL) {
      printf("Memory allocation failed!\n");
      exit(1);
    }
    for (int c = 0; c < cols; c++) {
      word roll = (rand() & 0X7);
      if (roll <= 3) {
        map_data[r][c] = roll;
      }
      else {
        map_data[r][c] = 0;
      }
      map_data[0][c] = 2;
      map_data[r][c] = 3;
    }
    map_data[r][0] = 1;
  }

  for (int r = 0; r < rows; r++) {
    map_data[r][cols - 1] = 1;
    map_data[r][0] = 1;
  }
  for (int c = 0; c < cols; c++) {
    map_data[0][c] = 2;
    map_data[rows - 1][c] = 2;
  }

  map_data[0][0] = 3;
  map_data[0][rows - 1] = 3;

  // map_data[3][2] = 2;
  // map_data[3][5] = 1;
  // map_data[4][4] = 2;
  map_data[4][5] = 3;
  // map_data[5][7] = 2;
  // map_data[7][3] = 2;
  // map_data[7][4] = 2;
  // map_data[7][5] = 2;
  // map_data[7][7] = 1;

  return map_data;
}
void log_maze(word** maze, int rows, int cols, int srow, int scol, int erow, int ecol) {
  fprintf(log_file, "rows=%d, cols=%d, pixel height=%d, pixel_width=%d\n\n", rows, cols, rows * CELL_SIZE_PX, cols * CELL_SIZE_PX);
  for (int r = 0; r < cols; r++) {
    for (int c = 0; c < cols; c++) {
      fprintf(log_file, "%d ", maze[r][c]);
    }
    fprintf(log_file, "\n");
  }
  fprintf(log_file, "\nchunk: srow=%d,scol=%d,erow=%d, ecol=%d\n", srow, scol, erow, ecol);
  for (int r = srow; r < erow; r++) {
    for (int c = scol; c < ecol; c++) {
      fprintf(log_file, "%d", maze[r][c]);
      //     printf("%d%d%d ", r, c, maze[r][c]);
    }
    //   printf("\n\n\n\n");
    fprintf(log_file, "\n");
  }

  fflush(log_file);
}

void render_maze(bool mode, word** maze, word cx, word cy, Base* screenbase, void* spritebase, bool log) {
  Vsync();

  if (mode == ERASE_MODE) {
    cx = screenbase->last_cx;
    cy = screenbase->last_cy;
  }

  addr screenbase_addr = (addr)screenbase->base;
  addr spritebase_addr = (addr)spritebase;
  addr dest_addr;

  word start_row = (cy - VIEWPORT_HEIGHT / 2) / CELL_SIZE_PX;
  word end_row = 2 + (cy + VIEWPORT_HEIGHT / 2) / CELL_SIZE_PX;
  word start_col = -1 + (cx - VIEWPORT_WIDTH / 2) / CELL_SIZE_PX;
  word end_col = (cx + VIEWPORT_WIDTH / 2) / CELL_SIZE_PX;

  signed short start_row_top_y = -1 * (cy % CELL_SIZE_PX);

  word cx_mod = cx % 32;
  word vwall_src_y = (16 - (cx_mod % 16)) % 16;
  addr vwall_src_addr = (mode == DRAW_MODE) ? spritebase_addr + (vwall_src_y * LINE_SIZE_BYTES) : (addr)zeroes;
  // addr vwall_src_addr = sourcebase_addr + (vwall_src_y * LINE_SIZE_BYTES);

  signed short vwall_col_offset_bytes = (cx_mod == 0) ? 0 : (cx_mod >= 16) ? 0 : -16;
  word vwall_chunk_offset_bytes = (cx_mod > 0 && cx_mod <= 16) ? 8 : 0;

#ifdef LOG
  fprintf(log_file,
    "cx=%d, cy=%d, start_row=%d, start_col=%d, end_row=%d, end_col=%d, cxmod=%d, vwall_src_y=%d, vwall_chunk_offset_bytes=%d, "
    "vwall_col_offset_bytes=%d\n",
    cx, cy, start_row, start_col, end_row, end_col, cx_mod, vwall_src_y, vwall_chunk_offset_bytes, vwall_col_offset_bytes);
  // fflush(log_file);
#endif
  word screen_row = 0;
  for (word maze_row = start_row; maze_row < end_row; maze_row++) {
    signed short screen_col = -1;
    for (word maze_col = start_col; maze_col <= end_col; maze_col++) {
      if ((maze[maze_row][maze_col] & 1) == 1) {
        // 
        // vert lines
        //
        word screen_col_offset_bytes = screen_col * CELL_WIDTH_BYTES;
        signed short xoff = screen_col_offset_bytes + vwall_col_offset_bytes + vwall_chunk_offset_bytes;
        // fprintf(log_file,"xoff=%d, VIEWPORT_W_B=%d\n",xoff,VIEWPORT_WIDTH_BYTES);
        if (xoff < 0 || xoff >= VIEWPORT_WIDTH_BYTES) {
          if (log) {
            fprintf(log_file, "    vwall xoff=%d out of bounds, skipping screen_col=%d\n", xoff, screen_col);
          }
          screen_col++;
          continue;
        }
        signed short start_yoff = ((screen_row * CELL_SIZE_PX) + start_row_top_y) * LINE_SIZE_BYTES;
        for (signed long yoffset = start_yoff; yoffset < start_yoff + (CELL_SIZE_PX * LINE_SIZE_BYTES);
          yoffset = yoffset + LINE_SIZE_BYTES) {
          dest_addr = screenbase_addr + yoffset + xoff;
          // CLIP
          if (dest_addr >= screenbase_addr && dest_addr <= screenbase_addr + VIEWPORT_HEIGHT * LINE_SIZE_BYTES) {
            memcpy((void*)dest_addr, (void*)vwall_src_addr, 2);
          }
        }
      }

      //
      // hwalls
      ///
      bool prev_cell_has_hwall = (screen_col != -1) ? ((maze[maze_row][maze_col - 1] & 2) == 2) : false;
      bool this_cell_has_hwall = ((maze[maze_row][maze_col] & 2) == 2);

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

        addr hwall_src_addr = (mode == DRAW_MODE) ? spritebase_addr + (hwall_src_y * LINE_SIZE_BYTES) : (addr)zeroes;

        signed short hwall_screen_col_offset_bytes = screen_col * CELL_WIDTH_BYTES;
        signed short hwall_col_offset_bytes = (cx_mod == 0) ? 0 : (cx_mod >= 16) ? 0 : -16;
        signed short hwall_xoffset_bytes = hwall_screen_col_offset_bytes + hwall_col_offset_bytes;
        signed short hwall_yoffset_bytes = ((screen_row * CELL_SIZE_PX) + start_row_top_y) * LINE_SIZE_BYTES;
        dest_addr = screenbase_addr + hwall_yoffset_bytes + hwall_xoffset_bytes;

        if (log) {
          fprintf(log_file,
            "  draw_mode=%d, cx=%d, cy=%d, maze_row=%d, maze_col=%d, screen_row=%d, screen_col=%d,\n  prev_cell_has_hwall=%d,this_cell_has_hwall=%d, hwall_sprite_type=%d, cx_mod=%d, hwall_src_y=%d\n  hwall_screen_col_offset_bytes=%d, hwall_xoffset_bytes=%d, hwall_yoffset_bytes=%d, dest_addr=%d\n\n",
            mode, cx, cy, maze_row, maze_col, screen_row, screen_col, prev_cell_has_hwall, this_cell_has_hwall, hwall_sprite_type, cx_mod, hwall_src_y, hwall_screen_col_offset_bytes, hwall_xoffset_bytes, hwall_yoffset_bytes, dest_addr);
        }
        if (hwall_xoffset_bytes >= 0 &&
          hwall_xoffset_bytes < VIEWPORT_WIDTH_BYTES &&
          hwall_yoffset_bytes >= 0 &&
          hwall_yoffset_bytes <= VIEWPORT_HEIGHT * LINE_SIZE_BYTES) {
          memcpy((void*)dest_addr, (void*)hwall_src_addr, 16);
        }
      }
      screen_col++;
    }
    screen_row++;
  }

  if (mode == DRAW_MODE) {
    screenbase->last_cx = cx;
    screenbase->last_cy = cy;
  }

  // fflush(log_file);
  // getchar();

}

int main() {

  log_file = fopen("stolo.log", "w");
  if (log_file == NULL) {
    perror("Error opening log file");
    return EXIT_FAILURE;
  }

  srand(time(NULL));

  AlignedBuffer logbase_buffer = new_aligned_buffer(SCREEN_SIZE_BYTES);
  AlignedBuffer physbase_buffer = new_aligned_buffer(SCREEN_SIZE_BYTES);
  void* logbase_ptr = logbase_buffer.aligned_ptr;
  void* physbase_ptr = physbase_buffer.aligned_ptr;
  //fprintf(log_file, "a logbase=%p, physbase=%p\n", logbase_ptr, physbase_ptr);

  Cursconf(0, 1);

  Screen original_screen = copy_screen(physbase_ptr);
  Screen sprite_screen = read_degas_file(".\\RES\\SPRT.PI1");
  Screen background_screen = read_degas_file(".\\RES\\BKGD.PI1");
  memcpy(physbase_ptr, background_screen.base, SCREEN_SIZE_BYTES);
  memcpy(logbase_ptr, background_screen.base, SCREEN_SIZE_BYTES);
  free_screen(background_screen);
  Setscreen(logbase_ptr, physbase_ptr, -1);

  word** maze = generate_maze(MAZE_HEIGHT, MAZE_WIDTH);
  log_maze(maze, MAZE_HEIGHT, MAZE_WIDTH, 2, 2, 8, 8);

  word vx = 160;
  word vy = 160;
  word old_vx = vx;
  word old_vy = vy;
  word tmp_old_vx = vx;
  word tmp_old_vy = vy;

  word frames = 0;
  clock_t start = clock();
  long key;
  byte keep_looping = 1;

  Base physbase = { physbase_ptr, vx,vy };
  Base logbase = { logbase_ptr, vx,vy };

  bool log = false;
  bool screendump = false;

  while (keep_looping) {
    if (log) {
      fprintf(log_file, "\nFRAME=%d, logbase=%p, physbase=%p, vx=%d, vy=%d\n", frames, logbase.base, physbase.base, vx, vy);
    }
    render_maze(ERASE_MODE, maze, vx, vy, &logbase, sprite_screen.base, false);
    render_maze(DRAW_MODE, maze, vx, vy, &logbase, sprite_screen.base, log);
    if (log) {
      fflush(log_file);
      log = false;
    }
    if (Bconstat(2) != 0) {
      //while (Bconstat(2) == 0) {}
      key = Bconin(2);
      char scancode = (key >> 16) & 0xFF;
      switch (scancode) {
      case KEY_UP:
        vy--;
        break;
      case KEY_DOWN:
        vy++;
        break;
      case KEY_LEFT:
        vx--;
        break;
      case KEY_RIGHT:
        vx++;
        break;
      case KEY_L:
        log = true;
        break;
      case KEY_D:
        dump_degas_file(sprite_screen.palette, physbase.base);
        break;
      case KEY_ESC:
        keep_looping = 0;
        break;
      }
    }
    swap_pages(&logbase, &physbase);
    //fflush(log_file);
    frames++;
  }

  clock_t end = clock();
  the_end(start, end, physbase_ptr, original_screen, frames);
  free_screen(sprite_screen);
  free_aligned_buffer(logbase_buffer);
  free(maze);
  return 0;
}