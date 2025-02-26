#include <page.h>

Page tmppage;

Page new_page(Image image) {
  Page page = {.image = image, .base = image.base};
  return page;
}

Page2* new_page2(Image image) {
  Page2* page = (Page2*)Malloc(sizeof(Page2));
  page->image = image;
  page->base = image.base;
  page->num_hwalls = 0;
  page->num_vwalls = 0;
  return page;
}

void swap_pages(Page* logical, Page* physical) {
  tmppage = *physical;
  *physical = *logical;
  *logical = tmppage;
  Setscreen(logical->base, physical->base, -1);
}

void swap_pages2(Page2** logical, Page2** physical) {
  Page2* tmp = *physical;
  *physical = *logical;
  *logical = tmp;
}