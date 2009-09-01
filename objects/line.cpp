/*!
  \file line.cpp
  \ingroup ObjectGroup
  \brief Produces a file with a PKF line.
*/

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include "../include/Curve.h"
#include <stdlib.h>

int main(int argc,char** argv) {

  if (argc!=3) {
	  cerr << "Usage "<<argv[0]<<" <height> <segments>\n";
	  exit(1);
  }

  float height = atof(argv[1]);
  int seg = atoi(argv[2]);
  
  Curve<Vector3> line;
  Vector3 p,t;

  // construct header
  line.header("Line","M. Carlen","","");

  float delta = height/(float)seg;
  float offset = height/2.0;
  
  for (int i=0;i<=seg;i++) {
      p = Vector3((float)i*delta-offset,0,0);
      t = Vector3(1,0,0);
      line.append(p,t);
  }
  if (!line.writePKF("line.pkf"))
    cerr << "Problem writing file."<<endl;
   
  return 0;
}

#endif // DOXYGEN_SHOULD_SKIP_THIS
