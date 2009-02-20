/*!
  \file test.cpp
  \ingroup ToolsGroup
  \brief Program for testing purpose only.

*/

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include "../include/Curve.h"

int main(int argc, char **argv) {

  Curve<Vector3> c;
  c.append(Vector3(0,0,0),Vector3(1,1,0));
  c.append(Vector3(1,1,0),Vector3(1,0,0));
  c.append(Vector3(2,0,0),Vector3(1,-1,0));

  cout << c.normalVector(1) << endl;

  c.writePKF("test.pkf");
  
  return 0;
}

#endif // DOXYGEN_SHOULD_SKIP_THIS
