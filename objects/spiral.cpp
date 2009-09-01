/*!
  \file spiral.cpp
  \ingroup ObjectGroup
  \brief Constructs a Spiral PKF file.
*/

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include "../include/Curve.h"
#include <stdlib.h>

int main(int argc,char** argv) {

  if (argc!=6) {
    cerr << "Usage "<<argv[0]<<" <rad speed> <omega> <transl speed> "
         << "<time> <segments>\n";
    exit(1);
  }
  
  float time    = atof(argv[4]);
  float omega   = atof(argv[2]);
  float v       = atof(argv[3]);
  float r_speed = atof(argv[1]);
  int segs      = atoi(argv[5]);

  Curve<Vector3> spiral;

  Vector3 p,t;

  // construct header
  spiral.header("Spiral","M. Carlen","","");

  float i=0;
  float delta = time/float(segs);
  while (i<time) {

      float R = i*r_speed;

      p = Vector3(R*sin(i*omega), R*cos(i*omega),i*v);
      t = Vector3(r_speed*sin(i*omega)+R*omega*cos(i*omega),
		  r_speed*cos(i*omega)-R*omega*sin(i*omega),
		  v);
      t.normalize();
      spiral.append(p,t);

      i += delta;
  }
  if (!spiral.writePKF("spiral.pkf"))
    cerr << "problem writing file"<<endl;
   
  return 0;
}

#endif // DOXYGEN_SHOULD_SKIP_THIS
