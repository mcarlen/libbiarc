/*!
  \file solenoid.cpp
  \ingroup ObjectGroup
  \brief Constructs a solenoid like curve.

  Long explanation ... Bla bla bla
  Formulas for the solenoid construction come from Antonio Trovato (It).
*/

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include "../include/Curve.h"
#include <stdlib.h>

int main(int argc,char** argv) {

  if (argc!=5) {
	  cerr << "Usage "<<argv[0]<<" <rad> <big rad> <turns> "
	       << "<points>\n";
	  exit(1);
  }
  
  float rad     = atof(argv[1]);
  float bigrad  = atof(argv[2]);
  int nturn     = atoi(argv[3]);
  int n         = atoi(argv[4]);

  Curve<Vector3> solenoid;

  Vector3 p,t;

  float angle = 2.0*M_PI/n;
  float arg;
  for (int i=0;i<n;i++) {
    arg = (float)i*angle;
    p = Vector3(bigrad*sin(arg)-rad*sin(arg)*sin(arg*nturn),
		bigrad*cos(arg)-rad*cos(arg)*sin(arg*nturn),
		rad*cos(nturn*arg));
    t = Vector3(bigrad*cos(arg)-nturn*rad*sin(arg)*cos(arg*nturn)-
		rad*cos(arg)*sin(arg*nturn),
		-bigrad*sin(arg)-nturn*rad*cos(arg)*cos(arg*nturn)+
		+rad*sin(arg)*sin(arg*nturn),
		-nturn*rad*sin(nturn*arg));
    t.normalize();

    solenoid.append(p,t);
  }

  solenoid.header("Solenoid","M. Carlen","","");
  if (!solenoid.writePKF("solenoid.pkf"))
    cerr << "problem writing file"<<endl;
   
  return 0;
}

#endif // DOXYGEN_SHOULD_SKIP_THIS
