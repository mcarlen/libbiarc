/*!
  \file s3viz.cpp
  \ingroup InventorGroup
  \brief Visualize a curve in S^3 in two balls in R^3.
*/

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include "CurveBundle.h"

#include <stdlib.h>

void usage(char* prog) {
  cerr << "Usage : " << prog <<" <S2 Infile>\n";
  exit(1);
}

int main(int argc, char** argv) {
  // Check number of arguments
  if (argc!=2) {
    usage(argv[0]);
  }

  string infile(argv[1]);

  // Process infile to outfile

  Curve<Vector3> original(infile.c_str());
  CurveBundle<Vector3> hemi_up, hemi_down;
  hemi_up.newCurve(new Curve<Vector3>);
  hemi_down.newCurve(new Curve<Vector3>);

  char buf[1024];
  sprintf(buf,"%s upper hemi",original.getName());
  hemi_up.header(buf,"libbiarc");
  sprintf(buf,"%s lower hemi",original.getName());
  hemi_down.header(buf,"libbiarc");

  cout << "hemi up/down curve no : " << hemi_up.curves() << "/"<<hemi_down.curves() << endl;

  original.link();
  
  // hemi says in what hemisphere we're currently working
#define HEMI_UP 1
#define HEMI_DOWN 2
  int hemi = (original[0].getPoint()[2]>0?HEMI_UP:HEMI_DOWN);
  Vector3 p;
  Vector3 t;
  for (vector<Biarc<Vector3> >::iterator
       i=original.begin();i!=original.end();++i) {
    p = i->getPoint();
    if (p[2]>0) {
      if (hemi==HEMI_DOWN) {
        cout << "down -> up : new component" << flush;
        if (hemi_up[hemi_up.curves()-1].nodes()>1)
          hemi_up.newCurve(new Curve<Vector3>);
        else
          hemi_up[hemi_up.curves()-1].flush_all();
        cout << "up has " << hemi_up.curves() << " components";
        hemi = HEMI_UP;
        cout << " [ok]\n" << flush;
      }
      hemi_up[hemi_up.curves()-1].append(Vector3(p[0],p[1]+1.2,0),t);
    }
    else {
      if (hemi==HEMI_UP) {
        cout << "up -> down : new component" << flush;
        if (hemi_down[hemi_down.curves()-1].nodes()>1)
          hemi_down.newCurve(new Curve<Vector3>);
        else
          hemi_down[hemi_down.curves()-1].flush_all();
        cout << "up down " << hemi_down.curves() << " components";
        hemi = HEMI_DOWN;
        cout << " [ok]\n" << flush;
      }
      hemi_down[hemi_down.curves()-1].append(Vector3(p[0],p[1]-1.2,0),t);
    }
  }
    
  if (hemi_up[0].nodes()>1) {
    hemi_up.computeTangents();
    cout << "Write up.pkf.\n" << flush;
    hemi_up.writePKF("up.pkf");
  }
  else
    cout << "Upper hemisphere is empty.\n";
  if (hemi_down[0].nodes()>1) {
    hemi_down.computeTangents();
    cout << "Write down.pkf.\n" << flush;
    hemi_down.writePKF("down.pkf");
  }
  else
    cout << "Lower hemisphere is empty.\n";

  cout << "Finished\n";

  return 0;
}

#endif // DOXYGEN_SHOULD_SKIP_THIS
