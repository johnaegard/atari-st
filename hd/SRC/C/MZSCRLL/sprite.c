#include <stdio.h>
#include <stdlib.h>
#include <osbind.h>
#include <string.h>
#include <unistd.h>
#include <stddef.h>
#include <time.h>
#include <mint/linea.h>
#include <stdbool.h>

typedef unsigned char byte;
typedef unsigned short word;

typedef struct {
  void* aligned_ptr;
  void* original_ptr;
} AlignedBuffer;

typedef struct {
  unsigned short* palette;
  unsigned short* bitmap;
} Image;

FILE* log_file;

AlignedBuffer new_aligned_buffer(size_t size) {
  void* original_ptr = malloc(size + 255);
  if (!original_ptr) {
    return (AlignedBuffer) { NULL, NULL }; // Return NULL if malloc fails
  }
  memset(original_ptr, 0, size + 255);
  unsigned long addr = (unsigned long)original_ptr;
  unsigned long aligned_addr = (addr + 255) & ~255;

  AlignedBuffer buffer = { (void*)aligned_addr, original_ptr };


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
Image read_degas_file(const char* filename) {
  Image screen; // Struct to hold the arrays

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
Image make_screen_from_base(void* addr) {
  Image screen;
  screen.palette = (unsigned short*)malloc(16 * sizeof(unsigned short));
  screen.bitmap = (unsigned short*)malloc(16000 * sizeof(unsigned short));
  get_current_palette(screen.palette);
  memcpy(screen.bitmap, addr, 32000);
  return screen;
}
void copy_image_to_base(Image screen, void* buffer) {
  Setpalette(screen.palette);
  memcpy(buffer, screen.bitmap, 32000);
}
void free_image(Image screen) {
  free(screen.bitmap);
  free(screen.palette);
}
void clear_image(Image screen) {
  memset(screen.bitmap, 0, 32000);
}

const OP_TAB optab = {
    .fg0bg0 = 0X03,
    .fg0bg1 = 0X03,
    .fg1bg0 = 0X03,
    .fg1bg1 = 0X03
};

SDDB source = {
  .bl_nxwd = 8,
  .bl_nxln = 160,
  .bl_nxpl = 2
};

SDDB dest = {
  .bl_nxwd = 8,
  .bl_nxln = 160,
  .bl_nxpl = 2
};

void lineablit(void* src_base, short src_x, short src_y, short src_w, short src_h,
  void* dest_base, short dest_x, short dest_y,
  bool log) {

  if (log) {
    fprintf(log_file, "src_base=%p, dest_base=%p\n",src_base,dest_base);
  }

  source.bl_form = (char *) src_base; 
  source.bl_xmin = src_x;
  source.bl_ymin = src_y;

  dest.bl_form = (char *) dest_base; 
  dest.bl_xmin = dest_x;
  dest.bl_ymin = dest_y;

  BBPB bbpb = {
   .bb_b_wd = src_w,           /*	 width of block in pixels 		     */
   .bb_b_ht = src_h,           /*	 height of block in pixels		     */
   .bb_plane_ct = 4,       /*	 number of planes to blit 		     */
   .bb_fg_col =1,          /*	 foreground color 			           */
   .bb_bg_col=0,           /*	 back	ground color 			            */
   .bb_op_tab=optab,       /*	 logic for fg x bg combination 	    */
   .bb_s=source,           /*	 source info block			            */
   .bb_d=dest,             /*	 destination info block 		        */
   .bb_p_addr=0,           /*	 pattern buffer address 		       */
   .bb_p_nxln=0,           /*	 offset to next line in pattern 	     */
   .bb_p_nxpl=0,           /*	 offset to next plane in pattern 	     */
   .bb_p_mask=0            /*	 pattern index mask 			     */
  };

  linea7(&bbpb);
}

int main() {

  log_file = fopen("sprite.log", "w");
  if (log_file == NULL) {
    perror("Error opening log file");
    return EXIT_FAILURE;
  }
  Cursconf(0, 1);

  Image sprite_screen = read_degas_file(".\\RES\\SPRT.PI1");
  AlignedBuffer sprite_buffer = new_aligned_buffer(32000);
  copy_image_to_base(sprite_screen,sprite_buffer.aligned_ptr);

  lineablit(sprite_buffer.aligned_ptr,257,193,7,7,Physbase(),100,100,true);
  getchar();
  lineablit(sprite_buffer.aligned_ptr,265,193,7,7,Physbase(),100,100,true);
  getchar();

  free_image(sprite_screen);
  free_aligned_buffer(sprite_buffer);
  return 0;
}