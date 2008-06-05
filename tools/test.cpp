/*!
  \file test.cpp
  \ingroup ToolsGroup
  \brief Program for testing purpose only.

*/

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include "../include/Curve.h"
#include "../include/algo_helpers.h"

#define b3 vector<Biarc<Vector3> >::iterator

int th_cond(float d,Vector3& p0, Vector3 &p1, float tol) {
  if ( fabsf((p0-p1).norm()-d)<d*tol ) return 1;
  return 0;
}

int main(int argc, char **argv) {

  Curve<Vector3> c(argv[1]);
  c.link();
  c.make_default();

  float tol = atof(argv[2]);

  float thick = c.thickness();

  Vector3 b0,b1,b2,p,v;
  float r;
  cout << "#Inventor V2.1 ascii\nSeparator {\nCoordinate3 {\npoint [";
  int ContactsN = 0;
  for (b3 b=c.begin();b!=c.end();++b) {
    p = b->getPoint();
    for (b3 it=c.begin();it!=c.end();++it) {
      if (it==b) continue;
      it->getBezierArc0(b0,b1,b2);
      r = it->radius0();
      if (rhopt(p,b0,b1,b2,r,v) && th_cond(thick,p,v,tol)) {
        ++ContactsN;
        cout << p << ", " << v << ", " << endl;
      }
      it->getBezierArc1(b0,b1,b2);
      r = it->radius1();
      if (rhopt(p,b0,b1,b2,r,v) && th_cond(thick,p,v,tol)) {
        ++ContactsN;
        cout << p << ", " << v << ", " << endl;
      }
    }
  }
  cout << "] }\n";
  cout << "LineSet {\nnumVertices [ ";
  cerr << ContactsN << " contacts\n";
  for (int i=0;i<ContactsN;++i) {
    cout << "2, ";
  }
  cout << "] } }";
  return 0;
}

#endif // DOXYGEN_SHOULD_SKIP_THIS
