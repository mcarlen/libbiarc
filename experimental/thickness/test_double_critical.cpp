#include "include/minsegdist.h"
#include "include/segdist.h"

int main() {
  // Vector3 a0(0,1,0),a1(-.5,0,0), a2(0,-1,0), b0(1,0,1), b1(1.5,0,0), b2(1,0,-1);
  Vector3 a0(0,1,0),a1(-.5,0,0), a2(0,-1,0), b0(1,1,0), b1(1.5,0,0), b2(1,-1,0);
  float t1,t2;
  if (double_critical_test_v2(a0,a1,a2,b0,b1,b2)) {
    cerr << "IS Double Critical\n";
    cout << a0 << " " << a2 << " " << b0 << " " << b2 << endl;
  }

}
