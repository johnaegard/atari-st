#ifndef IMAGE_H
#define IMAGE_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <mint/osbind.h>
#include <16bittypes.h>

typedef struct {
    void* aligned_ptr;
    void* original_ptr;
} AlignedBuffer;

typedef struct {
  word* palette;
  AlignedBuffer aligned_buffer;
  void* base;   
} Image;

typedef struct {
    Image image;
    void* base;
    word last_cx;
    word last_cy;
} Page;

AlignedBuffer new_aligned_buffer(size_t size);
void free_aligned_buffer(AlignedBuffer buffer);
void get_current_palette(word* palette);
void dump_degas_file(word* palette, void* base);
Image make_image_from_degas_file(const char* filename);
Image duplicate_image(Image source);
void copy_image_to_base(Image image, void* buffer);
void free_image(Image image);
void clear_image(Image image);
Page new_page(Image image);
void swap_pages(Page* logical, Page* physical);
void the_end(clock_t start, clock_t end, void* physbase, Image original_screen, word frames);

#endif // SCREEN_H
