#include "include/minsegdist.h"
#include "include/segdist.h"

int main() {
  Vector3 a0(0,1,0), a2(0,-1,0), b0(1,0,1), b2(1,0,-1);
  float t1,t2;
  float d = min_seg_dist(a0,a2,b0,b2,t1,t2);

  cout << "Distance : " << d << " (" << t1 << "," << t2 << ")" << endl;
}
