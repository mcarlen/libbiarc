/*!
  \file info4.cpp
  \ingroup ToolsGroup
  \brief Computes Arc-length, center of mass, thickness (approx), ropelength (aprox) info of a PKF curve (S^3).
*/

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include "../include/CurveBundle.h"
#include "../include/algo_helpers.h"
#include "../include/Vector4.h"

void usage(const char* prog) {
  cerr << "Usage : "<< prog <<" [-open] <pkf in>\n";
  exit(0);
}

int main(int argc, char **argv) {

  int fd = 1, CLOSED = 1; 

  if (argc==3) {
    if (!strncmp(argv[1],"-open",5)) CLOSED = 0;
    else usage(argv[0]);
    fd = 2;
  }
  else if (argc!=2)
    usage(argv[0]);

  CurveBundle<Vector4> cb(argv[fd]);
  if (CLOSED)
    cb.link();
  cb.make_default();

  cout.precision(16);
  cout << "\nPKF info for < " << cb.getName() << " >\n\n";

  float off_equi, L, D;
  for (int i=0;i<cb.curves();i++) {

    off_equi = (cb[i].maxSegDistance() / cb[i].minSegDistance());
    L = cb[i].length();
    if (CLOSED)
      D = cb[i].thickness();
    else
      D = cb[i].thickness_fast();

    cout << "Curve " << i+1 << endl;
    cout << "------------------\n";
    cout << "Number of data pts : " << cb[i].nodes() << endl;
    cout << "Curve arc-length   : " << L << endl;
    cout << "Center of mass     : " << cb[i].getCenter() << endl;  
    cout << "Thickness (D=2r)   : " << D << endl;
    cout << "Roplength (L/D)    : " << L/D << endl;
    cout << "L/r                : " << L/D*2 << endl;
    cout << "maxArc/minArc      : " << off_equi << endl;
    cout << "==================\n";

  }

  return 0;

}

#endif // DOXYGEN_SHOULD_SKIP_THIS
