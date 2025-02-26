#include <image.h>

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
  byte zeroes[2]={};
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
Image make_image_from_degas_file(const char* filename) {
  AlignedBuffer buffer = new_aligned_buffer(32000);
  Image screen = {
    .palette = (word*)Malloc(16 * sizeof(word)),
    .aligned_buffer = buffer,
    .base = buffer.aligned_ptr
  };

  // Open the file in binary mode
  FILE* file = fopen(filename, "rb");
  if (!file) {
    perror("Error opening file");  screen.palette = (word*)Malloc(16 * sizeof(word));
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
  return screen;
}
Image duplicate_image(Image source){
  AlignedBuffer buffer = new_aligned_buffer(32000);
  Image screen = {
    .palette = (word*)Malloc(16 * sizeof(word)),
    .aligned_buffer = buffer,
    .base = buffer.aligned_ptr
  };
  memcpy(screen.base,source.base,32000);
  return screen;
}
void copy_image_to_base(Image image, void* base) {
  Setpalette(image.palette);
  memcpy(base, image.base, 32000);
}
void free_image(Image image) {
  free(image.base);
  free_aligned_buffer(image.aligned_buffer);
}
void clear_image(Image image) { memset(image.base, 0, 32000); }
void the_end(clock_t start, clock_t end, void* physbase, Image original_screen, word frames) {
  double total_time = (double)(end - start) / CLOCKS_PER_SEC;
  double fps = frames / total_time;

  copy_image_to_base(original_screen, physbase);
  Setscreen(physbase, physbase, -1);

  // fprintf(log_file, "%d frames\n%f seconds\n%f fps\n", frames, total_time, fps);
  // if (fclose(log_file) != 0) {
  //   perror("Error closing log file");
  // }
}
