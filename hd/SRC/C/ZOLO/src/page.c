#include <page.h>

Page tmppage;

Page new_page(Image image){
  Page page = {
    .image = image,
    .base = image.base
  };
  return page;
}
void swap_pages(Page* logical, Page* physical) {
  tmppage = *physical;
  *physical = *logical;
  *logical = tmppage;
  Setscreen(logical->base, physical->base, -1);
}
