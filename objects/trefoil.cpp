/*!
  \defgroup ObjectGroup Object Directory
*/

/*!
  \file trefoil.cpp
  \ingroup ObjectGroup
  \brief Approximated trefoil with arcs of circles ...

*/

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include "aux.h"
#include <CurveBundle.h>

#define ROT
#define TRANS

//float offset_angle = M_PI+M_PI/3., sweep_angle = 4.*M_PI/3.;
float sweep_angle = 3.5*M_PI/3.;

Curve<Vector3>* gen_pcircle(float rad, int nodes,float offset) {
  Curve<Vector3>* circle = new Curve<Vector3>;
  Vector3 p,t;
  circle->header("Partial Circle","libbiarc","","");

  for (int i=0;i<nodes;i++) {
    pointat_circle(rad, offset/2./M_PI+(float)i*(sweep_angle/2./M_PI)/(float)(nodes-1),p,t);
    circle->append(p,t);
  }

  return circle;
}

int main(int argc,char** argv) {
  if (argc!=5) {
	  cerr << "Usage "<<argv[0]<<" <rad> <sweep> <tri-rad> <rot-angle> <nodes>\n";
	  exit(1);
  }
  float rad  = atof(argv[1]);
  int nodes  = atoi(argv[5]);
  sweep_angle = atof(argv[2]);  
  float TS = 2;
  TS = atof(argv[3]);
  float ang = M_PI/6.;
  ang = atof(argv[4]);

  CurveBundle<Vector3> b;
  Curve<Vector3> *c1,*c2,*c3;

  // generate 3 parts of circles
  c1 = gen_pcircle(rad,nodes/3,2.*M_PI-sweep_angle/2.);
  c2 = gen_pcircle(rad,nodes/3,2.*M_PI-sweep_angle/2.+2.*M_PI/3.);
  c3 = gen_pcircle(rad,nodes/3,2.*M_PI-sweep_angle/2.-2.*M_PI/3.);

#ifdef ROT
  // out of plane rotation
  c1->rotAroundAxis(ang,Vector3(0,1,0));
  c2->rotAroundAxis(ang,Vector3(sin(2.*M_PI/3.),cos(2.*M_PI/3.),0));
  c3->rotAroundAxis(ang,Vector3(-sin(2.*M_PI/3.),cos(2.*M_PI/3.),0));
#endif

#ifdef TRANS
  // translate the 3 curves
  (*c1)+=(Vector3(0,cos(M_PI/3.),0)*TS);
  (*c2)+=(Vector3(sin(2.*M_PI/3.),cos(2.*M_PI/3.),0)*cos(M_PI/3.)*TS);
  (*c3)+=(Vector3(-sin(2.*M_PI/3.),cos(2.*M_PI/3.),0)*cos(M_PI/3.)*TS);
#endif

  Curve<Vector3> cc;
  for (int i=1;i<c1->nodes()-1;i++)
    cc.append((*c1)[i]);

  cc.append((*c1)[c1->nodes()]);
  cc.append((*c3)[0]);

  for (int i=1;i<c3->nodes()-1;i++)
    cc.append((*c3)[i]);

  cc.append((*c3)[c3->nodes()]);
  cc.append((*c2)[0]);

  for (int i=1;i<c2->nodes()-1;i++)
    cc.append((*c2)[i]);

  cc.append((*c2)[c2->nodes()]);
  cc.append((*c1)[0]);

  cc.make_default();
  cc.resample(nodes);

  b.newCurve(*c1);
  b.newCurve(*c2);
  b.newCurve(*c3);

  cc.make_default();
  float l = cc.length();
  cc.normalize();
  b.scale(1./l);

  if (!cc.writePKF("trefoil.pkf"))
    cerr << "Problem writing file."<<endl;
  if (!b.writePKF("trefoil2.pkf"))
    cerr << "Problem writing file."<<endl;

  delete c1; delete c2; delete c3;
  return 0;
}

#endif // DOXYGEN_SHOULD_SKIP_THIS
