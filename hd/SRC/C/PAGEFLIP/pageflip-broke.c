// TODO load PI1s, including pallette, into the two pages

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

AlignedBuffer align_buffer(size_t size){
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
    palette[i] = Setcolor(i,0x0777);
    Setcolor(i,palette[i]);
  }
}

int main(){
  
  const size_t screen_size_bytes = 32000; // Set the size of the buffer
  AlignedBuffer screen_ram = align_buffer(screen_size_bytes);

  if (screen_ram.aligned_ptr){
    printf("Original pointer: %p\n", screen_ram.original_ptr);
    printf("Aligned pointer: %p\n", screen_ram.aligned_ptr);
  }
  else{
    printf("Memory allocation failed.\n");
    return 1;
  }

  void *physbase = Physbase();
  void *logbase = screen_ram.aligned_ptr;

  unsigned short old_palette[16];
  get_current_palette(old_palette); 

  unsigned char fill_sequence[8] = {255,255,255,255,255,255,255,255};
  fill(logbase, screen_size_bytes, fill_sequence);
  
  printf("logbase: %p\n", logbase);
  printf("physbase: %p\n", physbase);
  Setscreen(logbase, physbase, 0);

  // Setcolor(0,0x0000);
  // Setcolor(1,0x0700);
  // Setcolor(2,0x0700);
  // Setcolor(3,0x0700);
  // Setcolor(4,0x0700);
  // Setcolor(5,0x0700);
  // Setcolor(6,0x0700);
  // Setcolor(7,0x0700);
  // Setcolor(8,0x0700);
  // Setcolor(9,0x0700);
  // Setcolor(10,0x0700);
  // Setcolor(11,0x0700);
  // Setcolor(12,0x0700);
  // Setcolor(13,0x0700);
  // Setcolor(14,0x0700);
  // Setcolor(15,0x0700);

  printf("other page!\n");
  Setscreen(physbase, logbase, 0);
  for (char i = 0; i < 3; i++)
  {
    Setscreen(logbase, physbase, 0);
    sleep(1);
    Setscreen(physbase, logbase, 0);
    sleep(1);
  }
  Setscreen(physbase, physbase, 0);
  getchar();
  Setscreen(physbase, physbase, 0);
  Setpalette(old_palette);
  free(screen_ram.original_ptr);
  return 0;
}
