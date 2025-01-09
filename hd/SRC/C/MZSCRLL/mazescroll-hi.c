#include <stdio.h>
#include <stdlib.h>
#include <osbind.h>
#include <string.h>
#include <unistd.h>
#include <stddef.h>
#include <time.h>

typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned long addr_t;

typedef struct {
  void* aligned_ptr;
  void* original_ptr;
} AlignedBuffer;

typedef struct {
  unsigned short* palette;
  unsigned short* bitmap;
} Screen;

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

  screen.palette = (unsigned short*)malloc(16 * sizeof(unsigned short));
  screen.bitmap = (unsigned short*)malloc(16000 * sizeof(unsigned short));

  // Open the file in binary mode
  FILE* file = fopen(filename, "rb");
  if (!file) {
    perror("Error opening file");
    exit(EXIT_FAILURE);
  }

  // Skip the first 16-bit word
  if (fseek(file, sizeof(unsigned short), SEEK_SET) != 0) {
    perror("Error seeking in file");
    fclose(file);
    exit(EXIT_FAILURE);
  }

  // Read the next 16 words into the palette array
  if (fread(screen.palette, sizeof(unsigned short), 16, file) != 16) {
    perror("Error reading palette data");
    fclose(file);
    exit(EXIT_FAILURE);
  }

  // Read the next 16000 words into the bitmap array
  if (fread(screen.bitmap, sizeof(unsigned short), 16000, file) != 16000) {
    perror("Error reading bitmap data");
    fclose(file);
    exit(EXIT_FAILURE);
  }

  // Close the file
  fclose(file);

  // Return the struct containing the arrays
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

int main() {
  const size_t screen_size_bytes = 32000; // Set the size of the buffer
  AlignedBuffer altpage_ram = new_aligned_buffer(screen_size_bytes);
  void* physbase = Physbase();
  void* logbase = altpage_ram.aligned_ptr;
  void* tmpbase;
  Cursconf(0, 1);

  Screen original_screen = copy_screen(physbase);
  memset(physbase, 0, 32000);

  Screen sprite_and_palette_screen = read_degas_file(".\\RES\\WALLS.PI3");
  Setpalette(sprite_and_palette_screen.palette);

  word src_line;
  addr_t sprite_screen_addr = (addr_t)sprite_and_palette_screen.bitmap;
  addr_t logbase_addr;
  addr_t src_addr;
  addr_t dest_addr;
  addr_t cleanup_addr;

  word oldx = 0;
  word tmp_oldx = 0;
  word frames = 0;

  byte zeros[2] = { 0, 0 };

  clock_t start = clock();

  addr_t oldxoffset;
  addr_t xoffset;
  byte col;

#define CHUNK_WIDTH_PIXELS 16
#define CHUNK_WIDTH_BYTES 2
#define LINE_WIDTH_BYTES 80      
#define COL_WIDTH 64
#define HEIGHT 275
#define COLS 6

  for (word x = 0; x < 250; x++) { 
    logbase_addr = (unsigned long)logbase;
    src_line = x % 16;
    src_addr = sprite_screen_addr + (src_line * LINE_WIDTH_BYTES);
    for (col = 0; col < COLS; col++) {
      oldxoffset = logbase_addr + (((oldx + col * COL_WIDTH) / CHUNK_WIDTH_PIXELS) * CHUNK_WIDTH_BYTES);
      xoffset = logbase_addr + (((x + col * COL_WIDTH)  / CHUNK_WIDTH_PIXELS) * CHUNK_WIDTH_BYTES);
      for (word line = 0; line < HEIGHT; line++) {
        cleanup_addr = line * LINE_WIDTH_BYTES + oldxoffset;
        memcpy((void*)cleanup_addr, zeros, CHUNK_WIDTH_BYTES);
        dest_addr = line * LINE_WIDTH_BYTES + xoffset;
        memcpy((void*)dest_addr, (void*)src_addr, CHUNK_WIDTH_BYTES);
      };
    }
    tmpbase = physbase;
    physbase = logbase;
    logbase = tmpbase;
    Vsync();
    Setscreen(logbase, physbase, -1);

    oldx = tmp_oldx;
    tmp_oldx = x;
    frames++;
  }
  clock_t end = clock();
  double total_time = (double)(end - start) / CLOCKS_PER_SEC;
  double fps = frames / total_time;

  Setscreen(physbase, physbase, -1);
  printf("%d cols\n%d height\n%d lines\n", COLS, HEIGHT, COLS*HEIGHT);
  printf("%d frames\n%f seconds\n%f fps\n", frames, total_time, fps);

  getchar();

  put_screen(original_screen, physbase);

  free_screen(sprite_and_palette_screen);
  free_aligned_buffer(altpage_ram);
  return 0;
}