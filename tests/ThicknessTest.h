#ifndef __BASICTEST_H
#define __BASICTEST_H

#include <cxxtest/TestSuite.h>

#include "Biarc.h"
#include "Curve.h"
#include "../experimental/thickness/include/curvealgos.h"

class ThicknessTest : public CxxTest::TestSuite {
public:
  void testCriticality() {

    Vector3 a0p(-0.841446, -0.25, 0), a1p(-0.75, -0.5, 0);
    Vector3 at0p(0.225598, -0.97422, 0), at1p(0.447214, -0.894427, 0);

    Vector3 b0p(0.841446, -0.25, 0), b1p(0.75, -0.5, 0);
    Vector3 bt0p(-0.225598, -0.97422, 0), bt1p(-0.447214, -0.894427, 0);

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

    Vector3 ttt1, ttt2;
    ttt1 = (xxx-a0p); ttt1.normalize();
    ttt2 = (a1p-xxx); ttt2.normalize();

    Vector3 btt1, btt2;
    btt1 = (yyy-b0p); btt1.normalize();
    btt2 = (b1p-yyy); btt2.normalize();

    TS_ASSERT_EQUALS(double_critical_test(a0p,a1p,ttt1,ttt2,b0p,b1p,btt1,btt2),0);
    TS_ASSERT_EQUALS(double_critical_test_v2(a0p,xxx,a1p,b0p,yyy,b1p),0);
    dbl_crit_filter(C,CC);
    TS_ASSERT_EQUALS(CC.size(),0);
  }

};


#endif // __BASICTEST_H
