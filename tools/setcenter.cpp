/*!
  \file setcenter.cpp
  \ingroup ToolsGroup
  \brief Reset the mass center of PKF curve.
*/

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include "../include/CurveBundle.h"

int main(int argc, char **argv) {

  if (argc!=6) {
    cerr << "Usage : "<<argv[0]<<" <Cx> <Cy> <Cz> <pkf in> <pkf out>\n";
    exit(0);
  }
	
  CurveBundle<Vector3> cb(argv[4]);
  cb+=Vector3(atof(argv[1]),atof(argv[2]),atof(argv[3]));

  cb.writePKF(argv[5]);
  
  return 0;

}

#endif // DOXYGEN_SHOULD_SKIP_THIS
