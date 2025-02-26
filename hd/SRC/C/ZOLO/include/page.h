#ifndef PAGE_H
#define PAGE_H

#include <image.h>

#define MAX_SEGMENTS 100

typedef struct {
  Image image;
  void* base;
  word last_cx;
  word last_cy;
} Page;

typedef struct {
  addr src;
  addr dest;
} HwallSegmentDef;

typedef struct {
  addr start_addr;
  addr end_addr;
} VwallSegmentDef;

typedef struct {
  Image image;
  void* base;
  word num_vwalls;
  word num_hwalls;
  HwallSegmentDef hwall_segments[MAX_SEGMENTS];
  VwallSegmentDef vwall_segments[MAX_SEGMENTS];
} Page2;

Page new_page(Image image);
void swap_pages(Page* logical, Page* physical);

Page2* new_page2(Image image);
void swap_pages2(Page2** logical, Page2** physical);

#endif