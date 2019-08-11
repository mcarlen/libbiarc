/*!
 \file shuffle.cpp
 \ingroup ToolsGroup
 \brief Randomize/shuffle a curve.
*/

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include "../include/Curve.h"
#include <stdlib.h>

#ifdef Dim4
#define VecType Vector4
#else
#define VecType Vector3
#endif

void inversion_in_sphere(Curve<VecType>* curve, VecType center = VecType(),
                         float radius = 2.0) {
  VecType p,pnew,t,tnew,v;
  float factor, vnorm2;
  for (int i=0;i<curve->nodes();++i) {

    p = (*curve)[i].getPoint();
    t = (*curve)[i].getTangent();

    v = (p-center);
    vnorm2 = v.norm2();

    factor = radius*radius/vnorm2;
    pnew = center + v*factor;
    tnew = t*factor - v*(2*factor/vnorm2*t.dot(v));
    tnew.normalize();
    (*curve)[i].setPoint(pnew);
    (*curve)[i].setTangent(tnew);
  }
}

#define rand01() ((float)rand()/(float)RAND_MAX)
#define myrand() (rand01()*2.-1.)
#define randminmax(a,b) ((a)+((b)-(a))*rand01())
#define randvec() VecType(myrand(),myrand(),myrand())

float pick_random_plane(Curve<VecType>* curve, VecType* n, float *dist) {
  *n = randvec();
  n->normalize();

  // compute the furthest and closest distance
  // between the curve points and this plane
  float dmin = 1+99, dmax = -1e-99;
  for (int i=0;i<curve->nodes();++i) {
    float d = n->dot((*curve)[i].getPoint());
    if (d<dmin) dmin = d;
    else if (d>dmax) dmax = d;
  }
  *dist = dmin + .5*(dmax-dmin) ; // randminmax(dmin,dmax);
  return dmax;
}

void scale_move(Curve<VecType>* curve) {
  VecType n; float plane_d;
  float dmax = pick_random_plane(curve,&n,&plane_d);

  float magnitude = 1.+(*curve)[0].biarclength()/10.*rand01();
  for (int i=0;i<curve->nodes();++i) {
    VecType p = (*curve)[i].getPoint();
    float val = p.dot(n);
    if (val > 0 && std::abs(val)>plane_d) {
      // stretch point
      float blend = (val-plane_d)/(dmax-plane_d);
      p = p + n*magnitude*blend*blend*blend;
      (*curve)[i].setPoint(p);
    }
  }
}

void rot_move(Curve<VecType>* curve) {
  VecType n; float plane_d;
  float dmax = pick_random_plane(curve,&n,&plane_d);

  float rot_angle = M_PI/2.+rand01()*M_PI/2; // rand01()*M_PI;

  // rotate about axis going through center
  VecType proj_center = curve->getCenter() - curve->getCenter().dot(n)*n;

  for (int i=0;i<curve->nodes();++i) {
    VecType p = (*curve)[i].getPoint();
    float val = p.dot(n);
    if (val > 0 && std::abs(val)>plane_d) {
      // rot point
      float blend = (val-plane_d)/(dmax-plane_d);
      p = (p-proj_center).rotPtAroundAxis(rot_angle*blend*blend, n) + proj_center;
      (*curve)[i].setPoint(p);
    }
  }
}

int main(int argc, char** argv) {
  if (argc!=3) {
    cout << "Usage : " << argv[0] << " curve.pkf out.pkf\n";
    exit(0);
  }
  srand(time(NULL));

  Curve<VecType> curve(argv[1]);
  curve.link();
  curve.make_default();
  curve.normalize();

  float thick;

  rot_move(&curve);
  curve.computeTangents();
  curve.center();
  curve.make_default(); curve.normalize();
  curve.make_default();

  /*
  scale_move(&curve);
  curve.computeTangents();
  curve.center();
  curve.make_default(); curve.normalize();
  curve.make_default();
  */

  inversion_in_sphere(&curve, randvec(), .2*rand01());
  curve.center();
  curve.make_default(); curve.normalize();
  curve.make_default();

  thick = curve.thickness_fast();
  int N = 10./thick;
  if (N>1000) N = 1000;
  curve.resample(N);

  curve.header(curve.getName(),"shuffle","","");
  curve.writePKF(argv[2]);

  return 0;
}

#endif
