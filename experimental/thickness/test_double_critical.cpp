#include "include/minsegdist.h"
#include "include/segdist.h"

int main() {
  Vector3 a0(0,1,0),a1(-.5,0,0), a2(0,-1,0), b0(1,0,1), b1(1.5,0,0), b2(1,0,-1);
  Vector3 a0p(0,1,0),a1p(-.5,0,0), a2p(0,-1,0), b0p(1,1,0), b1p(1.5,0,0), b2p(1,-1,0);
  float t1,t2;
  // These two cases ARE double critical
  if (!double_critical_test_v2(a0,a1,a2,b0,b1,b2))
    cerr << "1 ERROR ERROR\n";
  if (!double_critical_test_v2(a0p,a1p,a2p,b0p,b1p,b2p))
    cerr << "2 ERROR ERROR\n";

  // This case is not
  Vector3 c0(0,-1,0),c1(0,1,0),ct0(-0.001,1,0),ct1(0.001,1,0);
  ct0.normalize(); ct1.normalize();
  Vector3 d0(1,-5,0),d1(2,-5,0),dt0(1,0.001,0),dt1(1,-0.001,0);
  dt0.normalize(); dt1.normalize();
  if (double_critical_test(c0,c1,ct0,ct1,d0,d1,dt0,dt1))
    cerr << "0 " << c0 << " " << c1 << " " << d0 << " " << d1 << " 0"<<endl;

}
