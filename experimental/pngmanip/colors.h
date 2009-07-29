#ifndef __COLORS_H__
#define __COLORS_H__

#include <math.h>

typedef struct RGB {
  unsigned char r,g,b;
} RGB;

inline void set_rgb(RGB* c, unsigned char r, unsigned char g, unsigned char b) {
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

inline float b2f(unsigned char c) {
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
  RGB c2;float r,g,b,r2,g2,b2;
  unsigned int ri,gi,bi;

  const float eps = 1./100.;
  const float eps2 = (eps-1.)/eps;

  map_color_sine(c,t,0,1);
  if (t>1-eps) {
    // Map t to [0,1]
    t2 = 1./eps*t + eps2;
    map_color_rainbow(&c2,t2,0,1);
    r=b2f(c->r);g=b2f(c->g);b=b2f(c->b);
    r2=b2f(c2.r);g2=b2f(c2.g);b2=b2f(c2.b);

    // fade color in with sharp ramp
    const float fadein = .1;
    if (t2<fadein) {
      t2 /= fadein;
      ri = (unsigned int)rintf(r*(1.-t2) + t2*r2);
      gi = (unsigned int)rintf(g*(1.-t2) + t2*g2);
      bi = (unsigned int)rintf(b*(1.-t2) + t2*b2);
    }
    else {
      ri = (unsigned int)rintf(r2);
      gi = (unsigned int)rintf(g2);
      bi = (unsigned int)rintf(b2);
    }
    set_rgb(c,(unsigned char)ri,
              (unsigned char)gi,
              (unsigned char)bi);

  }
}

inline void map_color_rainbow_fast(RGB* c, float val, float min, float max) {
  // blue low, red hight, linear in between
  float lval = val;
  if (val>max) lval = max;
  if (val<min) lval = min;
  float t = (lval-min)/(max-min);

  const float num_reps = 10;

  while (t>1./num_reps) {
    t -= 1./num_reps;
  }
  t *= num_reps;
  map_color_rainbow_cycle(c, t, 0, 1);
}

inline void map_color_sine_acc(RGB* c, float val, float min, float max) {
  // blue low, red hight, linear in between
  float lval = val;
  if (val>max) lval = max;
  if (val<min) lval = min;
  float t = (lval-min)/(max-min);

  const float num_reps = 80;

  set_rgb(c,(unsigned char)(255.*(sin(num_reps*M_PI*t*t+M_PI)*.5+.5)),
            (unsigned char)(255.*(sin(num_reps*2*M_PI*t*t*t)*.5+.5)),
            (unsigned char)(255.*t*t));
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

inline void map_bw(RGB* c, float val, float min, float max) {
  // blue low, red hight, linear in between

  float lval = val;
  if (val>max) lval = max;
  if (val<min) lval = min;
  float t = (lval-min)/(max-min), s;

  s = cos(2*M_PI*10.*t);
	if (s>.8)
    set_rgb(c,0,0,0);
	else
		set_rgb(c,255,255,255);
}



#endif
