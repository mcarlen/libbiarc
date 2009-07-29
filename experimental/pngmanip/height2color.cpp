#include "png.h"
#include <math.h>
#include "colors.h"
#include "pngwrite.h"
#include <stdlib.h>
#include <fstream>

unsigned int i,j,w,h,n;
unsigned char *px, *src;
char outname[40] = "out.png";
RGB c;
int val, dummy1, dummy2, **vals;

int main(int argc, char** argv) {

  std::ifstream in(argv[1]);

  in >> w >> h;
  px = (unsigned char*)malloc(sizeof(unsigned char)*3*w*h);
  vals = new int*[h];
  for (j=0;j<h;++j) {
    vals[j] = new int[w];
    for (i=0;i<w;++i) {
      in >> val >> dummy1 >> dummy2;
      vals[j][i] = val;
    }
  }
  in.close();

  // Rainbow
  for (j=0;j<h;j++) {
    src = px + j*(3*w);
    for (i=0;i<w;i++) {
      map_color_rainbow(&c,(float)vals[j][i]/255.,0,1);
      *src++ = c.r; *src++ = c.g; *src++ = c.b;
    }
  }
  sp_png_write_rgb("h2c_1_rainbow.png",px,w,h,0.1,0.1,3*w);

  // Rainbow cycle
  for (j=0;j<h;j++) {
    src = px + j*(3*w);
    for (i=0;i<w;i++) {
      map_color_rainbow_cycle(&c,(float)vals[j][i]/255.,0,1);
      *src++ = c.r; *src++ = c.g; *src++ = c.b;
    }
  }
  sp_png_write_rgb("h2c_1_rainbow_cycle.png",px,w,h,0.1,0.1,3*w);

  // Color fine sawtooth
  for (j=0;j<h;j++) {
    src = px + j*(3*w);
    for (i=0;i<w;i++) {
      map_color_fine(&c,(float)vals[j][i]/255.,0,1);
      *src++ = c.r; *src++ = c.g; *src++ = c.b;
    }
  }
  sp_png_write_rgb("h2c_sawtooth.png",px,w,h,0.1,0.1,3*w);
 
  // sin function
  for (j=0;j<h;j++) {
    src = px + j*(3*w);
    for (i=0;i<w;i++) {
      map_color_sine(&c,(float)vals[j][i]/255.,0,1);
      *src++ = c.r; *src++ = c.g; *src++ = c.b;
    }
  }
  sp_png_write_rgb("h2c_2_sin.png",px,w,h,0.1,0.1,3*w);

  // refined sin func
/*
  for (j=0;j<h;j++) {
    src = px + j*(3*w);
    for (i=0;i<w;i++) {
      map_color_sine_end(&c,(float)vals[j][i]/255.,0,1);
      *src++ = c.r; *src++ = c.g; *src++ = c.b;
    }
  }
  sp_png_write_rgb("h2c_sin_fine.png",px,w,h,0.1,0.1,3*w);
*/

  // rainbow fast
  for (j=0;j<h;j++) {
    src = px + j*(3*w);
    for (i=0;i<w;i++) {
      map_color_rainbow_fast(&c,(float)vals[j][i]/255.,0,1);
      *src++ = c.r; *src++ = c.g; *src++ = c.b;
    }
  }
  sp_png_write_rgb("h2c_2_rainbow_fast.png",px,w,h,0.1,0.1,3*w);

/*
  // sin accelerated
  for (j=0;j<h;j++) {
    src = px + j*(3*w);
    for (i=0;i<w;i++) {
      map_color_sine_acc(&c,(float)vals[j][i]/255.,0,1);
      *src++ = c.r; *src++ = c.g; *src++ = c.b;
    }
  }
  sp_png_write_rgb("h2c_sin_acc.png",px,w,h,0.1,0.1,3*w);
*/

  // contour plot
  for (j=0;j<h;j++) {
    src = px + j*(3*w);
    for (i=0;i<w;i++) {
      map_bw(&c,(float)vals[j][i]/255.,0,1);
      *src++ = c.r; *src++ = c.g; *src++ = c.b;
    }
  }
  sp_png_write_rgb("h2c_2_heightmap.png",px,w,h,0.1,0.1,3*w);

  free(px);

}

