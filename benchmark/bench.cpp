#include <time.h>
#include <stdlib.h>
#ifndef BENCH_BEN
#include "Curve.h"
#else
#include "knot.h"
#endif

#define rand01() ((float)rand()/(float)RAND_MAX)
#define randmod(N) (rand()%(N))

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

#ifdef BENCH_BEN
  CKnotComponent* c;
#else
  Curve<Vector3> c;
#endif

int N,Nodes;

#ifdef BENCH_BEN
CKnotPoint b;
#else
Biarc<Vector3> b;
#endif

void access() {
  int j;
  for (int i=0;i<N;i++) {
    j = randmod(Nodes);
#ifdef BENCH_BEN
    b = c->Get(j);
#else
    b = c[j];
#endif
  }
}

int main(int argc, char** argv) {

  if (argc!=3) { cout << argv[0] << " AccessN Nodes\n"; exit(-1); }
  N = atoi(argv[1]); Nodes = atoi(argv[2]);

#ifdef BENCH_BEN
  c = new CKnotComponent(N,N,NULL,0);
#endif

  srand(time(NULL));
  for (int i=0;i<Nodes;i++) {
#ifndef BENCH_BEN
    c.append(Biarc<Vector3>(Vector3(rand01(),rand01(),rand01()),(Vector3(rand01(),rand01(),rand01()))));
#else
    c->SetNC(i,rand01(),rand01(),rand01(),
               rand01(),rand01(),rand01());
#endif
  }

  double t = start_time();
  access();
  cout << stop_time(t) << endl;

  return 0;

}
