#ifndef PAGE_H
#define PAGE_H

#include <image.h>

typedef struct {
    Image image;
    void* base;
    word last_cx;
    word last_cy;
} Page;

Page new_page(Image image);
void swap_pages(Page* logical, Page* physical);

#endif