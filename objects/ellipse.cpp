/*!
  \defgroup ObjectGroup Object Directory
*/

/*!
  \file ellipse.cpp
  \ingroup ObjectGroup
  \brief Constructs an ellipse.
*/

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include "aux.h"

int main(int argc,char** argv) {
  if (argc!=4) {
    cerr << "Usage "<<argv[0]<<" <a> <b> <nodes>\n";
    exit(1);
  }
  float a = atof(argv[1]), b = atof(argv[2]);
  int nodes = atoi(argv[3]);
  Curve<Vector3>* ellipse = gen_ellipse(a,b,nodes);
  if (!ellipse->writePKF("ellipse.pkf"))
    cerr << "Problem writing file."<<endl;

  return 0;
}

#endif // DOXYGEN_SHOULD_SKIP_THIS
