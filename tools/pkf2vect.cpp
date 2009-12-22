/*!
  \file arcs2polygonal.cpp
  \ingroup ToolsGroup
  \brief Convert an arc curve to a polygonal curve.

  The curve is taken to be closed.
*/

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include "../include/CurveBundle.h"

int main(int argc, char **argv) {

  if (argc!=3) {
    cerr << "Usage : "<<argv[0]<<" <pkf in> <vect out>\n";
    exit(0);
  }
  
  cout << "Read PKF from " << argv[1];
  CurveBundle<Vector3> cb(argv[1]);
  cout << "\t[OK]\n";

  // Interpolation is done inside of arcsToPoly()
  cout << "Close curves and convert to arcs (this doubles the number of vertices) " << flush;
  cb.link();
  cb.arcsToPolygonal();
  cout << "\t[OK]\n" <<flush;

  cout << "Write VECT file to " << argv[2];
  cb.writeVECT(argv[2]);
  cout << "\t[OK]\n";

  return 0;

}

#endif // DOXYGEN_SHOULD_SKIP_THIS
