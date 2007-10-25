/*!
  \file crouzy.cpp
  \ingroup ObjectGroup
  \brief Constructs a crouzy curve.

  Long explanation ... Bla bla bla
*/

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include "../include/Curve.h"
#include <stdlib.h>

int main(int argc,char** argv) {

  if (argc!=8) {
   cerr << "Usage "<<argv[0]<<" <radius> <omega>"
        << " <trans speed> <decay> <oscill period> <time> <segments>\n";
   exit(1);
  }
  
  float decay = atof(argv[4]);
  float period= atof(argv[5]);
  float time  = atof(argv[6]);
  float R     = atof(argv[1]);
  float v     = atof(argv[3]);
  float pulse = atof(argv[2]);
  int segs    = atoi(argv[7]);

  Curve<Vector3> crouzy;
  Vector3 p,t;

  // construct header
  crouzy.header("Crouzy Curve","Carlen&Crouzy","","");

  float x=0;
  float delta = time/float(segs);
  while (x<time) {

    p = Vector3(exp(-decay*x)*R*sin(x*pulse),
		exp(-decay*x)*R*cos(pulse*x),
		period*cos(v*x));
    t = Vector3(exp(-decay*x)*R*pulse*cos(x*pulse)+
		-decay*exp(-decay*x)*R*sin(x*pulse),
		-exp(-decay*x)*R*pulse*sin(pulse*x)+
		-decay*exp(-decay*x)*R*cos(pulse*x),
		-period*sin(v*x)*v);

    t.normalize();
    crouzy.append(p,t);

    x+=delta;
  }
  if (!crouzy.writePKF("crouzy.pkf"))
    cerr << "problem writing file"<<endl;
   
  return 0;
}

#endif // DOXYGEN_SHOULD_SKIP_THIS
