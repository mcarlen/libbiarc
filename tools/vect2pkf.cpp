/*!
  \file polygonal2arcs.cpp
  \ingroup ToolsGroup
  \brief Convert a polygonal space curve to an arc curve.

  The curve is taken to be closed.
*/

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include "../include/CurveBundle.h"

int main(int argc, char **argv) {

  if (argc!=3) {
    cerr << "Usage : "<<argv[0]<<" <polygonal in> <pkf out>\n";
    exit(0);
  }

  CurveBundle<Vector3> cb;
  cout << "Read polygonal curve from " << argv[1];
  cb.readVECT(argv[1]);
  cout << "\t[OK]\n";

  cb.polygonalToArcs();

  cout << "Write arc curve to " << argv[2];
  cb.writePKF(argv[2]);
  cout << "\t\t[OK]\n";
  
  return 0;

}

#endif // DOXYGEN_SHOULD_SKIP_THIS
