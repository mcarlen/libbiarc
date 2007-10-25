/*!
  \file resample.cpp
  \ingroup ToolsGroup
  \brief Resample an open or closed PKF curve.

  Long explanation ... Bla bla bla
*/

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include <CurveBundle.h>
#include <stdlib.h>

void usage(char* prog) {
  cerr << "Usage : " << prog
       <<" <Infile> [Outfile]\n";
  exit(1);
}

int main(int argc, char** argv) {
  // Check number of arguments
  if (argc<2||argc>3) {
    usage(argv[0]);
  }

  // File to be resampled
  string infile(argv[1]);

  string outfile;
  if (argc==3)
    outfile = string(argv[2]);
  else {
    cout << "No output file specified, write to default.pkf!\n";
    outfile = "default.pkf";
  }

  // Process infile to outfile
  cout << "Read in file\t\t\t"<<flush;
  CurveBundle<Vector3> original(infile.c_str());
  cout << "\t[OK]\n";
 
  cout << "Re-compute the tangents!\n";
  original.computeTangents();
  
  cout << "Write curve to file\t";
  original.writePKF(outfile.c_str());
  cout << "\t[OK]\n";

  return 0;
}

#endif // DOXYGEN_SHOULD_SKIP_THIS
