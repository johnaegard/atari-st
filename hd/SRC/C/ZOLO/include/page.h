#ifndef PAGE_H
#define PAGE_H

#include <image.h>

typedef struct {
  Image image;
  void* base;
  word last_cx;
  word last_cy;
} Page;

typedef struct {
  addr src;
  addr dest;
  word num_bytes;
} MemcpyDef;

typedef struct {
  Image image;
  void* base;
  word num_memcopies;
  word max_memcopies;
  word last_cx;
  word last_cy;
  MemcpyDef memcopies[];
} Page2;

Page new_page(Image image);
void swap_pages(Page* logical, Page* physical);

Page2* new_page2(Image image, word max_memcopies);
void swap_pages2(Page2** logical, Page2** physical);

#endif