#include <osbind.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define COL_HEIGHT_PX 200
#define COLS 5
#define CELL_SIZE_PX 32
#define LINE_SIZE_BYTES 160
#define HORIZ_WALL_BYTES 32
#define CHUNK_SIZE_BYTES 16
#define MAZE_WIDTH 6
#define MAZE_HEIGHT 6
#define VIEWPORT_WIDTH 192
#define VIEWPORT_HEIGHT 192

typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned long addr;

typedef struct {
  void *aligned_ptr;
  void *original_ptr;
} AlignedBuffer;

typedef struct {
  word *palette;
  word *base;
} Screen;

void *tmpbase;

AlignedBuffer new_aligned_buffer(size_t size) {
  void *original_ptr = malloc(size + 255);
  if (!original_ptr) {
    return (AlignedBuffer){NULL, NULL};  // Return NULL if malloc fails
  }
  memset(original_ptr, 0, size + 255);
  unsigned long addr = (unsigned long)original_ptr;
  unsigned long aligned_addr = (addr + 255) & ~255;

  AlignedBuffer buffer = {(void *)aligned_addr, original_ptr};
  // printf("Original pointer: %p\n", buffer.original_ptr);
  // printf("Aligned pointer:  %p\n", buffer.aligned_ptr);

  return buffer;
}
void free_aligned_buffer(AlignedBuffer buffer) { free(buffer.original_ptr); }
void get_current_palette(word *palette) {
  for (byte i = 0; i < 16; i++) {
    palette[i] = Setcolor(i, -1);
    // printf("color %d = %04X\n",i,palette[i]);
    Setcolor(i, palette[i]);
  }
}
Screen read_degas_file(const char *filename) {
  Screen screen;  // Struct to hold the arrays

  screen.palette = (word *)malloc(16 * sizeof(word));
  screen.base = (word *)malloc(16000 * sizeof(word));

  // Open the file in binary mode
  FILE *file = fopen(filename, "rb");
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
Screen copy_screen(void *addr) {
  Screen screen;
  screen.palette = (word *)malloc(16 * sizeof(word));
  screen.base = (word *)malloc(16000 * sizeof(word));
  get_current_palette(screen.palette);
  memcpy(screen.base, addr, 32000);
  return screen;
}
void put_screen(Screen screen, void *buffer) {
  Setpalette(screen.palette);
  memcpy(buffer, screen.base, 32000);
}
void free_screen(Screen screen) {
  free(screen.base);
  free(screen.palette);
}
void clear_screen(Screen screen) { memset(screen.base, 0, 32000); }
void swap_pages(void **a, void **b) {
  tmpbase = *b;
  *b = *a;
  *a = tmpbase;
  Vsync();
  Setscreen(*a, *b, -1);
}
void the_end(clock_t start, clock_t end, void *physbase, Screen original_screen, word frames) {
  double total_time = (double)(end - start) / CLOCKS_PER_SEC;
  double fps = frames / total_time;

  Setscreen(physbase, physbase, -1);

  printf("%d cols\n%d height\n%d lines\n", COLS, COL_HEIGHT_PX, COLS * COL_HEIGHT_PX);
  printf("%d frames\n%f seconds\n%f fps\n", frames, total_time, fps);

  getchar();
  put_screen(original_screen, physbase);
}
word **generate_maze(int cols, int rows) {
  srand(time(NULL));

  word **map_data = (word **)malloc(rows * sizeof(word *));
  if (map_data == NULL) {
    printf("Memory allocation failed!\n");
    exit(1);
  }
  for (int r = 0; r < rows; r++) {
    map_data[r] = (word *)malloc(cols * sizeof(word));
    if (map_data[r] == NULL) {
      printf("Memory allocation failed!\n");
      exit(1);
    }
    for (int c = 0; c < cols; c++) {
      word roll = rand() & 0X7;
      map_data[r][c] = 1;
      if (roll <= 3) {
        map_data[r][c] = roll;
      }
      else {
         map_data[r][c] =0; 
      }
    }
  }

  return map_data;
}
void print_maze(word **mazedata, int cols, int rows) {
  for (int r = 0; r < rows; r++) {
    for (int c = 0; c < cols; c++) {
      printf("%d   ", mazedata[r][c]);
    }
    printf("\n\n\n\n");
  }
}
void render_maze(word **maze, word cx, word cy, word oldcx, word oldcy, void *logbase, void *spritebase) {
  Vsync();

  addr logbase_addr = (addr) logbase;
  addr spritebase_addr = (addr) spritebase;
  addr dest_addr;

  word vwall_src_y = (16 - cx % 16) % 16; // x % 16;
  addr vwall_src_addr = spritebase_addr + (vwall_src_y * LINE_SIZE_BYTES);

  word hwall_chunk1_src_y = 30 + (32-cx%32) %32;
  addr hwall_chunk1_src_addr = spritebase_addr + (hwall_chunk1_src_y * LINE_SIZE_BYTES);

  word start_row = (cy - VIEWPORT_HEIGHT / 2) / CELL_SIZE_PX;
  word end_row = (cy + VIEWPORT_HEIGHT / 2) / CELL_SIZE_PX;
  word start_col = (cx - VIEWPORT_HEIGHT / 2) / CELL_SIZE_PX;
  word end_col = (cx + VIEWPORT_HEIGHT / 2) / CELL_SIZE_PX;

  for(word row = start_row; row < end_row ; row++) {
    for(word col = start_col; col < end_col ; col++) {
      if ((maze[row][col] & 1) == 1) {
        // low bit - vert lines
        word xoff = (((col * CELL_SIZE_PX) / CHUNK_SIZE_BYTES) * 8);
        word start_yoff = row * CELL_SIZE_PX * LINE_SIZE_BYTES;
        for (word yoffset = start_yoff; 
                yoffset < start_yoff + (CELL_SIZE_PX * LINE_SIZE_BYTES); 
                yoffset = yoffset + LINE_SIZE_BYTES) {
           dest_addr = logbase_addr + yoffset + xoff;
           memcpy((void *)dest_addr, (void *)vwall_src_addr, 2);
        }
      };
      if ((maze[row][col] & 2) == 2) {
        // second bit - horiz line here
        addr xoffset = ((col * CELL_SIZE_PX) / 32) * 16;
        word yoffset = row * CELL_SIZE_PX * LINE_SIZE_BYTES;
        dest_addr = logbase_addr  + xoffset + yoffset;
        memcpy((void *)dest_addr, (void *)hwall_chunk1_src_addr, 32);
      }
    }
  }
}

int main() {
  srand(time(NULL));
  const size_t screen_size_bytes = 32000;  // Set the size of the buffer
  AlignedBuffer altpage_ram = new_aligned_buffer(screen_size_bytes);
  void *logbase = altpage_ram.aligned_ptr;
  void *physbase = Physbase();
  Cursconf(0, 1);

  Screen original_screen = copy_screen(physbase);
  Screen sprite_screen = read_degas_file(".\\RES\\SPRT.PI1");
  memset(physbase, 0, 32000);

  word **maze = generate_maze(MAZE_WIDTH, MAZE_HEIGHT);
  print_maze(maze, MAZE_WIDTH, MAZE_HEIGHT);
  getchar();
//  memset(physbase, 0, 32000);

  word frames = 0;
  clock_t start = clock();

  render_maze(maze,96,96,0,0,physbase,sprite_screen.base);

  // byte vertical_wall_src_y, horiz_wall_chunk1_src_y, horiz_wall_chunk2_src_y;
  // unsigned long vertical_wall_src_addr, horiz_wall_chunk1_src_addr, horiz_wall_chunk2_src_addr;
  // unsigned long sprite_screen_addr = (unsigned long)sprite_screen.bitmap;
  // unsigned long logbase_addr;
  // unsigned long dest_addr;
  // unsigned long cleanup_addr;
  // unsigned long old_vert_xoffset;
  // unsigned long vert_xoffset;
  // unsigned long old_horiz_xoffset;
  // unsigned long horiz_xoffset;
  // word x = 0;
  // word oldx = 0;
  // word tmp_oldx = 0;
  // byte zeros[32] = {0};
  // byte col;

// #define CYCLES 150

//   for (word x = 0; x < CYCLES; x++) {
//     logbase_addr = (unsigned long)logbase;

//     vertical_wall_src_y = x % 16;
//     vertical_wall_src_addr = sprite_screen_addr + (vertical_wall_src_y * LINE_SIZE_BYTES);
//     horiz_wall_chunk1_src_y = 30 + (x % 32);
//     horiz_wall_chunk1_src_addr = sprite_screen_addr + (horiz_wall_chunk1_src_y * LINE_SIZE_BYTES);

//     // cleanup
//     for (col = 0; col < COLS; col++) {
//       old_vert_xoffset = logbase_addr + (((oldx + col * CELL_SIZE_PX) / CHUNK_SIZE_BYTES) * 8);
//       old_horiz_xoffset = logbase_addr + (((oldx + col * CELL_SIZE_PX) / 32) * 16);
//       for (word line = 0; line < (COL_HEIGHT_PX * LINE_SIZE_BYTES); line = line + LINE_SIZE_BYTES) {
//         cleanup_addr = line + old_vert_xoffset;
//         memcpy((void *)cleanup_addr, zeros, 2);
//       };
//       word dest_y = (1 + col % 2) * CELL_SIZE_PX;
//       cleanup_addr = (dest_y * LINE_SIZE_BYTES) + old_horiz_xoffset;
//       memcpy((void *)cleanup_addr, zeros, 32);
//     }

//     for (col = 0; col < COLS; col++) {
//       vert_xoffset = logbase_addr + (((x + col * CELL_SIZE_PX) / CHUNK_SIZE_BYTES) * 8);
//       // vert lines
//       for (word line = 0; line < (COL_HEIGHT_PX * LINE_SIZE_BYTES); line = line + LINE_SIZE_BYTES) {
//         // if (!(line ==32 || line==64)) {
//         dest_addr = line + vert_xoffset;
//         memcpy((void *)dest_addr, (void *)vertical_wall_src_addr, 2);
//         //}
//       };
//     }
//     for (col = 0; col < COLS; col++) {
//       // horiz lines
//       horiz_xoffset = logbase_addr + (((x + col * CELL_SIZE_PX) / 32) * 16);
//       word dest_y = (1 + col % 2) * CELL_SIZE_PX;
//       dest_addr = (dest_y * LINE_SIZE_BYTES) + horiz_xoffset;
//       memcpy((void *)dest_addr, (void *)horiz_wall_chunk1_src_addr, 32);
//     }
//     swap_pages(&logbase, &physbase);
//     oldx = tmp_oldx;
//     tmp_oldx = x;
//     frames++;
//   }
  getchar();
  clock_t end = clock();
  the_end(start, end, physbase, original_screen, frames);
  free_screen(sprite_screen);
  free_aligned_buffer(altpage_ram);
  free(maze);
  return 0;
}