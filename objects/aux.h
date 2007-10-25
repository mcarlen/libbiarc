/*!
  \file aux.h
  \ingroup ObjectGroup
  \brief Auxiliary routines that produce particular curves

  The routines contained in this file are convenience functions
  needed for generating f.ex circle, ellipse, and more. Read
  the source for more info.
*/

#ifndef DOXYGEN_SHOULD_SKIP_THIS
#ifndef __AUX_OBJECTS_H__
#define __AUX_OBJECTS_H__

#include <Curve.h>

// Routines for a circle
void pointat_circle(float rad, float s, Vector3& p, Vector3& t) {
  // s in [0,1]
  p = Vector3(rad*sin(2.*M_PI*s), rad*cos(2.*M_PI*s),0.);
  t = Vector3(cos(2.*M_PI*s),-sin(2.*M_PI*s),0.);
  t.normalize();
}

Curve<Vector3>* gen_circle(float rad, int nodes) {
  Curve<Vector3>* circle = new Curve<Vector3>;
  Vector3 p,t;
  circle->header("Circle","libbiarc","","");

  for (int i=0;i<nodes;i++) {
    pointat_circle(rad, (float)i/(float)nodes,p,t);
    circle->append(p,t);
  }
  circle->link();
  circle->make_default();

  return circle;
}

// Routines for an ellipse
void pointat_ellipse(float a, float b, float s, Vector3& p, Vector3& t) {
  // s in [0,1]
  float omega = 2*M_PI;
  p = Vector3(a*cos(s*omega), b*sin(s*omega),0);
  t = Vector3(-a*sin(s*omega), b*cos(s*omega),0);
  t.normalize();
}

Curve<Vector3>* gen_ellipse(float a, float b, int nodes) {
  Curve<Vector3>* ellipse = new Curve<Vector3>;
  Vector3 p,t;
  ellipse->header("Ellipse","libbiarc","","");
  for (int i=0;i<nodes;i++) {
    pointat_ellipse(a,b,(float)i/(float)nodes,p,t);
    ellipse->append(p,t);
  }
  ellipse->link();
  ellipse->make_default();

  return ellipse;
}

// TODO
// helix
// solenoid

// bone shaped curve
void pointat_bone(float a, float b, float scale, float s,
                  Vector3& p, Vector3& t) {
  // s in [0,1]
  float omega = 2*M_PI;
  float ex0 = exp(-pow(s*omega*2.-3.*M_PI,2.));
  float ex0p = ex0*(12.*M_PI-8.*s*omega);
  float ex1 = exp(-pow(s*omega*2.-M_PI,2.));
  float ex1p = ex1*(4.*M_PI-8.*s*omega);

  p = Vector3(a*cos(s*omega),
              b*sin(s*omega)+scale*(ex0-ex1),
              0.);
  t = Vector3(-a*sin(s*omega),
              b*cos(s*omega)+scale*(ex0p-ex1p),
              0.);

/*
  float ss = (s+0.1)*7*2.*M_PI;
  p = Vector3(s*2.,
              sin(ss)/ss, //-ex1),
              0.);
  t = Vector3(1.,
              (cos(ss)*ss-sin(ss))/ss/ss,0.);
 */
  //t.normalize();
}

Curve<Vector3>* gen_bone(float a, float b, float scale, int nodes) {
  Curve<Vector3>* bone = new Curve<Vector3>;
  Vector3 p,t;
  bone->header("Bone shape","libbiarc","","");
  for (int i=0;i<nodes;i++) {
    pointat_bone(a,b,scale,(float)i/(float)nodes,p,t);
    bone->append(p,t);
  }
bone->computeTangents();
  return bone;
}

// Eight shaped curve without intersection
void pointat_inf(float a, float b, float scale, float s,
                 Vector3& p, Vector3& t) {
  // s in [0,1]
  float omega = 2*M_PI;
  float ex0 = exp(-pow(s*omega*2.-3.*M_PI,2.));
  float ex0p = ex0*(12.*M_PI-8.*s*omega);
  float ex1 = exp(-pow(s*omega*2.-M_PI,2.));
  float ex1p = ex1*(4.*M_PI-8.*s*omega);

  p = Vector3(a*cos(s*omega),b*sin(2.*s*omega),scale*(ex0-ex1));
  t = Vector3(-a*sin(s*omega),2.*b*cos(2.*s*omega),scale*(ex0p-ex1p));
  t.normalize();
}

Curve<Vector3>* gen_inf(float a, float b, float scale, int nodes) {
  Curve<Vector3>* inf = new Curve<Vector3>;
  Vector3 p,t;
  inf->header("Infinity","libbiarc","","");
  for (int i=0;i<nodes;i++) {
    pointat_inf(a,b,scale,(float)i/(float)nodes,p,t);
    inf->append(p,t);
  }
  return inf;
}

// Routines for a stadium curve
void pointat_stadium(float rad, float l, float s, Vector3& p, Vector3& t) {
  // s in [0,1]
  float ss = s*(2.*M_PI*rad+2.*l);
  if (ss>=0. && ss<l) {
    p = Vector3(l/2.,rad,0.)+Vector3(-1,0,0)*ss;
    t = Vector3(-1,0,0);
  }
  else if (ss>=l && ss<(l+M_PI*rad)) {
    p = -Vector3(l/2.,0,0)+Vector3(-rad*sin(ss-l),
                 rad*cos(ss-l),0.);
    t = Vector3(-cos(ss-l),-sin(ss-l),0.);
  }
  else if (ss>=(l+M_PI*rad) && ss<(2.*l+M_PI*rad)) {
    p = Vector3(-l/2.,-rad,0.)+Vector3(1,0,0)*(ss-l-M_PI*rad);
    t = Vector3(1,0,0);
  }
  else {
    p = Vector3(l/2.,0,0)+Vector3(-rad*sin(ss-2.*l),
                rad*cos(ss-2.*l),0.);
    t = Vector3(-cos(ss-2.*l),-sin(ss-2.*l),0.);
  }
  t.normalize();
}

Curve<Vector3>* gen_stadium(float rad, float l, int nodes) {
  Curve<Vector3>* stadium = new Curve<Vector3>;
  Vector3 p,t;
  stadium->header("Stadiumcurve","libbiarc","","");

  for (int i=0;i<nodes;i++) {
    pointat_stadium(rad, l, (float)i/(float)nodes,p,t);
    stadium->append(p,t);
  }
  return stadium;
}

#endif // __AUX_OBJECTS_H__
#endif // DOXYGEN_SHOULD_SKIP_THIS
