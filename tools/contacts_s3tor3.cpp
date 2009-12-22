/*!
 \file contacts_s3tor3.cpp
 \ingroup ToolsGroup
 \brief Project contact chords from S^3 to R^3.
*/

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include "Vector4.h"
#include <fstream>
#include <stdlib.h>

#define myprint(a) (a)[0] << " " << (a)[1] << " " << (a)[2]

// inversion in a sphere centered at (0,0,0,1) and radius 2.0
Vector4 inversion_in_sphere(Vector4 &p, Vector4 center = Vector4(0,0,0,1),
                         float radius = 2.0) {
  Vector4 pnew,t,tnew,v; 
  float factor, vnorm2;

  v = (p-center);
  vnorm2 = v.norm2();
 
  factor = radius*radius/vnorm2;
  pnew = center + v*factor;
  return pnew;
}

int main(int argc, char** argv) {
  if (argc!=2) {
    cout << "Usage : " << argv[0] << " contacts\n";
    exit(0);
  }

  Vector4 p[3], q[3];
  ifstream in(argv[1]);
  while (in >> p[0] >> p[1] >> p[2]) {
    for (int i=0;i<3;++i)
      q[i] = inversion_in_sphere(p[i]);
    cout << myprint(q[0]) << ", " << myprint(q[2]) << ", " << endl;
  }
  in.close();

  return 0;
}

#endif
