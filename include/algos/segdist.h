#ifndef __SEG_DIST__
#define __SEG_DIST__

#include "../Vector3.h"

// This comes from David Eberly www.geometrictools.com
// we basically minimize the quadratic function
// Q(s,t) = as^2+2bst+ct^2+2ds+2et+f

FLOAT_TYPE aux_edge(FLOAT_TYPE tmp, FLOAT_TYPE denum) {
  // F(t) = Q(1,t) = (a+2*d+f)+2*(b+e)*t+c*t^2
  // F'(T) = 0 when T = -(b+e)/c
  if (tmp>0.0)
    return 0.0;
  else {
    if (-tmp>denum)
      return 1.0;
    else
      return -tmp/denum;
  }
}

void aux_corner(FLOAT_TYPE s_in, FLOAT_TYPE t_in, FLOAT_TYPE Qs, FLOAT_TYPE Qt,
                 FLOAT_TYPE edge0, FLOAT_TYPE denum0, FLOAT_TYPE edge1,
                 FLOAT_TYPE denum1, FLOAT_TYPE &s, FLOAT_TYPE &t) {
  if (Qs>0.0) { t=t_in;s=aux_edge(edge0,denum0); }
  else { s=s_in;
    if (Qt>0.0) t=aux_edge(edge1,denum1);
    else t = t_in;
  }
}

template<class Vector>
FLOAT_TYPE min_seg_dist(const Vector &B0, const Vector &B0p, const Vector &B1,
                   const Vector &B1p,FLOAT_TYPE &s,FLOAT_TYPE &t) {
  const FLOAT_TYPE tol = 1e-12;
  Vector D = B0-B1, M0 = B0p-B0, M1 = B1p-B1;
  FLOAT_TYPE a = M0.dot(M0), b = -M0.dot(M1), c = M1.dot(M1);
  FLOAT_TYPE d = M0.dot(D), e = -M1.dot(D), f = D.dot(D);
  FLOAT_TYPE det = fabsf(a*c-b*b);
  FLOAT_TYPE invDet, tmp;
  s = b*e-c*d; t = b*d-a*e;

  if (det>=tol) {
    // non parallel case
    if (s>=0.0) {
      if (s<=det) {
        if (t>=0.0) {
          if (t<=det) { // region 0
            invDet = 1.0/det; s*= invDet; t*=invDet; }
          else { // region 3
            t=1.0; s=aux_edge(b+d,a);
          }
        }
        else { // region 7
          t=0.0; s=aux_edge(d,a);
        }
      }
      else {
        if (t>=0.0) {
          if (t<=det) { // region 1
            s = 1.0; t=aux_edge(b+e,c);
          }
          else { // region 2
            aux_corner(1.0,1.0,a+b+c,b+c+e,b+d,a,b+e,c,s,t);
          }
        }
        else { // region 8
          aux_corner(1.0,0.0,a+d,b+e,d,a,b+e,c,s,t);
        }
      }
    }
    else { // s<0
      if (t>=0.0) {
        if (t<=det) { // region 5
          s = 0.0; t=aux_edge(e,c);
        } else { // region 4
            aux_corner(0.0,1.0,-(b+d),c+e,b+d,a,e,c,s,t);
          }
      }
      else { // region 6
        aux_corner(0.0,0.0,-d,-e,d,a,e,c,s,t);
      }
    }
  }
  else {
    // parallel case
    // cerr << "Parallel case!\n";
    if (b>0) {
      if (d>=0) { s=0.0; t=0.0; }
      else {
        if (-d<=a) { s=-d/a; t=0.0; }
        else {
          s=1.0; tmp = a+d; if (-tmp>=b) t=1.0; else t = -tmp/b;
        }
      }
    }
    else {
      if (-d>=a) { s=1.0;t=0.0; }
      else { if (d<=0.0) { s=-d/a; t=0.0; }
      else { s=0.0; if (d>=-b) t=1.0; else t=-d/b;} }
    }
  }
  return ((B0+(B0p-B0)*s)-(B1+(B1p-B1)*t)).norm();
}

#endif // __SEG_DIST__
