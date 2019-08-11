/*!
  \file flatten.cpp
  \ingroup ToolsGroup
  \brief Project curve onto 2D plane.
*/

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include <Curve.h>

int main(int argc, char **argv) {

  if (argc!=3) {
    cerr << "Usage : "<<argv[0]<<" <pkf in> <pkf out>\n";
    exit(0);
  }

  clog << "Read in curve from " << argv[1];
  Curve<Vector3> curve(argv[1]);
  /* FIXME: this test can never fail
  if (&curve!=NULL)
    clog << "\t[OK]\n";
  else {
    clog << "\t[FAILED]\n";
    return 1;
  }
  */

  for (int i=0;i<curve.nodes();i++) {
    Vector3 p=curve[i].getPoint(),t=curve[i].getTangent();
    p[2]=0.0;t[2]=0.0; t.normalize();
    curve[i].setPoint(p);curve[i].setTangent(t);
  }

  clog << "Write curve to " << argv[2];
  if (curve.writePKF(argv[2]))
    clog << "\t\t\t[OK]\n";
  else {
    clog << "\t\t\t[FAILED]\n";
    return 2;
  }
  return 0;

}

#endif // DOXYGEN_SHOULD_SKIP_THIS
