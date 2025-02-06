#include <stdio.h>
#include <lineablit.h>
#include <16bittypes.h>
#include <screen.h>
#include <maze.h>

int main() {
  Cursconf(0, 1);

  Screen sprite_screen = make_screen_from_degas_file(".\\RES\\SPRT.PI1");
  Screen background_screen = make_screen_from_degas_file(".\\RES\\BKGD.PI1");
  Screen physical_screen = new_screen(background_screen);
  Screen logical_screen = new_screen(background_screen);

  Setpalette(sprite_screen.palette);
  Setscreen(physical_screen.base,physical_screen.base,-1);
  free_screen(background_screen);

  lineablit(sprite_screen.base,257,193,7,7,physical_screen.base,100,100,true);
  getchar();
  lineablit(sprite_screen.base,265,193,7,7,physical_screen.base,100,100,true);
  getchar();
  lineablit(sprite_screen.base,273,193,7,7,physical_screen.base,100,100,true);
  getchar();
  lineablit(sprite_screen.base,281,193,7,7,physical_screen.base,100,100,true);
  getchar();
  lineablit(sprite_screen.base,289,193,7,7,physical_screen.base,100,100,true);
  getchar();
  lineablit(sprite_screen.base,297,193,7,7,physical_screen.base,100,100,true);
  getchar();
  lineablit(sprite_screen.base,305,193,7,7,physical_screen.base,100,100,true);
  getchar();
  lineablit(sprite_screen.base,313,193,7,7,physical_screen.base,100,100,true);
  getchar();


}