/*!
 \file r3tos3.cpp
 \ingroup ToolsGroup
 \brief Project a curve in R^3 to S^3.
*/

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include "../include/Curve.h"
#include "../include/Vector4.h"
#include <stdlib.h>

// inversion in a sphere centered at (0,0,0,1) and radius 2.0
void inversion_in_sphere(Curve<Vector4>* curve, Vector4 center = Vector4(0,0,0,1),
                         float radius = 2.0) {
  Vector4 p,pnew,t,tnew,v; 
  float factor, vnorm2;
  for (int i=0;i<curve->nodes();++i) {

    p = (*curve)[i].getPoint();
    t = (*curve)[i].getTangent();

    v = (p-center);
    vnorm2 = v.norm2();
 
    factor = radius*radius/vnorm2;
    pnew = center + v*factor;
    tnew = t*factor - v*(2*factor/vnorm2*t.dot(v));
    tnew.normalize();
    (*curve)[i].setPoint(pnew);
    (*curve)[i].setTangent(tnew);
  }
}

int main(int argc, char** argv) {
  if (argc!=3) {
    cout << "Usage : " << argv[0] << " r3.pkf out_s3.pkf\n";
    exit(0);
  }

  Curve<Vector3> r3(argv[1]);
  Curve<Vector4> s3;
  s3.header(r3.getName(),"r3tos3","","");

  // Embed R^3 knot as the -1 plane in R^4
  for (int i=0;i<r3.nodes();++i) {
    Vector4 p(r3[i].getPoint(),-1), t(r3[i].getTangent(),0);
    s3.append(p,t);
  }

  inversion_in_sphere(&s3);
  s3.writePKF(argv[2]);

  return 0;
}

#endif
