/*!
  \file torusknot.cpp
  \ingroup ObjectGroup
  \brief Constructs a (p,q) torus knot.

*/

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include "../include/Curve.h"
#include <stdlib.h>

int main(int argc,char** argv) {

  if (argc!=4) {
	  cerr << "Usage "<<argv[0]<<" <p> <q> <points>\n";
	  exit(1);
  }
  
  float p   = atof(argv[1]);
  float q   = atof(argv[2]);
  int n     = atoi(argv[3]);

  Curve<Vector3> curve;

  Vector3 pt,tg;

  float angle = 2.0*M_PI/(float)n*(float)p;
  float arg = 0.0;
  for (int i=0;i<n;i++) {
    arg += angle; // (float)i*angle*(float)q;
    pt = Vector3((2.+cos(q*arg/p))*cos(arg),
		(2.+cos(q*arg/p))*sin(arg),
		sin(q*arg/p));
    tg = Vector3(1,0,0);

    curve.append(pt,tg);
  }
  curve.computeTangents();

  curve.header("Torusknot","M. Carlen","","");
  if (!curve.writePKF("torusknot.pkf"))
    cerr << "problem writing file"<<endl;
   
  return 0;
}

#endif // DOXYGEN_SHOULD_SKIP_THIS
