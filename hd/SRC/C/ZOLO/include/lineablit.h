#ifndef LINEABLIT_H
#define LINEABLIT_H

#include <stdbool.h>

void lineablit(void* src_base, short src_x, short src_y, short src_w, short src_h,
  void* dest_base, short dest_x, short dest_y,
  bool log);

#endif