/*!
  \defgroup ObjectGroup Object Directory
*/

/*!
  \file inf.cpp
  \ingroup ObjectGroup
  \brief Constructs an infinity shaped curve.
*/

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include "aux.h"

int main(int argc,char** argv) {
  if (argc!=5) {
    cerr << "Usage "<<argv[0]<<" <a> <b> <scale> <nodes>\n";
    exit(1);
  }
  float a = atof(argv[1]), b = atof(argv[2]);
  float scale = atof(argv[3]);
  int nodes = atoi(argv[4]);
  Curve<Vector3>* inf = gen_inf(a,b,scale,nodes);
  if (!inf->writePKF("inf.pkf"))
    cerr << "Problem writing file."<<endl;

  return 0;
}

#endif // DOXYGEN_SHOULD_SKIP_THIS
