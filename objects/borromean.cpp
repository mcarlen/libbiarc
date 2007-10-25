/*!
  \file borromean.cpp
  \ingroup ObjectGroup
  \brief Constructs the borromean rings.

  Long explanation ... Bla bla bla
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

  CurveBundle<Vector3> borromean;
  Curve<Vector3> c[3];

  Vector3 p,t;

  for(int i=0;i<Nodes;i++) {

    float time=((float)i-(float)Nodes/2.0)/(float)Nodes*M_PI*2.0;

    float x = 6.0*cos(time);
    float y = 6.0*sin(time);
    float z = sin(3.0*time);
    
    t = Vector3(-6.0*sin(time),6.0*cos(time),3.0*sin(3.0*time));

    c[0].append(Vector3(x,y,z),t);
    c[1].append(Vector3(x+sqrt(27.0f),y+3,z),t);
    c[2].append(Vector3(x,y+6,z),t);
  }

  for (int i=0;i<3;i++)
    borromean.newCurve(c[i]);

  borromean.header("Borromean rings","M.Carlen","","");
  if (!borromean.writePKF("borromean.pkf"))
    cerr << "problem writing file"<<endl;
   
  return 0;
}

#endif // DOXYGEN_SHOULD_SKIP_THIS
