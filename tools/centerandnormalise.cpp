/*!
  \file centerandnormalise.cpp
  \ingroup ToolsGroup
  \brief Center and normalize a closed PKF curve.
*/

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include "../include/CurveBundle.h"

int main(int argc, char **argv) {

  if (argc!=3) {
    cerr << "Usage : "<<argv[0]<<" <pkf in> <pkf out>\n";
    exit(0);
  }
	
  CurveBundle<Vector3> curves(argv[1]);
  curves.center();
  curves.link();
  curves.make_default();
  curves.normalize();

  curves.writePKF(argv[2]);
  
  return 0;

}

#endif // DOXYGEN_SHOULD_SKIP_THIS
