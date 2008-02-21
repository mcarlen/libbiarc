#include "png.h"
#include <math.h>
#include "colors.h"
#include "pngwrite.h"
#include <stdlib.h>

unsigned int i,j,w,h;
unsigned char *px, *src;
char outname[40] = "gradients.png";
RGB c;

int main(int argc, char** argv) {

  w = 500; h = 250;
  px = (unsigned char*)malloc(sizeof(unsigned char)*3*w*h);

  float pttable[w][h];

  for (j=0;j<h/5;j++) {
    src = px + j*(3*w);
    for (i=0;i<w;i++) {
      map_color_rainbow(&c,(float)i/(float)w,0,1);
      *src++ = c.r; *src++ = c.g; *src++ = c.b;
    }
  }

  for (j=0;j<h/5;j++) {
    src = px + (h/5+j)*(3*w);
    for (i=0;i<w;i++) {
      map_color_rainbow_cycle(&c,(float)i/(float)w,0,1);
      *src++ = c.r; *src++ = c.g; *src++ = c.b;
    }
  }

  for (j=0;j<h/5;j++) {
    src = px + (2*h/5+j)*(3*w);
    for (i=0;i<w;i++) {
      map_color_fine(&c,(float)i/(float)w,0,1);
      *src++ = c.r; *src++ = c.g; *src++ = c.b;
    }
  }
 
  for (j=0;j<h/5;j++) {
    src = px + (3*h/5+j)*(3*w);
    for (i=0;i<w;i++) {
      map_color_sine(&c,(float)i/(float)w,0,1);
      *src++ = c.r; *src++ = c.g; *src++ = c.b;
    }
  }

  for (j=0;j<h/5;j++) {
    src = px + (4*h/5+j)*(3*w);
    for (i=0;i<w;i++) {
      map_color_sine_end(&c,(float)i/(float)w,0,1);
      *src++ = c.r; *src++ = c.g; *src++ = c.b;
    }
  }

  sp_png_write_rgb(outname,px,w,h,0.1,0.1,3*w);
  free(px);

}
