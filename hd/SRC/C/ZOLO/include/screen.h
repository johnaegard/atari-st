#ifndef SCREEN_H
#define SCREEN_H

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
} Screen;

typedef struct {
    void* base;
    word last_cx;
    word last_cy;
} Base;

AlignedBuffer new_aligned_buffer(size_t size);
void free_aligned_buffer(AlignedBuffer buffer);
void get_current_palette(word* palette);
void dump_degas_file(word* palette, void* base);
Screen make_screen_from_degas_file(const char* filename);
Screen copy_screen(Screen source);
void copy_screen_to_base(Screen screen, void* buffer);
void free_screen(Screen screen);
void clear_screen(Screen screen);
void swap_pages(Base* logbase, Base* physbase);
void the_end(clock_t start, clock_t end, void* physbase, Screen original_screen, word frames);

#endif // SCREEN_H
