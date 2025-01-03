#include <stdio.h>
#include <stdlib.h>
#include <osbind.h>
#include <string.h>
#include <unistd.h>
#include <stddef.h>

typedef struct
{
  void *aligned_ptr;
  void *original_ptr;
} AlignedBuffer;

typedef struct {
    unsigned short *palette;
    unsigned short *bitmap;
  } Image;

AlignedBuffer align_buffer(size_t size)
{
  void *original_ptr = malloc(size + 255);
  if (!original_ptr)
  {
    return (AlignedBuffer){NULL, NULL}; // Return NULL if malloc fails
  }
  memset(original_ptr,0,size+255);
  unsigned long addr = (unsigned long)original_ptr;
  unsigned long aligned_addr = (addr + 255) & ~255;

  AlignedBuffer buffer = {(void *)aligned_addr, original_ptr};
  printf("Original pointer: %p\n", buffer.original_ptr);
  printf("Aligned pointer:  %p\n", buffer.aligned_ptr);

  return buffer;
}
void fill(void *buffer, size_t buffer_size, const unsigned char sequence[8])
{
  if (buffer == NULL || sequence == NULL || buffer_size == 0)
  {
    return; // Handle invalid input
  }

  unsigned char *byte_buffer = (unsigned char *)buffer;

  for (size_t i = 0; i < buffer_size; i += 8)
  {
    // Copy the 4-byte sequence into the buffer
    for (size_t j = 0; j < 8 && (i + j) < buffer_size; j++)
    {
      byte_buffer[i + j] = sequence[j];
    }
  }
}
void get_current_palette(unsigned short* palette){
  for(char i=0;i<16;i++) {
    palette[i] = Setcolor(i,-1);
    // printf("color %d = %04X\n",i,palette[i]);
    Setcolor(i,palette[i]);
  }
}
Image read_degas_screen(const char *filename) {
    Image screen;  // Struct to hold the arrays

    screen.palette = (unsigned short *) malloc(16 * sizeof(unsigned short));
    screen.bitmap = (unsigned short *) malloc(16000 * sizeof(unsigned short));

    // Open the file in binary mode
    FILE *file = fopen(filename, "rb");
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

int main() {
  const size_t screen_size_bytes = 32000; // Set the size of the buffer
  AlignedBuffer screen_ram = align_buffer(screen_size_bytes);
  void *physbase = Physbase();
  void *logbase = screen_ram.aligned_ptr;
  
  printf("logbase: %p\n", logbase);
  printf("physbase: %p\n", physbase);

  unsigned short old_palette[16];
  get_current_palette(old_palette);
  void *old_bitmap = malloc(32000); 

  Image screen1 = read_degas_screen(".\\RES\\PAGE1.PI1");
  Image screen2 = read_degas_screen(".\\RES\\PAGE2.PI1");

  memcpy(old_bitmap, physbase, 16000 * sizeof(unsigned short));
  memcpy(physbase, screen1.bitmap, 16000 * sizeof(unsigned short));
  memcpy(logbase, screen2.bitmap, 16000 * sizeof(unsigned short));

  Setpalette(screen1.palette);
  Setscreen(logbase, physbase, -1);

  Setpalette(screen2.palette);
  Setscreen(physbase, logbase, -1);

  for (char i = 0; i < 2; i++) {
    Setpalette(screen1.palette);
    Setscreen(logbase, physbase, -1);
    sleep(1);

    Setpalette(screen2.palette);
    Setscreen(physbase, logbase, -1);
    sleep(1);
  }

  Setpalette(old_palette);
  memcpy(physbase, old_bitmap, 32000);
  Setscreen(physbase, physbase, -1);

  getchar();

  free(screen1.bitmap);
  free(screen1.palette);
  free(screen2.bitmap);
  free(screen2.palette);
  free(screen_ram.original_ptr);
  return 0;
}
