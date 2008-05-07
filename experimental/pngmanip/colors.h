#ifndef __COLORS_H__
#define __COLORS_H__

#include <math.h>

typedef struct RGB {
  char r,g,b;
} RGB;

inline void set_rgb(RGB* c, char r, char g, char b) {
  c->r = r; c->g = g; c->b = b;
}

inline void map_color_rainbow(RGB* c, float val, float min, float max) {
  // blue low, red hight, linear in between
  float lval = val;
  if (val>max) lval = max;
  if (val<min) lval = min;

  float t4=((lval-min)/(max-min))*4.;
  int i = (int)t4;
  float v = t4-(float)i;
  switch (i) {
    case 0:
      set_rgb(c,0,(unsigned int)(255.*v),255);
      break;
    case 1:
      set_rgb(c,0,255,(unsigned int)(255.*(1.-v)));
      break;
    case 2:
      set_rgb(c,(unsigned int)(255.*v),255,0);
      break;
    case 3:
      set_rgb(c,255,(unsigned int)(255.*(1.-v)),0);
      break;
    case 4:
      set_rgb(c,255,0,0);
      break;
  }
}

inline void map_color_rainbow_cycle(RGB* c, float val, float min, float max) {
  // blue low, red hight, linear in between
  float lval = val;
  if (val>max) lval = max;
  if (val<min) lval = min;

  float t6=((lval-min)/(max-min))*6.;
  int i = (int)t6;
  float v = t6-(float)i;
  switch (i) {
    case 0:
      set_rgb(c,0,(unsigned int)(255.*v),255);
      break;
    case 1:
      set_rgb(c,0,255,(unsigned int)(255.*(1.-v)));
      break;
    case 2:
      set_rgb(c,(unsigned int)(255.*v),255,0);
      break;
    case 3:
      set_rgb(c,255,(unsigned int)(255.*(1.-v)),0);
      break;
    case 4:
      set_rgb(c,255,0,(unsigned int)(255.*(v)));
      break;
    case 5:
      set_rgb(c,(unsigned int)(255.*(1.-v)),0,255);
      break;
    case 6:
      set_rgb(c,0,0,255);
      break;
  }
}


inline void map_color_fine(RGB* c, float val, float min, float max) {
  // blue low, red hight, linear in between
  float lval = val;
  if (val>max) lval = max;
  if (val<min) lval = min;

  float t = (lval-min)/(max-min);
  int i = (int)(t*20.);
  float v = t*20.0-(float)i;

  set_rgb(c,(unsigned char)(255.*t),
            (unsigned char)(255.*v),
            (unsigned char)(255.*(1.-v)*(1.-t)));
}

inline void map_color_sine(RGB* c, float val, float min, float max) {
  // blue low, red hight, linear in between
  float t = (val-min)/(max-min);

  set_rgb(c,(unsigned char)(255.*t),
            (unsigned char)(255.*(.5*sin(2*M_PI*20.*t-M_PI)+.5)),
            (unsigned char)(255.*(.5*sin(2*M_PI*20.*t)+.5)*(1.-t)));
}

float b2f(unsigned char c) {
  int i,v=0;
  for (i=0;i<8;i++) v += (1<<i&c);
  return (float)v;
}

inline void map_color_sine_end(RGB* c, float val, float min, float max) {
  // blue low, red hight, linear in between
  float lval = val;
  if (val>max) lval = max;
  if (val<min) lval = min;
  float t = (lval-min)/(max-min),t2;
  RGB c2;float r,g,b;
  unsigned int ri,gi,bi;

  const float STRIPELEN = 80.0;

  if (t>(STRIPELEN-1.)/STRIPELEN) {
    t2 = (1.-t)*STRIPELEN;
float t3 = t2-.5;
if (t3<0.) t3 = 1.+t3;
    map_color_rainbow_cycle(&c2,t3,0,1);
    r=b2f(c2.r);g=b2f(c2.g);b=b2f(c2.b);
    ri = (unsigned int)rintf(r*(1.-t2) + t2*t*255.);
    gi = (unsigned int)rintf(g*(1.-t2) + t2*255.*(.5*sin(2*M_PI*20.*t-M_PI)+.5));
    bi = (unsigned int)rintf(b*(1.-t2) + t2*255.*(.5*sin(2*M_PI*20.*t)+.5)*(1.-t));
    set_rgb(c,(unsigned char)ri,
              (unsigned char)gi,
              (unsigned char)bi);
  }
  else {
  set_rgb(c,(unsigned char)(255.*t),
            (unsigned char)(255.*(.5*sin(2*M_PI*20.*t-M_PI)+.5)),
            (unsigned char)(255.*(.5*sin(2*M_PI*20.*t)+.5)*(1.-t)));
  }
}

// Black and white scale from 0 (black) to 1 (white)
inline void height_map(RGB* c, float val, float min, float max) {
  // blue low, red hight, linear in between
  float lval = val;
  if (val>max) lval = max;
  if (val<min) lval = min;
  float t = (lval-min)/(max-min);

  unsigned int ri,gi,bi;

  ri = (unsigned int)rintf(t*255.);
  gi = (unsigned int)rintf(t*255.);
  bi = (unsigned int)rintf(t*255.);

  set_rgb(c,(unsigned char)ri,
            (unsigned char)gi,
            (unsigned char)bi);
}


#endif
