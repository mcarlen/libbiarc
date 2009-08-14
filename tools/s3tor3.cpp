/*!
 \file r3tos3.cpp
 \ingroup ToolsGroup
 \brief Project a curve on S^3 back to R^3.
*/

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include "../include/Curve.h"
#include "../include/Matrix4.h"
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

Matrix4 rotm(int i, int k, float angle) {
  Matrix4 m; m.id();
  m[i][i] = cos(angle);
  m[k][k] = cos(angle);
  m[i][k] = sin(angle);
  m[k][i] = -sin(angle);
  return m;
}

int main(int argc, char** argv) {
  if (argc!=3) {
    cout << "Usage : " << argv[0] << " s3.pkf out_r3.pkf\n";
    exit(0);
  }

  Curve<Vector4> s3(argv[1]);

  // Align S^3 and the curve according to the northpole we want
  Vector4 northpole(0,0,0,1);
  float rho, angle;
  for (int i=0;i<3;++i) {
    if (northpole[i]) {
      rho = (northpole[i]>=0?1:-1)*sqrt(northpole[i]*northpole[i]+northpole[3]*northpole[3]);
      angle = acos(northpole[3]/rho);
      Matrix4 mat = rotm(3,i,angle);
      northpole = mat*northpole;
      for (int k=0;k<s3.nodes();++k) {
        Vector4 p = s3[k].getPoint();
        Vector4 t = s3[k].getTangent();
        p = mat*p; t = mat*t;
        s3[k].setPoint(p);
        s3[k].setPoint(t);
      }
    }
  }

  inversion_in_sphere(&s3);

  // Clip the last coordinate, since our knot is embedded in the (0,0,0,-1) hyperplane
  // in R^4
  Curve<Vector3> r3;
  r3.header(s3.getName(),"s3tor3","","");
  for (int i=0;i<s3.nodes();++i) {
    Vector4 p(s3[i].getPoint());
    Vector4 t(s3[i].getTangent());
    r3.append(Vector3(p[0],p[1],p[2]),Vector3(t[0],t[1],t[2]));
  }

  r3.writePKF(argv[2]);

  return 0;
}

#endif
