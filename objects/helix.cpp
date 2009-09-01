/*!
  \file helix.cpp
  \ingroup ObjectGroup
  \brief Constructs a helical curve.
*/

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include "../include/Curve.h"
#include <stdlib.h>

int main(int argc,char** argv) {

  if (argc!=6) {
   cerr << "Usage "<<argv[0]<<" <radius> <omega>"
        << " <trans speed> <time> <segments>\n";
   exit(1);
  }
  
  float time  = atof(argv[4]);
  float R     = atof(argv[1]);
  float v     = atof(argv[3]);
  float pulse = atof(argv[2]);
  int segs    = atoi(argv[5]);

  Curve<Vector3> helix;
  Vector3 p,t;

  // construct header
  helix.header("Helix","M. Carlen","","");

  float x=0;
  float delta = time/float(segs);
  while (x<time) {

    p = Vector3(R*sin(x*pulse),R*cos(pulse*x),v*x);
    t = Vector3(R*pulse*cos(x*pulse),
		-R*pulse*sin(pulse*x),
		v);

    t.normalize();
    helix.append(p,t);

    x+=delta;
  }
  if (!helix.writePKF("helix.pkf"))
    cerr << "problem writing file"<<endl;
   
  return 0;
}

#endif // DOXYGEN_SHOULD_SKIP_THIS
