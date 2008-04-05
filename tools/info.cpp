/*!
  \defgroup ToolsGroup PKF curve tools
*/

/*!
  \file info.cpp
  \ingroup ToolsGroup
  \brief Computes Arc-length, center of mass, thickness (approx), ropelength (aprox) info of a PKF curve.

  Long explanation ... Bla bla bla
*/

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include "../include/CurveBundle.h"
#include "../include/algo_helpers.h"

int main(int argc, char **argv) {

  if (argc!=2) {
    cerr << "Usage : "<<argv[0]<<" <pkf in>\n";
    exit(0);
  }

  Vector3 from,to;
  CurveBundle<Vector3> cb(argv[1]);
  cb.link();
  cb.make_default();

  cout.precision(16);
  cout << "\nPKF info for < " << cb.getName() << " >\n\n";

  for (int i=0;i<cb.curves();i++) {

    float L = cb[i].length();
    float D = cb[i].thickness(&from,&to);
    cout << "Curve " << i+1 << endl;
    cout << "------------------\n";
    cout << "Number of data pts : " << cb[i].nodes() << endl;
    cout << "Curve arc-length   : " << L << endl;
    cout << "Center of mass     : " << cb[i].getCenter() << endl;  
    cout << "Thickness (D=2r)   : " << D << endl;
    cout << "Roplength (L/D)    : " << L/D << endl;
    cout << "L/r                : " << L/D*2 << endl;
    cout << "Thickness between  : " << from << ", " << to << endl;
    cout << "==================\n";

  }

  return 0;

}

#endif // DOXYGEN_SHOULD_SKIP_THIS
