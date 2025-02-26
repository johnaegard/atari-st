#include <16bittypes.h>
#include <image.h>
#include <page.h>
#include <lineablit.h>

typedef struct {
  Image src_image;
  word src_x;
  word src_y;
  word frame_src_x_offset;
  word frame_src_y_offset;
  word src_w;
  word src_h;
  word num_frames;
} Sprite;

void render_sprite(Sprite* sprite, word sprite_frame, word screen_x, word screen_y, Page* target);