/*!
  \file tangent_indicatrix.cpp
  \ingroup ToolsGroup
  \brief Write the tangent indicatrix of a given (open or closed) curve to
         a PKF file.
	 Tangents for the tangent indicatrix are interpolated, therefore strange
	 points like cusps would not be visible! The default is for an open curve,
	 for closed curves put the -closed switch.

*/

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include "../include/CurveBundle.h"

int main(int argc, char **argv) {

  if (argc<3 || argc>5) {
    cout << "Usage : "<<argv[0]
	 << " [-closed] [-neg] <pkf in> <indicatrix pkf>\n";
    exit(0);
  }

  int Neg = 0;
  int Closed = 0;

  if (argc>3)
    for (int i=1;i<argc-2;i++) {
      if (!strcmp(argv[i],"-closed")) Closed = 1;
      if (!strcmp(argv[i],"-neg")) Neg = 1;
    }

  CurveBundle<Vector3> cb(argv[argc-2]);
  if (Closed) cb.link();

  CurveBundle<Vector3> indicatrix;
  Vector3 tan_tmp, v1, v2;

  cout << "Construct indicatrix " << flush;
  for (int i=0;i<cb.curves();i++) {

    Curve<Vector3> tmp;

    vector<Biarc<Vector3> >::iterator current = cb[i].begin();
    while (current!=cb[i].end()) {
      tan_tmp = current->getTangent();
      if (Neg) tan_tmp=-tan_tmp;
      tmp.append(tan_tmp,Vector3(0,0,0));
      current++;
    }
    tmp.computeTangents();

    // Put the tangents in the sphere tangent space !
    current = tmp.begin();
    while (current!=tmp.end()) {
      v1 = current->getPoint();
      v2 = current->getTangent();
      tan_tmp = v2 - v1.dot(v2)*v1;
      tan_tmp.normalize();
      current->setTangent(tan_tmp);
      current++;
    }
    indicatrix.newCurve(tmp);
  }
  cout << "\t\t[OK]\n";  

  cout << "Write indicatrix to "<< argv[argc-1];
  indicatrix.writePKF(argv[argc-1]);
  cout << "\t[OK]\n";  

  return 0;

}

#endif // DOXYGEN_SHOULD_SKIP_THIS
