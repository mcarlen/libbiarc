#ifndef __TIMER_H__
#define __TIMER_H__

#include <time.h>

// start timer and return start time
double start_time() {
  struct timespec tp;
  clock_gettime(CLOCK_REALTIME, &tp);
  return 1e9*tp.tv_sec + tp.tv_nsec;
}

// return time in seconds
double stop_time(double begin) {
  struct timespec tp;
  clock_gettime(CLOCK_REALTIME, &tp);
  double end = 1e9*tp.tv_sec + tp.tv_nsec;
  return (end-begin)*1e-9;
}

#endif // __TIMER_H__
