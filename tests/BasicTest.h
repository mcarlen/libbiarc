#ifndef __BASICTEST_H
#define __BASICTEST_H

#include <cxxtest/TestSuite.h>

#include "Biarc.h"
#include "Curve.h"

//
// A simple test suite: Just inherit CxxTest::TestSuite and write tests!
//

#define TS_ASSERT_VECTOR3(v,v2,tol) TS_ASSERT_DELTA((v)[0],(v2)[0],(tol)); \
                                    TS_ASSERT_DELTA((v)[1],(v2)[1],(tol)); \
                                    TS_ASSERT_DELTA((v)[2],(v2)[2],(tol));


class BasicTest : public CxxTest::TestSuite {
public:
  void testBiarc() {
    Biarc<Vector3> b;
    TS_ASSERT(b==b);
    TS_ASSERT(!b.isBiarc());
    TS_ASSERT_EQUALS(b.id(),-1);
    Vector3 p = b.getPoint(), t = b.getTangent();
    TS_ASSERT_EQUALS(p,Vector3(0,0,0));
    TS_ASSERT_EQUALS(t,Vector3(0,1,0));

    b.setPoint(Vector3(1,1,1));
    b.setTangent(Vector3(5,0,0));
    TS_ASSERT(b==Biarc<Vector3>(Vector3(1,1,1),Vector3(1,0,0)));

    b.setTangentUnnormalized(Vector3(5,0,0));
    TS_ASSERT_EQUALS(b.getTangent(),Vector3(5,0,0));
 
    b.reverse();
    TS_ASSERT_EQUALS(b.getPoint(),Vector3(-1,-1,-1));

  }

  void testPointOn() {
    Curve<Vector3> c;
    c.append(Biarc<Vector3>(Vector3(0,1,0),Vector3(1,0,0)));
    c.append(Biarc<Vector3>(Vector3(1,0,0),Vector3(0,-1,0)));
    c.makeMidpointRule();
    TS_ASSERT_DELTA((c[0].pointOnBiarc(M_PI/4.)-c[0].getPoint()).norm2(),
                    (c[1].getPoint()-c[0].pointOnBiarc(M_PI/4.)).norm2(),
                    1e-6);

    Vector3 t = c[0].tangentOnBiarc(M_PI/4.);
    TS_ASSERT_VECTOR3(t,Vector3(1,-1,0)/Vector3(1,-1,0).norm(),1e-6);

  }

  void testCurve() {
    Curve<Vector3> c;

    for (int i=0;i<3;i++)
      c.append(Vector3(i+1,i+1,i+1),Vector3(1,0,0));
    c.link();

    TS_ASSERT_EQUALS(c[0].getCurve(),&c);

    TS_ASSERT_EQUALS(c.getNext(0),c[1]);
    TS_ASSERT_EQUALS(c.getNext(2),c[0]);
    TS_ASSERT_EQUALS(c.getPrevious(0),c[2]);

    TS_ASSERT_EQUALS(c[0].getNext(),c[1]);
    TS_ASSERT_EQUALS(c[2].getNext(),c[0]);
    TS_ASSERT_EQUALS(c[0].getPrevious(),c[2]);

    c.flush_all();

    // Quarter Circle check
    c.append(Biarc<Vector3>(Vector3(0,1,0),Vector3(1,0,0)));
    c.append(Biarc<Vector3>(Vector3(1,0,0),Vector3(0,-1,0)));

    TS_ASSERT_EQUALS(c[0].id(),0);
    TS_ASSERT_EQUALS(c[1].id(),1);
    TS_ASSERT_EQUALS(c.nodes(),2);

    c[0].make(.5);
    TS_ASSERT(c[0].isBiarc());
    TS_ASSERT(c[0].isProper());
    TS_ASSERT_DELTA(c[0].radius0(),1.0,1e-6);
    TS_ASSERT_DELTA(c[0].radius1(),1.0,1e-6);
    TS_ASSERT_DELTA(c[0].biarclength(),M_PI/2.,1e-7);
    TS_ASSERT_EQUALS(c[0].biarclength(),c.length());
   
    c.flush_all();
    c.append(Biarc<Vector3>(Vector3(0,0,0),Vector3(1,1,0)));
    c.append(Biarc<Vector3>(Vector3(1,0,0),Vector3(1,-1,0)));
    c[0].make(.5);
    Vector3 tmp = c[0].getMidPoint();
    TS_ASSERT_VECTOR3(tmp,Vector3(.6,.2,0),1e-6);

    c[1].set(Vector3(1,0,0),Vector3(1,1,0));
    c[0].make(.5);
    TS_ASSERT(c[0].isBiarc());
    TS_ASSERT(c[0].isProper());
    TS_ASSERT_DELTA(c[0].radius0(),c[0].radius1(),1e-6);
    TS_ASSERT_VECTOR3(c[0].getMidPoint(),Vector3(.5,0,0),1e-6);
    tmp = Vector3(1,-1,0); tmp.normalize();
    TS_ASSERT_VECTOR3(c[0].getMidTangent(),tmp,1e-6);
    c.flush_all();

    // Slalom biarc check 2
    c.append(Biarc<Vector3>(Vector3(0,1,0),Vector3(1,0,0)));
    c.append(Biarc<Vector3>(Vector3(1,0,0),Vector3(1,0,0)));

    c[0].make(.5);
    TS_ASSERT(c[0].isBiarc());
    TS_ASSERT(c[0].isProper());

    TS_ASSERT_DELTA(c[0].radius0(),c[0].radius1(),1e-6);
    TS_ASSERT_DELTA(c[0].biarclength(),M_PI/2.,1e-6);
   
    TS_ASSERT(c[0].getMidPoint()==Vector3(.5,.5,0));
    TS_ASSERT_VECTOR3(c[0].getMidTangent(),Vector3(0,-1,0),1e-6);
  }
  
  void testMidpointRule() {
    Curve<Vector3> c;
    c.append(Biarc<Vector3>(Vector3(1,0,0),Vector3(1,.2,0)));
    c.append(Biarc<Vector3>(Vector3(2,-.2,-.5),Vector3(1,-.3,-.4)));

    c.makeMidpointRule();
    TS_ASSERT(c[0].isBiarc());
    TS_ASSERT(c[0].isProper());
    TS_ASSERT_DELTA((c[0].getMidPoint()-c[0].getPoint()).norm2(),
                    (c[1].getPoint()-c[0].getMidPoint()).norm2(),
                     1e-6);
  }

  void testStraightLine() {
    // TODO
  }

};


#endif // __BASICTEST_H
