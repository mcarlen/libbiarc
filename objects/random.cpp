/*!
  \file random.cpp
  \ingroup ObjectGroup
  \brief Generate a "random" curve
*/

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include "../include/Curve.h"
#include <stdlib.h>

float frand(float x) { return ((x)*(float)rand()/(float)RAND_MAX); }

void pot(Vector3 &v1,Vector3 &v2) {
  Vector3 a = v1 + v2*(1.-frand(0.1))*frand(0.01);
  v2 -= v1/(a.norm()*a.norm())*frand(0.01);
  v1 = a;
}

int main(int argc,char** argv) {
  srand(time(NULL));
  if (argc!=3) {
	  cerr << "Usage "<<argv[0]<<" <scale> <nodes>\n";
	  exit(1);
  }

  int nodes = atoi(argv[2]);

  Curve<Vector3> c;

  // construct header
  c.header("RandomCurve","M. Carlen","","");

  Vector3 p = Vector3(1,0,0),op,t = Vector3(frand(1),frand(1),frand(1));
  t.normalize();
  for (int i=0;i<nodes;i++) {
      c.append(p,t);
      pot(p,t);
  }
  c.computeTangents();
  if (!c.writePKF("random.pkf"))
    cerr << "Problem writing file."<<endl;

  return 0;
}

#endif // DOXYGEN_SHOULD_SKIP_THIS
