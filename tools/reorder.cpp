/*!
  \file reorder.cpp
  \ingroup ToolsGroup
  \brief Reorder the nodes of a curve, so that the first
         points of the 2 curves are the closest possible.

*/

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include "../include/Curve.h"
#include <stdlib.h>

void usage(char* prog) {
  cerr << "Usage : " << prog
       <<" <file1> <file2> <reorderedfile>\n";
  exit(1);
}

int main(int argc, char** argv) {
  // Check number of arguments
  if (argc!=4) {
    usage(argv[0]);
  }

  // File to be resampled
  string infile(argv[1]);
  string infile2(argv[2]);
  string outfile(argv[3]);

  cout << "Read in files\t\t\t"<<flush;
  Curve<Vector3> original(infile.c_str());
  Curve<Vector3> toorder(infile2.c_str());
  cout << "\t[OK]\n";
 
  int N = original.nodes();
  if (toorder.nodes()!=N){
    toorder.make_default();
    toorder.resample(N);
  }
  
  Curve<Vector3> result;
  int idx = 0; float mdist = 1e10, dist;
  for (int i=0;i<N;++i) {
    dist = (original[0].getPoint()-toorder[i].getPoint()).norm2();
    if (dist<mdist) {
      mdist = dist;
      idx = i;
    }
  }
  for (int i=0;i<N;++i)
    result.append(toorder[(idx+i)%N]);

  cout << "Write reordered curve to file\t";
  result.writePKF(outfile.c_str());
  cout << "\t[OK]\n";

  return 0;
}

#endif // DOXYGEN_SHOULD_SKIP_THIS
