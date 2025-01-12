#include <stdio.h>
#include <stdlib.h>
#include <osbind.h>
#include <string.h>
#include <unistd.h>
#include <stddef.h>
#include <time.h>

#define COL_HEIGHT_PX 200
#define COLS 5
#define COL_WIDTH_PX 32
#define LINE_SIZE_BYTES 160
#define HORIZ_WALL_BYTES 32
#define CHUNK_SIZE_BYTES 16

typedef unsigned char byte;
typedef unsigned short word;

typedef struct {
  void* aligned_ptr;
  void* original_ptr;
} AlignedBuffer;

typedef struct {
  unsigned short* palette;
  unsigned short* bitmap;
} Screen;

void *tmpbase;

AlignedBuffer new_aligned_buffer(size_t size) {
  void* original_ptr = malloc(size + 255);
  if (!original_ptr) {
    return (AlignedBuffer) { NULL, NULL }; // Return NULL if malloc fails
  }
  memset(original_ptr, 0, size + 255);
  unsigned long addr = (unsigned long)original_ptr;
  unsigned long aligned_addr = (addr + 255) & ~255;

  AlignedBuffer buffer = { (void*)aligned_addr, original_ptr };
  // printf("Original pointer: %p\n", buffer.original_ptr);
  // printf("Aligned pointer:  %p\n", buffer.aligned_ptr);

  return buffer;
}
void free_aligned_buffer(AlignedBuffer buffer) {
  free(buffer.original_ptr);
}
void get_current_palette(unsigned short* palette) {
  for (byte i = 0; i < 16; i++) {
    palette[i] = Setcolor(i, -1);
    // printf("color %d = %04X\n",i,palette[i]);
    Setcolor(i, palette[i]);
  }
}
Screen read_degas_file(const char* filename) {
  Screen screen; // Struct to hold the arrays

  screen.palette = (word*)malloc(16 * sizeof(word));
  screen.bitmap = (word*)malloc(16000 * sizeof(word));

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
  if (fread(screen.bitmap, sizeof(word), 16000, file) != 16000) {
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
  screen.palette = (unsigned short*)malloc(16 * sizeof(unsigned short));
  screen.bitmap = (unsigned short*)malloc(16000 * sizeof(unsigned short));
  get_current_palette(screen.palette);
  memcpy(screen.bitmap, addr, 32000);
  return screen;
}
void put_screen(Screen screen, void* buffer) {
  Setpalette(screen.palette);
  memcpy(buffer, screen.bitmap, 32000);
}
void free_screen(Screen screen) {
  free(screen.bitmap);
  free(screen.palette);
}
void clear_screen(Screen screen) {
  memset(screen.bitmap, 0, 32000);
}
void swap_pages(void **a, void **b){
    tmpbase = *b;
    *b = *a;
    *a = tmpbase;
    Vsync();
    Setscreen(*a, *b, -1);
}
void the_end(clock_t start, clock_t end,void *physbase,Screen original_screen, word frames){
 double total_time = (double)(end - start) / CLOCKS_PER_SEC;
  double fps = frames / total_time;

  Setscreen(physbase, physbase, -1);

  printf("%d cols\n%d height\n%d lines\n", COLS, COL_HEIGHT_PX, COLS * COL_HEIGHT_PX);
  printf("%d frames\n%f seconds\n%f fps\n", frames, total_time, fps);

  getchar();
  put_screen(original_screen, physbase);
}

int main() {
  const size_t screen_size_bytes = 32000; // Set the size of the buffer
  AlignedBuffer altpage_ram = new_aligned_buffer(screen_size_bytes);
  void* logbase = altpage_ram.aligned_ptr;
  void* physbase = Physbase();
  Cursconf(0, 1);

  Screen original_screen = copy_screen(physbase);
  Screen sprite_screen = read_degas_file(".\\RES\\SPRT.PI1");
  memcpy(physbase,sprite_screen.bitmap,32000);
  getchar();
  memset(physbase,0,32000);

  byte vertical_wall_src_y, horiz_wall_chunk1_src_y, horiz_wall_chunk2_src_y;
  unsigned long vertical_wall_src_addr, horiz_wall_chunk1_src_addr, horiz_wall_chunk2_src_addr;
  unsigned long sprite_screen_addr = (unsigned long) sprite_screen.bitmap;
  unsigned long logbase_addr;
  unsigned long dest_addr;
  unsigned long cleanup_addr;
  unsigned long old_vert_xoffset;
  unsigned long vert_xoffset;
  unsigned long old_horiz_xoffset;
  unsigned long horiz_xoffset;
  word x = 0;
  word oldx = 0;
  word tmp_oldx = 0;
  word frames = 0;
  byte zeros[32] = {0};
  clock_t start = clock();
  byte col;

#define CYCLES 150

  for (word x = 0; x < CYCLES; x++) {

    logbase_addr = (unsigned long)logbase;

    vertical_wall_src_y = x % 16;
    vertical_wall_src_addr = sprite_screen_addr + (vertical_wall_src_y * LINE_SIZE_BYTES);
    horiz_wall_chunk1_src_y = 30 + (x % 32);
    horiz_wall_chunk1_src_addr = sprite_screen_addr + (horiz_wall_chunk1_src_y * LINE_SIZE_BYTES);

    //cleanup
    for (col = 0; col < COLS; col++) {
      old_vert_xoffset = logbase_addr + (((oldx + col * COL_WIDTH_PX) / CHUNK_SIZE_BYTES) * 8);
      old_horiz_xoffset = logbase_addr + (((oldx + col * COL_WIDTH_PX) / 32 ) * 16);

      for (word line = 0; line < (COL_HEIGHT_PX * LINE_SIZE_BYTES); line = line + LINE_SIZE_BYTES) {
        cleanup_addr = line + old_vert_xoffset;
        memcpy((void*)cleanup_addr, zeros, 2);
      };
      word dest_y = (1+col % 2) * COL_WIDTH_PX;
      cleanup_addr = (dest_y * LINE_SIZE_BYTES) + old_horiz_xoffset;
      memcpy((void*)cleanup_addr, zeros, CHUNK_SIZE_BYTES*2);
    }

    for (col = 0; col < COLS; col++) {
      vert_xoffset = logbase_addr + (((x + col * COL_WIDTH_PX) / CHUNK_SIZE_BYTES) * 8);
      // vert lines
      for (word line = 0; line < (COL_HEIGHT_PX * LINE_SIZE_BYTES); line = line + LINE_SIZE_BYTES) {
        if (!(line ==32 || line==64)) {
          dest_addr = line + vert_xoffset;
          memcpy((void*)dest_addr, (void*)vertical_wall_src_addr, 2);
        }
      };
   }

    for (col = 0; col < COLS; col++) {
      // horiz lines
      horiz_xoffset = logbase_addr + (((x + col * COL_WIDTH_PX) / 32) * 16);
      word dest_y = (1+col % 2) * COL_WIDTH_PX;
      dest_addr = (dest_y * LINE_SIZE_BYTES) + horiz_xoffset;
      memcpy((void*)dest_addr,(void*)horiz_wall_chunk1_src_addr,32);
    }

    swap_pages(&logbase, &physbase);
    oldx = tmp_oldx;
    tmp_oldx = x;
    frames++;
  }
  clock_t end = clock();
  the_end(start,end,physbase,original_screen,frames);
  free_screen(sprite_screen);
  free_aligned_buffer(altpage_ram);
  return 0;
}