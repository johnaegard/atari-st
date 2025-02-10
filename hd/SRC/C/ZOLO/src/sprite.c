#include <sprite.h>
#include <lineablit.h>
#include <image.h>

void render_sprite(Sprite* sprite, word sprite_frame, word screen_x, word screen_y, Page* target) {

  word frame_src_x = sprite->src_x + (sprite_frame * sprite->frame_src_x_offset);
  word frame_src_y = sprite->src_y + (sprite_frame * sprite->frame_src_y_offset);

  lineablit(
    sprite->src_image.base,
    frame_src_x,
    frame_src_y,
    sprite->src_h,
    sprite->src_y,
    target->base,
    screen_x,
    screen_y,
    false);
}