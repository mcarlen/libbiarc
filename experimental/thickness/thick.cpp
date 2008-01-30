#include "Curve.h"
#include "knot.h"

#include "curvealgos.h"
#include "minsegdist.h"
#include "segdist.h"

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

int main(int argc,char** argv) {

  if (argc!=2) {
    cout << "Usage : " << argv[0] << " pkffile\n";
    exit(1);
  }

  cout.precision(24);

  double tick;

  Curve<Vector3> curve(argv[1]);
  curve.link();
  curve.make_default();
  Vector3 dum1,dum2;
  tick = start_time();
  double MatD = compute_thickness(&curve,&dum1,&dum2);
  tick = stop_time(tick);
  cout << "Mat   " << MatD << " ";
  cout.precision(6); cout << tick << endl;

  CKnot k;
  k.ReadPKF(argv[1]);
  tick = start_time();
  double BenD = k.MinSegDistance();
  tick = stop_time(tick);

  cout.precision(24);
  cout << "Ben   " << BenD << " ";
  cout.precision(6); cout << tick << endl;

  return 0;

}
