/*!
  \file xyz2pkf.cpp
  \ingroup ToolsGroup
  \brief Convert XYZ coordinates to PKF.

*/

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include "../include/Curve.h"

int main(int argc, char **argv) {

  if (argc!=3) {
    cout << "Usage : " << argv[0] << " <xyz> <pkf>\n";
    return 0;
  }

  Curve<Vector3> c;
  c.header("XYZ Curve","xyz2pkf","","");

  double v0, v1, v2;
  ifstream in(argv[1]);
  while (in >> v0 >> v1 >> v2)
    c.append(Vector3(v0,v1,v2),Vector3());

  c.computeTangents();

  c.writePKF(argv[2]);
}

#endif // DOXYGEN_SHOULD_SKIP_THIS
