#include <stdio.h>
#include <stdlib.h>
#include <osbind.h>
#include <string.h>
#include <unistd.h>

// flips successfully.

typedef struct
{
  void *aligned_ptr;
  void *original_ptr;
} AlignedBuffer;

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

  AlignedBuffer result = {(void *)aligned_addr, original_ptr};
  return result;
}
#include <stddef.h> // For size_t

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
    printf("color %d = %04X\n",i,palette[i]);
    Setcolor(i,palette[i]);
  }
}

int main()
{
  const size_t screen_size_bytes = 32000; // Set the size of the buffer
  AlignedBuffer screen_ram = align_buffer(screen_size_bytes);

  printf("size of u short=%d\n",sizeof(unsigned short));

  unsigned short old_palette[16];
  get_current_palette(old_palette); 

  Setcolor(0,0);
  Setcolor(1,0x777);
  Setcolor(2,0x666);
  Setcolor(3,0x555);
  Setcolor(9, 0x700);
  Setcolor(15,0xddd);

  if (screen_ram.aligned_ptr)
  {
    printf("Original pointer: %p\n", screen_ram.original_ptr);
    printf("Aligned pointer: %p\n", screen_ram.aligned_ptr);
  }
  else
  {
    //        printf("Memory allocation failed.\n");
    return 1;
  }

  void *physbase = Physbase();
  void *logbase = screen_ram.aligned_ptr;

  unsigned char fill_sequence[8] = {255,255,0,0,0,0,255,255};
  fill(logbase, screen_size_bytes, fill_sequence);
  
  printf("logbase: %p\n", logbase);
  printf("physbase: %p\n", physbase);
  Setscreen(logbase, physbase, -1);
  printf("other page!\n");
  Setscreen(physbase, logbase, -1);
  for (char i = 0; i < 3; i++)
  {
    Setscreen(logbase, physbase, -1);
    sleep(1);
    Setscreen(physbase, logbase, -1);
    sleep(1);
  }
  Setpalette(old_palette);
  Setscreen(physbase, physbase, -1);
  getchar();
  free(screen_ram.original_ptr);
  return 0;
}
