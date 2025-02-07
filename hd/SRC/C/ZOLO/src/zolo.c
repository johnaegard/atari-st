#include <stdio.h>
#include <lineablit.h>
#include <16bittypes.h>
#include <screen.h>
#include <maze.h>

// const word MAZE_HEIGHT_CELLS = 32;
// const word MAZE_WIDTH_CELLS = 32;

int main() {
  Cursconf(0, 1);

  word *original_palette = (word*)Malloc(16 * sizeof(word));
  get_current_palette(original_palette);
  Screen background_screen = make_screen_from_degas_file(".\\RES\\BKGD.PI1");
  Screen physical_screen = copy_screen(background_screen);
  Screen logical_screen = copy_screen(background_screen);
  Page logical_page = new_page(logical_screen);
  Page physical_page = new_page(physical_screen);
  free_screen(background_screen);
  Screen sprite_screen = make_screen_from_degas_file(".\\RES\\SPRT.PI1");
  Setpalette(sprite_screen.palette);
  Setscreen(physical_page.base,physical_page.base,-1);

  word** maze = generate_maze(MAZE_WIDTH_CELLS, MAZE_HEIGHT_CELLS);
  render_maze(true,maze,430,430, &physical_page, sprite_screen.base,false,0);
  lineablit(sprite_screen.base,257,193,7,7,physical_page.base,100,100,true);
  getchar();
  lineablit(sprite_screen.base,265,193,7,7,physical_page.base,100,100,true);
  getchar();
  lineablit(sprite_screen.base,273,193,7,7,physical_page.base,100,100,true);
  getchar();
  lineablit(sprite_screen.base,281,193,7,7,physical_page.base,100,100,true);
  getchar();
  lineablit(sprite_screen.base,289,193,7,7,physical_page.base,100,100,true);
  getchar();
  lineablit(sprite_screen.base,297,193,7,7,physical_page.base,100,100,true);
  getchar();
  lineablit(sprite_screen.base,305,193,7,7,physical_page.base,100,100,true);
  getchar();
  lineablit(sprite_screen.base,313,193,7,7,physical_page.base,100,100,true);
  getchar();

  free_screen(physical_screen);
  free_screen(logical_screen);
  free_screen(sprite_screen);
  Setpalette(original_palette);
  Mfree(original_palette);
}