/*!
  \file reverse_direction.cpp
  \ingroup ToolsGroup
  \brief Change the orientation of the curve.
  
  We change the orientation of the curve by flipping the
  tangents and reordering the points.
*/

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include "../include/CurveBundle.h"
#include <stdlib.h>

void usage(char* prog) {
  cerr << "Usage : " << prog
       <<" <Infile> <Outfile>\n";
  exit(1);
}

int main(int argc, char** argv) {
  // Check number of arguments
  if (argc!=3) {
    usage(argv[0]);
  }

  // in File
  string infile(argv[1]);
  string outfile = string(argv[2]);

  // Process infile to outfile
  cout << "Read in file\t\t\t"<<flush;
  CurveBundle<Vector3> original(infile.c_str());
  cout << "\t[OK]\n";
 
  original.changeDirection();
  
  cout << "Write output to file\t";
  original.writePKF(outfile.c_str());
  cout << "\t[OK]\n";

  return 0;
}

#endif // DOXYGEN_SHOULD_SKIP_THIS
