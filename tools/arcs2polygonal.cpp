/*!
  \file arcs2polygonal.cpp
  \ingroup ToolsGroup
  \brief Convert an arc curve to a polygonal curve.

  The curve is taken to be closed
  Long explanation ... Bla bla bla
*/

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include "../include/CurveBundle.h"

int main(int argc, char **argv) {

  if (argc!=3) {
    cerr << "Usage : "<<argv[0]<<" <pkf in> <pkf out>\n";
    exit(0);
  }
  
  cout << "Read polygonal curve from " << argv[1];
  CurveBundle<Vector3> cb(argv[1]);
  cout << "\t[OK]\n";

  // Interpolation is done inside of arcsToPoly()
  cout << "Close curves and convert to arcs" << flush;
  cb.link();
  cb.arcsToPolygonal();
  cout << "\t[OK]\n" <<flush;

  cout << "Write polygonal curve to " << argv[2];
  cb.writeData(argv[2]," ",0);
  cout << "\t[OK]\n";

  return 0;

}

#endif // DOXYGEN_SHOULD_SKIP_THIS
