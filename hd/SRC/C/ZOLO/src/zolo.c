#include <stdio.h>
#include <lineablit.h>
#include <16bittypes.h>
#include <screen.h>
#include <maze.h>

int main() {
  Cursconf(0, 1);

  word *original_palette = (word*)Malloc(16 * sizeof(word));
  get_current_palette(original_palette);
  Image background = make_image_from_degas_file(".\\RES\\BKGD.PI1");
  Image physical = duplicate_image(background);
  Image logical = duplicate_image(background);
  Page logical_page = new_page(logical);
  Page physical_page = new_page(physical);
  free_image(background);
  Image sprites = make_image_from_degas_file(".\\RES\\SPRT.PI1");
  Setpalette(sprites.palette);
  Setscreen(physical_page.base,physical_page.base,-1);

  MazeRenderConf maze_render_conf = {
    .viewport_width_px = VIEWPORT_WIDTH_PX
  };

  Maze maze = generate_maze(32,32);
  render_maze(true, &maze, 430, 430, &physical_page, &sprites, false,0);
  lineablit(sprites.base,257,193,7,7,physical_page.base,100,100,true);
  getchar();
  lineablit(sprites.base,265,193,7,7,physical_page.base,100,100,true);
  getchar();
  lineablit(sprites.base,273,193,7,7,physical_page.base,100,100,true);
  getchar();
  lineablit(sprites.base,281,193,7,7,physical_page.base,100,100,true);
  getchar();
  lineablit(sprites.base,289,193,7,7,physical_page.base,100,100,true);
  getchar();
  lineablit(sprites.base,297,193,7,7,physical_page.base,100,100,true);
  getchar();
  lineablit(sprites.base,305,193,7,7,physical_page.base,100,100,true);
  getchar();
  lineablit(sprites.base,313,193,7,7,physical_page.base,100,100,true);
  getchar();

  free_image(physical);
  free_image(logical);
  free_image(sprites);
  Setpalette(original_palette);
}