#ifndef __PNGWRITE_H__
#define __PNGWRITE_H__

#include "png.h"

// stolen and adapted from inkscape

typedef struct SPPNGBD {
    const unsigned char *px;
    int rowstride;
} SPPNGBD;

int sp_png_get_block_stripe(const unsigned char **rows,
                            int row,
                            int num_rows,
                            void *data);

int sp_png_write_rgb_striped(const char *filename,
                             int width,
                             int height,
                             double xdpi,
                             double ydpi,
                             int (* get_rows) (
                                  const unsigned char **rows,
                                  int row,
                                  int num_rows,
                                  void *data),
                             void *data);

int sp_png_write_rgb(const char *filename,
                     const unsigned char *px,
                     int width,
                     int height,
                     double xdpi,
                     double ydpi,
                     int rowstride);

#endif
