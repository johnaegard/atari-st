#include <stdio.h>
#include <stdlib.h>
#include <osbind.h>
#include <string.h>
#include <unistd.h>
#include <stddef.h>
#include <time.h>
#include <mint/linea.h>

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

void lineablit(void* src_base, short src_x, short src_y, short src_w, short src_h,
  void* dest_base, short dest_x, short dest_y,
  bool log) {

  if (log) {
    fprintf(log_file, "src_base=%p, dest_base=%p\n");
  }
}


int main() {

  log_file = fopen("sprite.log", "w");
  if (log_file == NULL) {
    perror("Error opening log file");
    return EXIT_FAILURE;
  }
  Cursconf(0, 1);

  Screen sprite_screen = read_degas_file(".\\RES\\SPRT.PI1");
  AlignedBuffer sprite_buffer = new_aligned_buffer(32000);
  put_screen(sprite_screen,sprite_buffer.aligned_ptr);

  OP_TAB optab = {
    .fg0bg0 = 0X03,
    .fg0bg1 = 0X03,
    .fg1bg0 = 0X03,
    .fg1bg1 = 0X03
  };

  SDDB source = {
    .bl_xmin = 257,
    .bl_ymin = 193,
    .bl_form = (char *) sprite_buffer.aligned_ptr,
    .bl_nxwd = 8,
    .bl_nxln = 160,
    .bl_nxpl = 2
  };

  SDDB dest = {
    .bl_xmin = 100,
    .bl_ymin = 100,
    .bl_form = (char *) Physbase(),
    .bl_nxwd = 8,
    .bl_nxln = 160,
    .bl_nxpl = 2
  };

  BBPB bbpb = {
   .bb_b_wd = 7,           /*	 width of block in pixels 		     */
   .bb_b_ht = 7,           /*	 height of block in pixels		     */
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
  getchar();

  // source.bl_xmin = source.bl_xmin + 8;
  // bbpb.bb_b_ht =7;
  // bbpb.bb_b_wd =7;

  SDDB source2 = {
    .bl_xmin = 265,
    .bl_ymin = 193,
    .bl_form = (char *) sprite_buffer.aligned_ptr,
    .bl_nxwd = 8,
    .bl_nxln = 160,
    .bl_nxpl = 2
  };

  SDDB dest2 = {
    .bl_xmin = 132,
    .bl_ymin = 100,
    .bl_form = (char *) Physbase(),
    .bl_nxwd = 8,
    .bl_nxln = 160,
    .bl_nxpl = 2
  };

  BBPB bbpb2 = {
   .bb_b_wd = 7,           /*	 width of block in pixels 		     */
   .bb_b_ht = 7,           /*	 height of block in pixels		     */
   .bb_plane_ct = 4,       /*	 number of planes to blit 		     */
   .bb_fg_col =1,          /*	 foreground color 			           */
   .bb_bg_col=0,           /*	 back	ground color 			            */
   .bb_op_tab=optab,       /*	 logic for fg x bg combination 	    */
   .bb_s=source2,           /*	 source info block			            */
   .bb_d=dest2,             /*	 destination info block 		        */
   .bb_p_addr=0,           /*	 pattern buffer address 		       */
   .bb_p_nxln=0,           /*	 offset to next line in pattern 	     */
   .bb_p_nxpl=0,           /*	 offset to next plane in pattern 	     */
   .bb_p_mask=0            /*	 pattern index mask 			     */
  };

  linea7(&bbpb2);

  getchar();
  // memset(bbpb.bb_fill, 0, sizeof(bbpb.bb_fill));  

  // linea7(&bbpb);
  // getchar();

  free_screen(sprite_screen);
  free_aligned_buffer(sprite_buffer);
  return 0;
}