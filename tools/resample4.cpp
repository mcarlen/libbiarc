/*!
  \file resample.cpp
  \ingroup ToolsGroup
  \brief Resample an open or closed PKF curve.

  Long explanation ... Bla bla bla
*/

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include "../include/CurveBundle.h"
#include <stdlib.h>

#define TVec Vector4
#include "Vector4.h"

void usage(char* prog) {
  cerr << "Usage : " << prog
       <<" <closed=1;open=0> <Number of points> <Infile> [Outfile]\n";
  exit(1);
}

int main(int argc, char** argv) {
  // Check number of arguments
  if (argc<4||argc>5) {
    usage(argv[0]);
  }

  // Segmentation value
  int N = atoi(argv[2]);

  // File to be resampled
  string infile(argv[3]);

  string outfile;
  if (argc==5)
    outfile = string(argv[4]);
  else {
    cout << "No output file specified, write to default.pkf!\n";
    outfile = "default.pkf";
  }

  // Process infile to outfile

  cout << "Read in file\t\t\t"<<flush;
  CurveBundle<TVec> original(infile.c_str());
  cout << "\t[OK]\n";
 
  if (atoi(argv[1]))
    original.link();
  
  cout << "Interpolate pt/tan by biarcs\t";
  original.make_default();
  cout << "\t[OK]\n";
  
  cout << "Resample curve with "<< N << " points\t" << flush;
  original.resample(N);
  cout << "\t[OK]\n";
  
  for (int i=0;i<original.curves();i++)
    original[i].check_tangents();

  cout << "Write resampled curve to file\t";
  original.writePKF(outfile.c_str());
  cout << "\t[OK]\n";

  return 0;
}

#endif // DOXYGEN_SHOULD_SKIP_THIS
