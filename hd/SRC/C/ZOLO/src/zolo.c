#include <stdio.h>
#include <lineablit.h>
#include <16bittypes.h>
#include <image.h>
#include <stdbool.h>
#include <maze.h>
#include <sprite.h>
#include <page.h>

const word VIEWPORT_HEIGHT_PX = 192;
const word VIEWPORT_WIDTH_PX = 224;
const word VIEWPORT_WIDTH_BYTES = 112;
const word CELL_SIZE_PX = 32;
const word MAX_MEMCOPIES = 1600;

void log_frames(FILE* log_file, clock_t start, clock_t end, word frames) {
  double total_time = (double)(end - start) / CLOCKS_PER_SEC;
  double fps = frames / total_time;

  fprintf(log_file, "%d frames\n%f seconds\n%f fps\n", frames, total_time, fps);
  if (fclose(log_file) != 0) {
    perror("Error closing log file");
  }
}

int main() {

  FILE *log_file;
  
  log_file = fopen("zolo.log", "w");
  if (log_file == NULL) {
    perror("Error opening log file");
    return EXIT_FAILURE;
  }

  Cursconf(0, 1);
  word *original_palette = (word*)Malloc(16 * sizeof(word));
  get_current_palette(original_palette);
  Image background = make_image_from_degas_file(".\\RES\\BKGD.PI1");
  Image physical = duplicate_image(background);
  Image logical = duplicate_image(background);
  Page2* logical_page_val = new_page2(logical,MAX_MEMCOPIES);
  Page2* physical_page_val = new_page2(physical,MAX_MEMCOPIES);
  Page2** logical_page = &logical_page_val;
  Page2** physical_page = &physical_page_val;
  free_image(background);
  Image sprites = make_image_from_degas_file(".\\RES\\SPRT.PI1");
  Setpalette(sprites.palette);
  // Setscreen(logical_page->base,physical_page->base,-1);
  Setscreen((*logical_page)->base,(*physical_page)->base,-1);

  fprintf(log_file,"logical base=%p\n",logical.base);
  fprintf(log_file,"physical base=%p\n",physical.base);
  fprintf(log_file,"logical page base=%p\n",logical_page_val->base);
  fprintf(log_file,"physical page base=%p\n",physical_page_val->base);
  fprintf(log_file,"logical page ptr base=%p\n",(*logical_page)->base);
  fprintf(log_file,"physical page ptr base=%p\n",(*physical_page)->base);

  MazeRenderConf maze_render_conf = {
    .viewport_width_px = VIEWPORT_WIDTH_PX,
    .viewport_height_px = VIEWPORT_HEIGHT_PX,
    .viewport_width_bytes = VIEWPORT_WIDTH_BYTES,
    .cell_size_px = CELL_SIZE_PX,
    .line_size_bytes = LINE_SIZE_BYTES
  };

    Sprite arrow = {
    .frame_src_x_offset = 8,
    .frame_src_y_offset = 0,
    .num_frames=8,
    .src_image = sprites,
    .src_h = 7,
    .src_w = 7,
    .src_x = 257,
    .src_y = 193
  };

  Maze maze = generate_maze(32,32);
  bool running = true;
  word cx = 500;
  word cy = 500;
  long frames = 0;  
  clock_t start = clock();

  while(cx > 300) {
    Vsync();

    render_vwalls(MAZE_ERASE_MODE, &maze, &maze_render_conf, cx, cy, *logical_page, &sprites, false,log_file);
    render_hwalls(MAZE_ERASE_MODE, &maze, &maze_render_conf, cx, cy, *logical_page, &sprites, false,log_file);

    render_vwalls(MAZE_DRAW_MODE, &maze, &maze_render_conf, cx, cy, *logical_page, &sprites, false, log_file);
    render_hwalls(MAZE_DRAW_MODE, &maze, &maze_render_conf, cx, cy, *logical_page, &sprites, false, log_file);

    swap_pages2(logical_page, physical_page);
    Setscreen((*logical_page)->base,(*physical_page)->base,-1);

    cx--;
    cy--;
    frames++;
  }

  log_frames(log_file,start,clock(),frames);
  Setpalette(original_palette);
  Setscreen((*physical_page)->base,(*physical_page)->base,-1);

  exit(0);
}