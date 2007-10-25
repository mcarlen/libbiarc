/*!
  \defgroup ObjectGroup Object Directory
*/

/*!
  \file circle.cpp
  \ingroup ObjectGroup
  \brief Constructs a circle.

*/

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include "aux.h"

int main(int argc,char** argv) {
  if (argc!=3) {
	  cerr << "Usage "<<argv[0]<<" <rad> <nodes>\n";
	  exit(1);
  }
  float rad  = atof(argv[1]);
  int nodes  = atoi(argv[2]);
  
  Curve<Vector3>* circle = gen_circle(rad,nodes);
  if (!circle->writePKF("circle.pkf"))
    cerr << "Problem writing file."<<endl;

  return 0;
}

#endif // DOXYGEN_SHOULD_SKIP_THIS
