#ifndef __BASICTEST_H
#define __BASICTEST_H

#include <cxxtest/TestSuite.h>

#include "Vector4.h"
#include "Curve.h"
#include "algo_helpers.h"

template<class Vector>
float alen(biarc_it, biarc_it, int, int);

/*
Vector3 bezierMidPoint(Vector3& b0, Vector3& b1, Vector3& t0) {
  Vector3 d = b1 - b0; Vector3 e(d); e.normalize();
  Vector3 t1 = t0.reflect(e);
  float l = t0 + ;
  return b0 + ;
}
*/

class ThicknessTest : public CxxTest::TestSuite {
public:

  void testCircle() {
    Curve<Vector3> c; float a, thick; Vector3 p, t;
    for (int i=0;i<4;++i) {
      a = (float)i/(float)4*2.*M_PI;
      p = Vector3(cos(a),sin(a),0);
      t = Vector3(-sin(a),cos(a),0); t.normalize();
      c.append(p,t);
    }
    c.link();
    c.make_default();

    float min_diam = check_local_curvature(&c);
    TS_ASSERT_DELTA(2.0,min_diam,1e-14);

    // These guys are not double critical!
    // The condition here is necessary but not sufficient!!!
    TS_ASSERT(double_critical_test(Vector3(1, 0, 0),
                                   Vector3(0.707107, 0.707107, 0),
                                   Vector3(0, 1, 0),
                                   Vector3(-0.707107, 0.707107, 0),
                                   Vector3(0, 1, 0),
                                   Vector3(-0.707107, 0.707107, 0),
                                   Vector3(-1, 0, 0),
                                   Vector3(-0.707107, -0.707107, 0),1e22));


    // This is a first subdiv of the above and here the pair gets filtered
    // out by the double critical test
    Vector3 v1 = Vector3(0.707107, 0.707107, 0);
    Vector3 v2 = Vector3(0.92388, 0.382683, 0);
    Vector3 v3 = Vector3(0.707106, -0.707107, 0 );
    Vector3 v4 = Vector3(0, 1, 0);
    Vector3 v5 = Vector3(-0.382683, 0.92388, 0);
    Vector3 v6 = Vector3(-1, 0, 0);
    Vector3 ax = v2-v1; ax.normalize();
    Vector3 ax2 = v5-v4; ax2.normalize();
    Vector3 xxx = v3.reflect(ax);
    Vector3 yyy = v6.reflect(ax2);
    TS_ASSERT(!double_critical_test(v1,v2,v3,xxx,
                                    v4,v5,v6,yyy,1e22));
    
    thick = c.thickness();
    TS_ASSERT_DELTA(2.0,thick,1e-10);
  }

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
    dbl_crit_filter(C,CC,0);
    TS_ASSERT_EQUALS(CC.size(),0);
  }

  void testCurveThickness() {
    Curve<Vector3> c("../knots/k3.1.pkf");
    c.link();
    c.make_default();
    TS_ASSERT_DELTA(c.thickness(),0.061033,1e-8);
    // Test if hint does not mess up anything
    // and should be faster
    TS_ASSERT_DELTA(c.thickness(),0.061033,1e-8);
  }

   void testCurveThicknessTooBigBug() {
    
    Curve<Vector3> c("../knots/wrong_thickness_kp31m31_norm.pkf");
    c.link();
    c.make_default();
		float l = c.length();
		Vector3 a,b;
		float rope1 = l/compute_thickness(&c,&a,&b);
		float rope1_fast = l/c.thickness_fast();
		TS_ASSERT(rope1 > rope1_fast);
	
    Curve<Vector3> c2("k3.1_30.pkf");
		c2.link();
		c2.make_default();
		l = c2.length();
		rope1 = l/compute_thickness(&c2);
		rope1_fast = l/c2.thickness_fast();
		TS_ASSERT(rope1 > rope1_fast);

    Curve<Vector4> c3("../knots/r4/wrong_thickness_k51.pkf");
		c3.link();
		c3.make_default();
		TS_ASSERT(compute_thickness(&c3) > c3.thickness_fast());
  }



};


#endif // __BASICTEST_H
