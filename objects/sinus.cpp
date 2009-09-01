/*!
  \file sinus.cpp
  \ingroup ObjectGroup
  \brief Constructs a Sinusoidal PKF curve.
*/

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include "../include/Curve.h"
#include <stdlib.h>

int main(int argc,char** argv) {

  if (argc!=5) {
    cerr << "Usage "<<argv[0]<<" <amplitude> <omega> <time> <segments>\n";
    exit(1);
  }

  float amp  = atof(argv[1]);
  float pulse = atof(argv[2]);
  float time = atof(argv[3]);
  int segs = atoi(argv[4]);

  Curve<Vector3> sinus;
  Vector3 p,t;
  
  // construct header
  sinus.header("Sinus","M. Carlen","","");
  
  float x=0;
  float delta = time/float(segs);
  while (x<time) {

    p = Vector3(x,amp*sin(pulse*x),0);
    t = Vector3(1.0,amp*pulse*cos(pulse*x),0);

    t.normalize();
    sinus.append(p,t);

    x+=delta;
  }
  if (!sinus.writePKF("sinus.pkf"))
    cerr << "Problem writing file."<<endl;
  
  return 0;
}

#endif // DOXYGEN_SHOULD_SKIP_THIS
