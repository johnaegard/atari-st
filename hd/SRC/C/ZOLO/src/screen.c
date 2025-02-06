#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <mint/osbind.h>
#include <zolo-types.h>

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
Screen make_screen_from_degas_file(const char* filename) {
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
Screen make_screen_from_base(void* base) {
  Screen screen;
  screen.palette = (word*)Malloc(16 * sizeof(word));
  screen.base = (word*)Malloc(16000 * sizeof(word));
  get_current_palette(screen.palette);
  memcpy(screen.base, base, 32000);
  return screen;
}
void copy_screen_to_base(Screen screen, void* base) {
  Setpalette(screen.palette);
  memcpy(base, screen.base, 32000);
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

  copy_screen_to_base(original_screen, physbase);
  Setscreen(physbase, physbase, -1);

  fprintf(log_file, "%d frames\n%f seconds\n%f fps\n", frames, total_time, fps);
  if (fclose(log_file) != 0) {
    perror("Error closing log file");
  }
}
