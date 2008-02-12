#include "Curve.h"
#include "curvealgos.h"

#include "knot.h"

BOOL NeedToCalculate(const CArc &a,const CArc &b,double dCurrentMinimum);

int main() {
#if 1
  CKnotVec3 a0(-0.841446, -0.25, 0), a1(-0.75, -0.5, 0);
  CKnotVec3 at0(0.225598, -0.97422, 0), at1(0.447214, -0.894427, 0);

  CKnotVec3 b0(0.841446, -0.25, 0), b1(0.75, -0.5, 0);
  CKnotVec3 bt0(-0.225598, -0.97422, 0), bt1(-0.447214, -0.894427, 0);


  CArc arc1(a0,a1,at0);
  CArc arc2(b0,b1,bt0);

  cout << "BEN : ";
  if (NeedToCalculate(arc1,arc2,1e8))
    cout << "PROBLEM\n";
  else cout << "OK\n";
#endif 

  Vector3 a0p(-0.841446, -0.25, 0), a1p(-0.75, -0.5, 0);
  Vector3 at0p(0.225598, -0.97422, 0), at1p(0.447214, -0.894427, 0);

  Vector3 b0p(0.841446, -0.25, 0), b1p(0.75, -0.5, 0);
  Vector3 bt0p(-0.225598, -0.97422, 0), bt1p(-0.447214, -0.894427, 0);

  cout << "MC  : ";
  if (double_critical_test(a0p,a1p,at0p,at1p,b0p,b1p,bt0p,bt1p))
    cout << "PROBLEM\n";
  else cout << "OK\n";

  vector<Candi<Vector3> > C,CC;

  float cosa = (a1p-a0p).dot(at0p);
  cosa = cosa/(a1p-a0p).norm();
  float l = (a1p-a0p).norm()/2./cosa;

  Vector3 xxx = a0p+l*at0p;

  cosa = (b1p-b0p).dot(bt0p);
  cosa = cosa/(b1p-b0p).norm();
  l = (b1p-b0p).norm()/2./cosa;

  Vector3 yyy = b0p+l*bt0p;
  C.push_back(Candi<Vector3>(a0p,xxx,a1p,b0p,yyy,b1p));

  cout << "MC2 : ";
  if (double_critical_test_v2(a0p,xxx,a1p,b0p,yyy,b1p))
    cout << "PROBLEM\n";
  else cout << "OK\n";

/*
  dbl_crit_filter(C,CC);
  if (CC.size()>0) cout << "MC  : PROBLEM " << CC.size() << endl;
*/
}
