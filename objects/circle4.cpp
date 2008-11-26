/*!
  \defgroup ObjectGroup Object Directory
*/

/*!
  \file circle.cpp
  \ingroup ObjectGroup
  \brief Constructs a circle.

*/

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include <stdlib.h>
#include "Curve.h"
#include "Vector4.h"

int main(int argc,char** argv) {
  if (argc!=3) {
	  cerr << "Usage "<<argv[0]<<" <rad> <nodes>\n";
	  exit(1);
  }
  float rad  = atof(argv[1]), fac;
  int nodes  = atoi(argv[2]);
  Vector4 p,t;
  
  Curve<Vector4> circle;
  circle.header("R4 Circle","libbiarc","","");
  fac = 2.*M_PI/(float)nodes;
  for (int i=0;i<nodes;++i) {
    p = Vector4(0,0,rad*sin(fac*(float)i),rad*cos(fac*(float)i));
    t = Vector4(0,0,cos(fac*(float)i),-sin(fac*(float)i));
    t.normalize();
    circle.append(p,t);
  }
  if (!circle.writePKF("circle4.pkf"))
    cerr << "Problem writing file."<<endl;

  return 0;
}

#endif // DOXYGEN_SHOULD_SKIP_THIS
