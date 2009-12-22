/*!
  \file rotate_curve.cpp
  \ingroup ToolsGroup
  \brief Rotate curve about axis v by angle a.

*/

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include "../include/Curve.h"

int main(int argc, char **argv) {

  if (argc!=7) {
    cerr << "Usage : "<<argv[0]<<" <angle> <x y z> <pkf in> <pkf out>\n";
    exit(0);
  }

  cout << "Read in curve from " << argv[5];
  Curve<Vector3> curve(argv[5]);

  float angle = atof(argv[1])/180.*M_PI;
  Vector3 v(atof(argv[2]),atof(argv[3]),atof(argv[4]));
  curve.rotAroundAxis(angle, v);
  cout << "\t[OK]\n";

  cout << "Write curve to " << argv[6]; 
  curve.writePKF(argv[6]);
  cout << "\t[OK]\n";

  return 0;

}

#endif // DOXYGEN_SHOULD_SKIP_THIS
