#ifndef MINSEGDIST
#define MINSEGDIST

#include <Biarc.h>
#include <assert.h>

/*
  The double criticality test is a translation
  of Jana's Matlab code (listing in her Thesis)
*/
template<class Vector>
int double_critical_test(const Vector &a0, const Vector &a1,
                         const Vector &t0a,const Vector &t1a,
                         const Vector &b0, const Vector &b1,
                         const Vector &t0b,const Vector &t1b) {
  Vector w = (a0+a1-b0-b1);
  float denum = w.norm();
  float val0 = (a0-a1).norm(), val1 = (b0-b1).norm();

  // Jana's Thesis eq 7.23
  // !! prevents thicknesscomputation on curves with a
  //    small number of nodes !!
  // not sure wheter this check is necessary in practice !!
  // assert(denum > val0+val1);

  // do balls intersect?
  if (denum<=(val0+val1)) return 1;

  float sina = (val0+val1)/denum;
  w.normalize();
  if ((w.dot(t0a)<-sina) && (w.dot(t1a)<-sina)) return 0;
  if ((w.dot(t0a)>sina)  && (w.dot(t1a)>sina)) return 0;
  if ((w.dot(t0b)<-sina) && (w.dot(t1b)<-sina)) return 0;
  if ((w.dot(t0b)>sina)  && (w.dot(t1b)>sina)) return 0;

  return 1;
}

template<class Vector>
int double_critical_test_v2(const Vector &a0,const Vector &a1,const Vector3 &a2,
                            const Vector &b0,const Vector &b1,const Vector3 &b2) {
  // XXX :
  // Possible optimization : since the Bezier triangles are
  // equilateral, only 2 of the 4 norms need to be calculated!
  // smthing like : invnorma = 1./(a1-a0).norm(); (a1-a0)*invnorma ...
  Vector3 ta0 = a1-a0, ta2 = a2-a1;
  ta0.normalize(); ta2.normalize();

  Vector3 tb0 = b1-b0, tb2 = b2-b1;
  tb0.normalize(); tb2.normalize();

  double_critical_test(a0,a2,
                       ta0,ta2,
                       b0,b2,
                       tb0,tb2);
}

#endif // MINSEGDIST
