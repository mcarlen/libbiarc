#ifndef __SIMPLETEST_H
#define __SIMPLETEST_H

#include <cxxtest/TestSuite.h>
#include "circle.h"
#include "ellipse.h"

template<class Vector>
class Biarc;

//
// A simple test suite: Just inherit CxxTest::TestSuite and write tests!
//

class SimpleTest : public CxxTest::TestSuite {

public:

// Absolute values are expected to become worse for
// big radius, so we normalize.
void testCircle() {
  float rad = 1.0f;
  int nodes = 100;
  float ds;

  Curve<Vector3>* c = gen_circle(rad, nodes);
  c->link(); c->make_default();
  ds = c->length()/(float)nodes;

  TS_ASSERT_DELTA(2.0f*rad,c->thickness(),1e-2);
  TS_ASSERT_DELTA(2.0f*M_PI,c->length(),1e-4);

  float mdiff = p_diff_circle(rad,ds,c);
  TS_ASSERT_LESS_THAN(mdiff/rad,1e-9f);
  mdiff = t_diff_circle(rad,ds,c);
  TS_ASSERT_LESS_THAN(mdiff/rad,1e-8f);
  float pdiff,tdiff;
  mp_diff_circle(rad,c,pdiff,tdiff);
  TS_ASSERT_LESS_THAN(pdiff/rad,1e-9f);
  TS_ASSERT_LESS_THAN(tdiff/rad,1e-8f);
}

void testEllipse() {
  // FIXME
  // Our "analytic" way of computing points on an ellipse
  // is not arc-length param!!! that's why we take a=b
  float a = 1.0;
  float b = 1.0;
  int nodes = 100;
  float ds;
  float rad = (a+b)/2.;

  Curve<Vector3>* c = gen_ellipse(a,b,nodes);
  c->link(); c->make_default();
  ds = c->length()/(float)nodes;

  float rH = b*b/a, rN = a*a/b;
  TS_ASSERT_DELTA(2.*(rH<rN?rH:rN),c->thickness(),1e-2);
  TS_ASSERT_DELTA(2.*M_PI,c->length(),1e-4);  
  float mdiff = p_diff_ellipse(a,b,ds,c);
  TS_ASSERT_LESS_THAN(mdiff/rad,1e-9f);
  mdiff = t_diff_ellipse(a,b,ds,c);
  TS_ASSERT_LESS_THAN(mdiff/rad,1e-8f);
  float tdiff,pdiff;
  mp_diff_ellipse(a,b,c,pdiff,tdiff);
  TS_ASSERT_LESS_THAN(pdiff/rad,1e-9f);
  TS_ASSERT_LESS_THAN(tdiff/rad,1e-8f);

}

};


#endif // __SIMPLETEST_H
