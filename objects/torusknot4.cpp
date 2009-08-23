/*!
  \file torusknot4.cpp
  \ingroup ObjectGroup
  \brief Constructs a (p,q) torus knot on a torus in R^4.

*/

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include "../include/Curve.h"
#include "../include/Vector4.h"
#include <stdlib.h>

int main(int argc,char** argv) {

  if (argc!=6) {
	  cerr << "Usage "<<argv[0]<<" <R> <r> <p> <q> <points>\n";
	  exit(1);
  }
  
  float R   = atof(argv[1]);
  float r   = atof(argv[2]);
  float p   = atof(argv[3]);
  float q   = atof(argv[4]);
  int n     = atoi(argv[5]);

  Curve<Vector4> curve;

  Vector4 pt,tg;

  float d_theta = 2.0*M_PI/(float)n;
  float d_phi   = 2.0*M_PI/(float)n;
  float theta = 0.0, phi = 0.0;
  for (int i=0;i<n;i++) {

    pt = Vector4(R*cos(theta*(float)p),R*sin(theta*(float)p),
                 r*cos(phi*(float)q),r*sin(phi*(float)q));
    tg = Vector4(1,0,0,0);

    curve.append(pt,tg);

    theta += d_theta;
    phi += d_phi;
  }
  curve.computeTangents();

  curve.header("Torusknot_R4","torusknot4","","");
  if (!curve.writePKF("torusknot4.pkf"))
    cerr << "problem writing file"<<endl;
   
  return 0;
}

#endif // DOXYGEN_SHOULD_SKIP_THIS
