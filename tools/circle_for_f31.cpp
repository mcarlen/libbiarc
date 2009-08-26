/*!
  \file circle_for_f31.cpp
  \ingroup ToolsGroup
  \brief Given eps, compute using 3 points the radius and center of
         circle (compare it to gamma(.5), which might be a center!).
*/

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include "../include/Curve.h"
#include "../include/algo_helpers.h"
#include "../experimental/pngmanip/colors.h"
#include <algorithm>

// http://en.wikipedia.org/wiki/Circumscribed_circle
void circle3p(Vector3& p1, Vector3& p2, Vector3& p3) {
  Vector3 a = (p1-p2), b = (p2-p3), c = (p3-p1);
  float radius = a.norm()*b.norm()*c.norm()/2./(a.cross(b)).norm();

  float alpha, beta, gamma, denom;
  denom = 2*(a.cross(b)).norm2();
  alpha = b.norm2()*(a.dot(-c))/denom;
  beta  = c.norm2()*((-a).dot(b))/denom;
  gamma = a.norm2()*(c.dot(-b))/denom;

  Vector3 center = alpha*p1 + beta*p2 + gamma*p3;

  // cerr << "Radius " << radius << "\nCenter " << center << endl;
  cerr << radius << " " << center;

  Vector3 e1 = p1-center, e2 = c-center;
  e1.normalize();
  e2 = e2 - e1.dot(e2)*e1;
  e2.normalize();

}

int main(int argc, char **argv) {

  Curve<Vector3> c(argv[1]);
  float eps = atof(argv[2]);
  c.link();
  c.make_default();

  Vector3 p0 = c.pointAt(0.0);
  Vector3 p1 = c.pointAt(eps);
  Vector3 p2 = c.pointAt(1-eps);

  cerr << eps << " ";
  circle3p(p0,p1,p2);
  cerr << endl;

  // cout << "Special point : " << c.pointAt(.5) << endl;

  return 0;
}

#endif // DOXYGEN_SHOULD_SKIP_THIS
