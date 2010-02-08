/*!
  \file ptcircles.cpp
  \ingroup ToolsGroup
  \brief Resample an open or closed PKF curve.
*/

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include "../include/CurveBundle.h"
#include <stdlib.h>

void usage(char* prog) {
  cerr << "Usage : " << prog
       <<" s0 samples pkffile\n";
  exit(1);
}

int CircleN = 16;

int main(int argc, char** argv) {
  // Check number of arguments
  if (argc!=4) {
    usage(argv[0]);
  }

  string infile(argv[3]);
  double fixedVal = atof(argv[1]);
  int Seg = atoi(argv[2]);

  Curve<Vector3> orig(infile.c_str());
  orig.link();
  orig.make_default();
  
  for (int j=1;j<Seg;++j) {
    Vector3 e1, e2, c, p0, p1, cp, ct;
    double pt, s, t = fixedVal, val;
    Curve<Vector3> circle;

    s = (float)j/(float)Seg*orig.length();

    e1 = orig.tangentAt(s);
    p0 = orig.pointAt(s);
    p1 = orig.pointAt(t);

    pt = orig.radius_pt(p0,e1,p1);

    e2 = p1-p0;
    e2.normalize();
    e2 = e2 - e2.dot(e1)*e1;
    e2.normalize();
    c = p0 + pt*e2;
    e2 = -e2;
    /*
    cout << pt << endl;
    cout << c << endl;
    cout << e1 << ", " << e2 << endl;
    cout << e1.dot(e2) << endl;
    */
    cout << p0 << " " << p1 << endl;
    for (int i=0;i<CircleN;++i) {
      val = (float)i*2.*M_PI/(float)CircleN;
      cp = c + pt*e1*cos(val) + pt*e2*sin(val);
      ct = c - pt*e1*sin(val) + pt*e2*cos(val);
      circle.append(cp,ct);
    }
    circle.link();
    circle.computeTangents();
    char name[100];
    sprintf(name, "circle%04d.pkf", j);
    circle.writePKF(name);
  }

  return 0;
}

#endif // DOXYGEN_SHOULD_SKIP_THIS
