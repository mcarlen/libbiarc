/*!
  \file hopf.cpp
  \ingroup ObjectGroup
  \brief Constructs a hopf links.
*/

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include "../include/CurveBundle.h"
#include <stdlib.h>

int main(int argc,char** argv) {
  if(argc != 2) {
    cerr << "Usage : " << argv[0] << " <nodes per ring>\n";
    exit(1);
  }

  int Nodes = atoi(argv[1]);

  CurveBundle<Vector3> hopf;
  Curve<Vector3> c[2];

  Vector3 p,t;

  for(int i=0;i<Nodes;i++) {

    float time=((float)i-(float)Nodes/2.0)/(float)Nodes*M_PI*2.0;

    float x = cos(time);
    float y = sin(time);
    float z = 0;
    
    t = Vector3(-sin(time),cos(time),0);
    t.normalize();

    c[0].append(Vector3(x-.5,y,z),t);
    c[1].append(Vector3(x+.5,-z,y),Vector3(t[0],-t[2],t[1]));
  }

  for (int i=0;i<2;i++)
    hopf.newCurve(c[i]);

  hopf.header("Hopf link","M.Carlen","","");
  if (!hopf.writePKF("hopf.pkf"))
    cerr << "problem writing file"<<endl;
   
  return 0;
}

#endif // DOXYGEN_SHOULD_SKIP_THIS
