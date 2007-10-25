/*!
  \defgroup ObjectGroup Object Directory
*/

/*!
  \file ellipse.cpp
  \ingroup ObjectGroup
  \brief Constructs an ellipse.

  Long explanation ... Bla bla bla
*/

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include "aux.h"

int main(int argc,char** argv) {
  if (argc!=4) {
    cerr << "Usage "<<argv[0]<<" <rad> <length> <nodes>\n";
    exit(1);
  }
  float rad = atof(argv[1]), len = atof(argv[2]);
  int nodes = atoi(argv[3]);
  Curve<Vector3>* stadium = gen_stadium(rad,len,nodes);
  if (!stadium->writePKF("stadium.pkf"))
    cerr << "Problem writing file."<<endl;

  return 0;
}

#endif // DOXYGEN_SHOULD_SKIP_THIS
