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

int main() {
  Screen sprite_screen = read_degas_file(".\\RES\\SPRT.PI1");
  AlignedBuffer sprite_buffer = new_aligned_buffer(32000);
  put_screen(sprite_screen,sprite_buffer.aligned_ptr);

  OP_TAB optab = {
    .fg0bg0 = 0X03,
    .fg0bg1 = 0X03,
    .fg1bg0 = 0X03,
    .fg1bg1 = 0X03
  };

	// short	bl_xmin;		/* Minimum x			*/
	// short	bl_ymin;		/* Minimum y 			*/
	// char	*bl_form;		/* short aligned memory form 	*/
	// short	bl_nxwd;		/* Offset to next word in line  */
	// short 	bl_nxln;		/* Offset to next line in plane */
	// short 	bl_nxpl;		/* Offset to next plane 	*/

  SDDB source = {
    .bl_xmin = 0,
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

  //  short   bb_b_wd;     /*	 width of block in pixels 		     */
  //  short   bb_b_ht;     /*	 height of block in pixels		     */
  //  short   bb_plane_ct; /*	 number of planes to blit 		     */
  //  short   bb_fg_col;   /*	 foreground color 			     */
  //  short   bb_bg_col;   /*	 back	ground color 			     */
  //  OP_TAB  bb_op_tab;   /*	 logic for fg x bg combination 		     */
  //  SDDB	   bb_s;        /*	 source info block			     */
  //  SDDB	   bb_d;        /*	 destination info block 		     */
  //  short*  bb_p_addr;  /*	 pattern buffer address 		     */
  //  short   bb_p_nxln;   /*	 offset to next line in pattern 	     */
  //  short   bb_p_nxpl;   /*	 offset to next plane in pattern 	     */
  //  short   bb_p_mask;   /*	 pattern index mask 			     */
  //  char	   bb_fill[24];	/*	 work space				     */

  linea7(&bbpb);
  getchar();
  free_screen(sprite_screen);
  free_aligned_buffer(sprite_buffer);
  return 0;
}