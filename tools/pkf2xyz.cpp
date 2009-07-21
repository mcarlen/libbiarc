/*!
  \file pkf2xyz.cpp
  \ingroup ToolsGroup
  \brief Convert PKF file to XYZ coordinates.

*/

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include "../include/Curve.h"

int main(int argc, char **argv) {

  if (argc!=3) {
    cout << "Usage : " << argv[0] << " <pkf> <xyz>\n";
    return 0;
  }

  Curve<Vector3> k(argv[1]);

  ofstream out(argv[2]);
  out.precision(12);
  for (int i=0;i<k.nodes();++i)
    out << k[i].getPoint() << endl;
  out.close();

}

#endif // DOXYGEN_SHOULD_SKIP_THIS
