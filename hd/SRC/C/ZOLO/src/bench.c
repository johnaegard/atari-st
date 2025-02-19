#include <16bittypes.h>
#include <image.h>
#include <sprite.h>
#include <stdbool.h>
#include <stdio.h>

void log_frames(FILE *log_file, clock_t start, clock_t end, word frames) {
  double total_time = (double)(end - start) / CLOCKS_PER_SEC;
  double fps = frames / total_time;

  fprintf(log_file, "%d frames\n%f seconds\n%f fps\n", frames, total_time, fps);
  if (fclose(log_file) != 0) {
    perror("Error closing log file");
  }
}

int main() {
  FILE *log_file;

  log_file = fopen("bench.log", "w");
  if (log_file == NULL) {
    perror("Error opening log file");
    return EXIT_FAILURE;
  }

  Cursconf(0, 1);
  word *original_palette = (word *)Malloc(16 * sizeof(word));
  get_current_palette(original_palette);
  Image background = make_image_from_degas_file(".\\RES\\BKGD.PI1");
  Image physical = duplicate_image(background);
  Image logical = duplicate_image(background);
  Page logical_page = new_page(logical);
  Page physical_page = new_page(physical);
  free_image(background);
  Image sprites = make_image_from_degas_file(".\\RES\\SPRT.PI1");
  Setpalette(sprites.palette);
  Setscreen(logical_page.base, physical_page.base, -1);

  Sprite sprite = {
                    .frame_src_x_offset = 0,
                    .frame_src_y_offset = 0,
                    .num_frames = 1,
                    .src_image = sprites,
                    .src_h = 8,
                    .src_w = 8,
                    .src_x = 0,
                    .src_y = 0
                  };

  word frames = 0;
  const clock_t start  = clock();
  const word spf = 30; 

  while (frames++ < 120) {
    Vsync();
    for(word i=0; i < spf; i++ ) {
      render_sprite(&sprite, 0, 100, 100, &logical_page);
    }
    swap_pages(&logical_page, &physical_page);
  }

  log_frames(log_file, start, clock(), frames);
  free_image(physical);
  free_image(logical);
  free_image(sprites);
  Setpalette(original_palette);
  Setscreen(physical_page.base, physical_page.base, -1);
  exit(0);
}