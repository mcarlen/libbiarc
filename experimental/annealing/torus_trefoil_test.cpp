#include "CurveBundle.h"
#include "algo_helpers.h"

// Torus with both radii = 1
// This torus has an offset of z_dist from the z axis
// and is tilted with angle tilt around the x axis
// u=0,v=0 is vector 1,0,0
inline Vector3 on_torus(float u, float v, float tilt, float xoffset) {
  Vector3 vec((1+cos(v))*cos(u), (1+cos(v))*sin(u), sin(v));
  Matrix3 mat;
  mat.rotAround(Vector3(1,0,0), tilt);
  return (mat*vec+Vector3(xoffset,0,0));
}

inline Vector3 torus(float u, float tilt, float xoffset) {
  Vector3 vec(cos(u), sin(u), 0);
  Matrix3 mat;
  mat.rotAround(Vector3(1,0,0), tilt);
  return (mat*vec+Vector3(xoffset,0,0));
}

int main(int argc, char** argv) {
  Curve<Vector3> c;
  CurveBundle<Vector3> cb;
  // Torus 1
  
  float xoffset = atof(argv[2]);
  float tilt = atof(argv[1]);
  cout << "tilt=" << tilt << " xoffset=" << xoffset << endl;

  for (int i=0;i<100;++i)
    c.append(torus((float)i/99.*2.*M_PI, tilt, xoffset),Vector3());

  cb.newCurve(c);
  cb.newCurve(c.rotAroundAxis(2.*M_PI/3.,Vector3(0,0,1)));
  cb.newCurve(c.rotAroundAxis(2.*M_PI/3.,Vector3(0,0,1)));

  cb.computeTangents();
  cb.writePKF("out.pkf");
  return 0;
}
