/*!
  \file map.cpp
  \ingroup ToolsGroup
  \brief Map one curve to another.

  The tool maps a given curve to another given a parameter t between zero and
  one.  The curves must have the same number of nodes. Corresponding nodes are
  interpolated linearely (using t) and the resulting curve is written to the
  specified output file.
*/

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include "../include/CurveBundle.h"

int main(int argc, char **argv) {

  if (argc!=5) {
    cerr << "Usage : "<<argv[0]<<" <t> <pkf start> <pkf end> <pkf out>\n";
    cerr << "\nt\tParameter in [0,1]. 0 returns <pkf start>, 1 <pkf end>\n";
    exit(0);
  }

  CurveBundle<Vector3> cb_start, cb_end, cb_out;

  cout << "Read in PKF initial and final curve";
  cb_start.readPKF(argv[2]);
  cb_end.readPKF(argv[3]);
  cb_out.header(cb_start.getName(),cb_start.getEtic(),
  		cb_start.getCite(),cb_start.getHistory());
  cout << "\t\t\t\t[OK]\n";

  float epsilon = atof(argv[1]);
  assert(epsilon<=1.0 && epsilon>=0.0);

  if (epsilon == 1.0) cb_end.writePKF(argv[4]);
  else if (epsilon == 0.0) cb_start.writePKF(argv[4]);

  assert(cb_start.curves()==cb_end.curves());

  for (int i=0;i<cb_start.curves();i++) {
    cout << "Compute intermediate curve " << flush;

    Curve<Vector3> tmp;
    tmp = cb_start[i] + (cb_end[i]-cb_start[i])*epsilon;
    tmp.computeTangents();
 
    cb_out.newCurve(tmp);
  }

  cout << "Write new curve to file";
  cb_out.writePKF(argv[4]);
  cout << "\t\t[OK]\n";
  
  return 0;

}

#endif // DOXYGEN_SHOULD_SKIP_THIS
