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
  float err,ferr,omega;
  ArcInfo() {}
  ArcInfo(const Vector &a0,const Vector &a1,const Vector &a2);
  ArcInfo(const Vector &a0,const Vector &a1,const Vector &a2,
          const float &Err, const float &FErr);
};

// Possible candidate for thickness achievement
template<class Vector>
class Candi {
public:
  ArcInfo<Vector> a,b;
  float d;

  Candi() {};
  Candi(const Vector &a0,const Vector &a1,const Vector &a2,
        const Vector &b0,const Vector &b1,const Vector &b2);
  Candi(const Vector &a0,const Vector &a1,const Vector &a2,
        const float &a_err,const float &a_ferr,
        const Vector &b0,const Vector &b1,const Vector &b2,
        const float &b_err,const float &b_ferr);
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
void get_it(Vector b0,Vector b1,Vector b2,float r,Vector &b, Vector &c);

template<class Vector>
int rhopt(Vector p, Vector b0,Vector b1,Vector b2,float r,Vector &v);

template<class Vector>
void dump_candidates(vector<Candi<Vector > > *C,int iter = 0);

template<class Vector>
float check_local_curvature(Curve<Vector>* c);

template<class Vector>
void initial_dbl_crit_filter(Curve<Vector>* c, vector<Candi<Vector> > &CritC,
                             float dCurrentMin);

template<class Vector>
void dbl_crit_filter(vector<Candi<Vector> > &C,vector<Candi<Vector> > &CritC, float dCurrentMin);

template<class Vector>
void compute_thickness_bounds(vector<Candi<Vector> > &C,float md, float &lb, float &ub, float &err, candi_it &min_candi);

template<class Vector>
void distance_filter(vector<Candi<Vector> > &C,vector<Candi<Vector> > &Cfiltered, const float dCurrMin = 1e22);

template<class Vector>
float compute_thickness(Curve<Vector> *c, Vector *from = NULL, Vector *to = NULL, const int hint_i = -1, const int hint_j = -1);

template<class Vector>
float compute_thickness(CurveBundle<Vector> *cb, Vector *from = NULL, Vector *to = NULL);

template<class Vector>
float mindist_between_bezier_arcs(const Vector &a0,const Vector &a1,const Vector &a2,
                                  const Vector &b0,const Vector &b1,const Vector &b2,
                                  const float dCurrMin = 1e22, Vector* from = NULL, Vector* to = NULL);

template<class Vector>
float mindist_between_arcs(const Candi<Vector> &pair_of_arcs, const float dCurrMin = 1e22,
                           Vector* from = NULL, Vector* to = NULL);

template<class Vector>
float mindist_between_arcs(const Vector &q00,const Vector &q01,const Vector &t0,
                           const Vector &q10,const Vector &q11,const Vector &b1,
                           const float dCurrMin, Vector* from, Vector* to);

template<class Vector>
int double_critical_test(const Vector &a0, const Vector &a1,
                         const Vector &t0a,const Vector &t1a,
                         const Vector &b0, const Vector &b1,
                         const Vector &t0b,const Vector &t1b,
                         float dCurrentMin = 1e22);

template<class Vector>
int double_critical_test_v2(const Vector &a0,const Vector &a1,const Vector &a2,
                            const Vector &b0,const Vector &b1,const Vector &b2,
                            float dCurrentMin = 1e22);

/*
   This comes from David Eberly www.geometrictools.com
   for computing linear segment/segment distances.
   we basically minimize the quadratic function
   Q(s,t) = as^2+2bst+ct^2+2ds+2et+f
*/

float aux_edge(float tmp, float denum);
void aux_corner(float s_in, float t_in, float Qs, float Qt,
                 float edge0, float denum0, float edge1,
                 float denum1, float &s, float &t);

template<class Vector>
float min_seg_dist(const Vector &B0, const Vector &B0p, const Vector &B1,
                   const Vector &B1p,float &s,float &t);

#include "../lib/algo_helpers.cpp"

#endif // __ALGO_HELPERS__
