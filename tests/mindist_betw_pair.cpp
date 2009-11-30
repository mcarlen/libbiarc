#include "Curve.h"
#include "algo_helpers.h"

int main() {
  Vector3 from,to;

  Vector3 a0(0,0,0), a1(1,1,0), a2(2,0,0);
  Vector3 b0(0,2.5,0), b1(1,1.5,0), b2(2,2.5,0);

  cout << "A : " << a0 << "/" << a1 << "/"<<a2<<endl;
  cout << "B : " << b0 << "/" << b1 << "/"<<b2<<endl;

  Vector3 mid1 = (a0+a1)/2.;
  Vector3 mid2 = (a1+a2)/2.;
  Vector3 ma = (mid1+mid2)/2.;
  cout << "arc 0 " << mid1 << " / " << mid2 << " / " << ma  << endl;
  mid1 = (b0+b1)/2.; mid2 = (b1+b2)/2.; Vector3 mb = (mid1+mid2)/2.;
  cout << "arc 1 " << mid1 << " / " << mid2 << " / " << mb  << endl;

  cout << "Bound " << (ma-mb).norm() << endl;
  cout << mindist_between_bezier_arcs(a0,a1,a2,b0,b1,b2,
                                      (ma-mb).norm()*1.1,&from,&to) << endl;
   cout << from << " -> " << to << endl;
  
}
