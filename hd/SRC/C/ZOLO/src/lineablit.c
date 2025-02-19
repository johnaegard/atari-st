#include <stdbool.h>
#include <stdio.h>
#include <mint/linea.h>

typedef unsigned char byte;
typedef unsigned short word;

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

FILE* log_file;

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
   .bb_b_wd = src_w,       /*	 width of block in pixels 		     */
   .bb_b_ht = src_h,       /*	 height of block in pixels		     */
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