/*!
 \file inversion_in_sphere.cpp
 \ingroup ToolsGroup
 \brief Inversion in sphere at center c and radius r of a curve.
*/

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include "../include/Curve.h"
#include "../include/Matrix4.h"
#include <stdlib.h>

#ifdef Dim4
#define VecType Vector4
#else
#define VecType Vector3
#endif

void inversion_in_sphere(Curve<VecType>* curve, VecType center = VecType(),
                         float radius = 2.0) {
  VecType p,pnew,t,tnew,v; 
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
  if (argc!=7) {
    cout << "Usage : " << argv[0] << " cx cy cz radius curve.pkf out.pkf\n";
    exit(0);
  }

  VecType center(atof(argv[1]),atof(argv[2]),atof(argv[3]));
  float rad = atof(argv[4]);
  Curve<VecType> curve(argv[5]);
  inversion_in_sphere(&curve,center,rad);
  curve.header(curve.getName(),"inversion_in_sphere","","");
  curve.writePKF(argv[6]);

  return 0;
}

#endif
