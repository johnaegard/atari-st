#include <stdio.h>
#include <lineablit.h>
#include <zolo-types.h>
#include <screen.h>

int main() {
  Cursconf(0, 1);
  Screen sprite_screen = make_screen_from_degas_file(".\\RES\\SPRT.PI1");
  AlignedBuffer sprite_buffer = new_aligned_buffer(32000);
  copy_screen_to_base(sprite_screen,sprite_buffer.aligned_ptr);
  lineablit(sprite_buffer.aligned_ptr,257,193,7,7,Physbase(),100,100,true);
  getchar();
}