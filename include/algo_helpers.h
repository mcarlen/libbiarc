#ifndef __ALGO_HELPERS__
#define __ALGO_HELPERS__

#include <vector>
#include "Vector3.h"
#include "Vector4.h"
#include "Biarc.h"
#include "CurveBundle.h"
#include "Tube.h"

#define biarc_it  typename vector< Biarc<Vector> >::iterator
#define candi_it typename vector<Candi<Vector> >::iterator

#define ITERATE

template<class Vector>
class ArcInfo{
public:
  Vector b0,b1,b2;
  Vector m; // Midpoint
  FLOAT_TYPE err,ferr,omega;
  ArcInfo() {}
  ArcInfo(const Vector &a0,const Vector &a1,const Vector &a2);
  ArcInfo(const Vector &a0,const Vector &a1,const Vector &a2,
          const FLOAT_TYPE &Err, const FLOAT_TYPE &FErr);
};

// Possible candidate for thickness achievement
template<class Vector>
class Candi {
public:
  ArcInfo<Vector> a,b;
  FLOAT_TYPE d;

  Candi() {};
  Candi(const Vector &a0,const Vector &a1,const Vector &a2,
        const Vector &b0,const Vector &b1,const Vector &b2);
  Candi(const Vector &a0,const Vector &a1,const Vector &a2,
        const FLOAT_TYPE &a_err,const FLOAT_TYPE &a_ferr,
        const Vector &b0,const Vector &b1,const Vector &b2,
        const FLOAT_TYPE &b_err,const FLOAT_TYPE &b_ferr);
};

template<class Vector>
inline ostream & operator<< (ostream &out, const Candi<Vector> &c) {
  Vector3 ta0 = (c.a.b1-c.a.b0); ta0.normalize();
  Vector3 ta1 = (c.a.b2-c.a.b1); ta1.normalize();

  Vector3 tb0 = (c.b.b1-c.b.b0); tb0.normalize();
  Vector3 tb1 = (c.b.b2-c.b.b1); tb1.normalize();

  out << c.a.b0 << " " << c.a.b2 << " " << ta0 << " " << ta1 << " "
      << c.b.b0 << " " << c.b.b2 << " " << tb0 << " " << tb1 << " " << c.d;
  return out;
}

template<class Vector>
void get_it(Vector b0,Vector b1,Vector b2,FLOAT_TYPE r,Vector &b, Vector &c);

template<class Vector>
int rhopt(Vector p, Vector b0,Vector b1,Vector b2,FLOAT_TYPE r,Vector &v);

template<class Vector>
void dump_candidates(vector<Candi<Vector > > *C,int iter = 0);

template<class Vector>
FLOAT_TYPE check_local_curvature(Curve<Vector>* c);

template<class Vector>
void initial_dbl_crit_filter(Curve<Vector>* c, vector<Candi<Vector> > &CritC,
                             FLOAT_TYPE dCurrentMin);

template<class Vector>
void dbl_crit_filter(vector<Candi<Vector> > &C,vector<Candi<Vector> > &CritC, FLOAT_TYPE dCurrentMin);

template<class Vector>
void compute_thickness_bounds(vector<Candi<Vector> > &C,FLOAT_TYPE md, FLOAT_TYPE &lb, FLOAT_TYPE &ub, FLOAT_TYPE &err, candi_it &min_candi);

template<class Vector>
void distance_filter(vector<Candi<Vector> > &C,vector<Candi<Vector> > &Cfiltered, const FLOAT_TYPE dCurrMin = 1e22);

template<class Vector>
FLOAT_TYPE compute_thickness(Curve<Vector> *c, Vector *from = NULL, Vector *to = NULL, const int hint_i = -1, const int hint_j = -1);

template<class Vector>
FLOAT_TYPE compute_thickness(CurveBundle<Vector> *cb, Vector *from = NULL, Vector *to = NULL);

template<class Vector>
FLOAT_TYPE mindist_between_bezier_arcs(const Vector &a0,const Vector &a1,const Vector &a2,
                                  const Vector &b0,const Vector &b1,const Vector &b2,
                                  const FLOAT_TYPE dCurrMin = 1e22, Vector* from = NULL, Vector* to = NULL);

template<class Vector>
FLOAT_TYPE mindist_between_arcs(const Candi<Vector> &pair_of_arcs, const FLOAT_TYPE dCurrMin = 1e22,
                           Vector* from = NULL, Vector* to = NULL);

template<class Vector>
FLOAT_TYPE mindist_between_arcs(const Vector &q00,const Vector &q01,const Vector &t0,
                           const Vector &q10,const Vector &q11,const Vector &b1,
                           const FLOAT_TYPE dCurrMin, Vector* from, Vector* to);

template<class Vector>
int double_critical_test(const Vector &a0, const Vector &a1,
                         const Vector &t0a,const Vector &t1a,
                         const Vector &b0, const Vector &b1,
                         const Vector &t0b,const Vector &t1b,
                         FLOAT_TYPE dCurrentMin = 1e22);

template<class Vector>
int double_critical_test_v2(const Vector &a0,const Vector &a1,const Vector &a2,
                            const Vector &b0,const Vector &b1,const Vector &b2,
                            FLOAT_TYPE dCurrentMin = 1e22);

/*
   This comes from David Eberly www.geometrictools.com
   for computing linear segment/segment distances.
   we basically minimize the quadratic function
   Q(s,t) = as^2+2bst+ct^2+2ds+2et+f
*/

FLOAT_TYPE aux_edge(FLOAT_TYPE tmp, FLOAT_TYPE denum);
void aux_corner(FLOAT_TYPE s_in, FLOAT_TYPE t_in, FLOAT_TYPE Qs, FLOAT_TYPE Qt,
                 FLOAT_TYPE edge0, FLOAT_TYPE denum0, FLOAT_TYPE edge1,
                 FLOAT_TYPE denum1, FLOAT_TYPE &s, FLOAT_TYPE &t);

template<class Vector>
FLOAT_TYPE min_seg_dist(const Vector &B0, const Vector &B0p, const Vector &B1,
                   const Vector &B1p,FLOAT_TYPE &s,FLOAT_TYPE &t);

#include "../lib/algo_helpers.cpp"

#endif // __ALGO_HELPERS__
