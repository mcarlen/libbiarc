#include "../include/algo_helpers.h"
#include <stdlib.h>
#include "../include/utils/timer.h"

#ifndef __ALGO_SRC__
#define __ALGO_SRC__

#define dump_candi(c) {for (candi_it i=(c).begin();i!=(c).end();i++) \
                              cerr << i->a.b0 << ", " \
                                   << i->a.b1 << ", " \
                                   << i->a.b2 << ", " \
                                   << i->b.b0 << ", " \
                                   << i->b.b1 << ", " \
                                   << i->b.b2 << ", (" \
                                   << i->d << ")\n"; }\



template<class Vector>
ArcInfo<Vector>::ArcInfo(const Vector &a0,const Vector &a1,const Vector &a2)
  : b0(a0),b1(a1),b2(a2)
{
  Vector b1b0 = b1-b0, b2b0 = b2-b0;
  b1b0.normalize(); b2b0.normalize();
  omega = b1b0.dot(b2b0);
  m     = (b0+2.*omega*b1+b2)/(2.+2.*omega);
  err   = (m-.5*(b0+b2)).norm();
  ferr  = 2.+sqrt(2.+2.*omega);
}

// // Second constructor for iteration part
template<class Vector>
ArcInfo<Vector>::ArcInfo(const Vector &a0,const Vector &a1,const Vector &a2,
                 const float &Err, const float &FErr)
  : b0(a0),b1(a1),b2(a2),err(Err),ferr(FErr)
{
  Vector b1b0 = b1-b0, b2b0 = b2-b0;
  b1b0.normalize(); b2b0.normalize();
  omega = b1b0.dot(b2b0);
  m     = (b0+2.*omega*b1+b2)/(2.+2.*omega);
}

template<class Vector>
Candi<Vector>::Candi(const Vector &a0,const Vector &a1,const Vector &a2,
             const Vector &b0,const Vector &b1,const Vector &b2)
  : a(a0,a1,a2), b(b0,b1,b2)
{
  static float dum1,dum2;
  d = min_seg_dist(a0,a2,b0,b2,dum1,dum2);
}

template<class Vector>
Candi<Vector>::Candi(const Vector &a0,const Vector &a1,const Vector &a2,
             const float &a_err,const float &a_ferr,
             const Vector &b0,const Vector &b1,const Vector &b2,
             const float &b_err,const float &b_ferr)
  : a(a0,a1,a2,a_err,a_ferr), b(b0,b1,b2,b_err,b_ferr)
{
  static float dum1,dum2;
  d = min_seg_dist(a0,a2,b0,b2,dum1,dum2);
}

/*!
   Given the Bezier arc b0,b1,b2 and radius r, this returns
   the binormal b and the center c of the circle through b0,b1,b2
*/
template<class Vector>
void get_it(Vector b0,Vector b1,Vector b2,float r,Vector &b, Vector &c) {
  b = (b0-b1).cross(b2-b1); b.normalize();
  Vector t0=(b1-b0); t0.normalize();
  c = (b0 + t0.cross(b)*r);
}

// jana page 100
template<class Vector>
int rhopt(Vector p, Vector b0,Vector b1,Vector b2,float r,Vector &v) {
  Vector b,c,aux;
  float val0 = (b1-b0).dot(b0-p),val1 = (b2-b1).dot(b2-p);
  if (val0<0 && val1 < 0) return 0;//cerr << "No min inside arc,min on endpoint\n";
  else if (val0<=0.0 && val1>=0.0) {
    get_it(b0,b1,b2,r,b,c);
    aux = p-c-b*b.dot(p-c);
    aux.normalize();
    v = c+aux*r;
    return 1;
  }
  else if (val0>=0.0 && val1<=0.0) {
    if (((b0-b1).cross(b2-b1)).dot(p-b1)==0.0) {
      get_it(b0,b1,b2,r,b,c);
      if (r<(p-c).norm()) {
        aux = p-c-b*b.dot(p-c);
        aux.normalize();
        v = c-aux*r;
        return 1;
      }
      else return 0;
    }
    else {
      get_it(b0,b1,b2,r,b,c);
      Vector centersph=c+b*(b.dot(p-c));
      float circ_cos= (b0-centersph).dot(b)/(b0-centersph).norm();
      float point_cos=(p-centersph).dot(b)/(p-centersph).norm();
      if (point_cos < circ_cos) {
        aux = p-c-b*b.dot(p-c);
        aux.normalize();
        v = c-aux*r;
        return 1;
      }
      else return 0;
    }
  }
  else if (val0>0 && val1>0) return 0;// cerr << "No min inside arc\n";

  return 0; //(new Vector(0,0,0));
}

/*!
  Write all the candidate segment pairs to cerr
*/
template<class Vector>
void dump_candidates(vector<Candi<Vector > > *C,int iter) {
  for (candi_it i=C->begin();i!=C->end();++i)
    cerr << iter << " " << (*i) << endl;
}

/*!
 Iterate through curve c and find the smallest
 radius of curvature. Returns 2x that radius.
*/
template<class Vector>
float check_local_curvature(Curve<Vector>* c) {
  // First we check the local redii of the arcs
  float tmpf, min_diam = 2.*c->begin()->radius0();
  if (min_diam<0) min_diam = 1e99;
  for (biarc_it it=c->begin();it!=c->end()-(c->isClosed()?0:1);++it) {
    tmpf = 2.0*it->radius0();
    if (tmpf<0.) continue;
    if (tmpf < min_diam)
      if (tmpf > 0)
        min_diam = tmpf;
    tmpf = 2.0*it->radius1();
    if (tmpf<0.) continue;
    if (tmpf < min_diam)
      if (tmpf > 0)
        min_diam = tmpf;
  }
  return min_diam;
}

/*!
  Initial double critical test

  Filters non critical arc pairs from the curve c. The candidate arc paris are
  pushed to a Candi vector CritC.
*/
template<class Vector>
void initial_dbl_crit_filter(Curve<Vector>* c,vector<Candi<Vector> > &CritC,
                             float dCurrentMin) {

  CritC.clear();

  Vector a0,am,a1,b0,bm,b1,t0a,tma,t1a,t0b,tmb,t1b;
  // Temporary Bezier points
  Vector Ba0,Ba1,Ba2,Bb0,Bb1,Bb2;

  biarc_it stop1 = c->isClosed()?c->end()-1:c->end()-2;
  biarc_it stop2 = c->isClosed()?c->end():c->end()-1;
  for (biarc_it i=c->begin();i!=stop1;i++) {
    for (biarc_it j=i+1;j!=stop2;j++) {

      i->getBezierArc0(Ba0,Ba1,Ba2);
      j->getBezierArc0(Bb0,Bb1,Bb2);

      // Now double criticle all 4 possibilities
      // excluding next neighbors
      // arc a1 - b1
      if (double_critical_test_v2(Ba0,Ba1,Ba2,Bb0,Bb1,Bb2,dCurrentMin)) {
        CritC.push_back(Candi<Vector>(Ba0,Ba1,Ba2,Bb0,Bb1,Bb2
                        ));
      }

      j->getBezierArc1(Bb0,Bb1,Bb2);
      // arc a1 - b2
      if (!(i==c->begin() && j==(c->end()-1))) {
        if (double_critical_test_v2(Ba0,Ba1,Ba2,Bb0,Bb1,Bb2,dCurrentMin)) {
          CritC.push_back(Candi<Vector>(Ba0,Ba1,Ba2,Bb0,Bb1,Bb2
          ));
        }
      }

      i->getBezierArc1(Ba0,Ba1,Ba2);
      // arc a2 - b2
      if (double_critical_test_v2(Ba0,Ba1,Ba2,Bb0,Bb1,Bb2,dCurrentMin)) {
        CritC.push_back(Candi<Vector>(Ba0,Ba1,Ba2,Bb0,Bb1,Bb2
        ));
      }
      // arc a2 - b1
      if (j!=i+1) {
        j->getBezierArc0(Bb0,Bb1,Bb2);
        if (double_critical_test_v2(Ba0,Ba1,Ba2,Bb0,Bb1,Bb2,dCurrentMin)) {
          CritC.push_back(Candi<Vector>(Ba0,Ba1,Ba2,Bb0,Bb1,Bb2
          ));
        }
      }
    }
  }
}

/*!
  Double critical test (in subdivision loop)

  Filters non critical arc pair candidates from Candi vector C.
  The candidate arc paris passing the test are pushed to a Candi
  vector CritC. The reference to CritC is cleared at the beginning!
*/
template<class Vector>
void dbl_crit_filter(vector<Candi<Vector> > &C,vector<Candi<Vector> > &CritC, const float dCurrentMin) {

  // Double Critical Test
  CritC.clear();

  for (candi_it i=C.begin();i!=C.end();i++) {
  // for each candidate use the bezier points
  // in double_critical_test_v2 and
  // put the correct 3 Bezier points in the subdiv arc!!!
  // example if we need the left sub arc of a0,a1,a2,m
  // subarc Bezier points are given by a0,(a0+a1)/2,m !!!
    Vector a0,a1,a2,b0,b1,b2;
    // current bezier points for arc a and b
    a0 = i->a.b0;
    a1 = i->a.b1;
    a2 = i->a.b2;
    b0 = i->b.b0;
    b1 = i->b.b1;
    b2 = i->b.b2;

    // points for the new subarcs
    Vector a01 = (a0+a1)/2., a12 = (a1+a2)/2.;
    Vector b01 = (b0+b1)/2., b12 = (b1+b2)/2.;
    Vector am = (a01+a12)/2., bm = (b01+b12)/2.;

    // temporary Vectors for the new arc pairs
    Vector c0,c1,c2,d0,d1,d2;

    // arc a1 - b1
    c0 = a0; c1 = a01; c2 = am;
    d0 = b0; d1 = b01; d2 = bm;
    if (double_critical_test_v2(c0,c1,c2,d0,d1,d2,dCurrentMin))
       CritC.push_back(Candi<Vector>(c0,c1,c2,i->a.err/i->a.ferr,i->a.ferr,
                                     d0,d1,d2,i->b.err/i->b.ferr,i->b.ferr));

    // arc a1 - b2
    d0 = bm; d1 = b12; d2 = b2;
    if (double_critical_test_v2(c0,c1,c2,d0,d1,d2,dCurrentMin))
       CritC.push_back(Candi<Vector>(c0,c1,c2,i->a.err/i->a.ferr,i->a.ferr,
                                     d0,d1,d2,i->b.err/i->b.ferr,i->b.ferr));

    // arc a2 - b2
    c0 = am; c1 = a12; c2 = a2;
    if (double_critical_test_v2(c0,c1,c2,d0,d1,d2,dCurrentMin))
       CritC.push_back(Candi<Vector>(c0,c1,c2,i->a.err/i->a.ferr,i->a.ferr,
                                     d0,d1,d2,i->b.err/i->b.ferr,i->b.ferr));

    // arc a2 - b1
    d0 = b0; d1 = b01; d2 = bm;
    if (double_critical_test_v2(c0,c1,c2,d0,d1,d2,dCurrentMin))
       CritC.push_back(Candi<Vector>(c0,c1,c2,i->a.err/i->a.ferr,i->a.ferr,
                                     d0,d1,d2,i->b.err/i->b.ferr,i->b.ferr));

  }
//  cout << ITERATION << " : No crit : " << CritC.size() << endl;

}


/*!
  Given a Candi vector C,
  compute the initial thickness bounds lb and ub and the maximal error err.
  The relative error is given by rel_err = err/D_lb*2.  // cf. Jana Page 94
  md is the smallest localdiameter so far.
*/
template<class Vector>
void compute_thickness_bounds(vector<Candi<Vector> > &C,float md, float &lb, float &ub, float &err, candi_it &min_candi) {
  // Initial Thickness Bounds
  float D_lb = 1e8, D_ub = 1e8;
  float max_err = 0, tmperr, tmpf; // ,relerr;

  for (candi_it i=C.begin();i!=C.end();i++) {
    tmperr = i->a.err+i->b.err;
    if (tmperr>max_err) max_err = tmperr;

    tmpf = i->d - tmperr;
    if (tmpf<0) tmpf = 0.;

    if (tmpf<D_lb) {
      D_lb = tmpf;
      // Where is this minimum thickness?
      min_candi = i;
    }
    tmpf = i->d + tmperr;
    if (tmpf<D_ub) D_ub = tmpf;
  }
  lb = (D_lb<md?D_lb:md);
  ub = (D_ub<md?D_ub:md);
  err = max_err;

/*
  cout << "Bounds : \n";
  cout << "max_err/rel_err : " << max_err << "/" << (max_err/D_lb*2.) << endl;
  cout << "D_lb/D_ub       : " << lb << "/" << ub << endl;
*/
}


/*!
  The Candidates in C that pass the distance test are pushed
  to the vector Cfiltered. Cfiltered is cleared first!
*/
template<class Vector>
void distance_filter(vector<Candi<Vector> > &C,vector<Candi<Vector> > &Cfiltered, const float dCurrMin) {
  float d_b = dCurrMin, tmpf;
  if (Cfiltered.size()!=0)
    Cfiltered.clear();
  for (candi_it i=C.begin();i!=C.end();i++) {
    tmpf = i->a.err+i->b.err+i->d;
    if (tmpf<d_b) d_b = tmpf;
  }
  for (candi_it i=C.begin();i!=C.end();i++) {
    tmpf = i->d - i->a.err - i->b.err;
    if (tmpf<=d_b) {
      Cfiltered.push_back(*i);
    }
  }
}

/*!
 Compute thickness of a curve Curve<Vector>, from and to are the points on the curve
 where this thickness is achieved. hint_i and hint_j is used in Simulated annealing runs
 to give a biarc pair where the thickness was achieved in the last thickness computation.
*/
template<class Vector>
float compute_thickness(Curve<Vector> *c, Vector *from, Vector *to, const int hint_i, const int hint_j) {

//  double myt = start_time();

  float min_diam = check_local_curvature(c);
  vector<Candi<Vector> > tmp, candidates;
  float global_min = min_diam, curr_min;

//	cerr << "Local curvature min : " << global_min << endl;

  // If we got a hint, compute first these pairs to get a good initial guess for global_min
  // We might compute the same pair twice, but that's ok
  if (hint_i>=0 && hint_j>=0) {
    vector<Candi<Vector> > CritC;
    Vector a0,am,a1,b0,bm,b1,t0a,tma,t1a,t0b,tmb,t1b;
    // Temporary Bezier points
    Vector Ba0,Ba1,Ba2,Bb0,Bb1,Bb2;

//    cerr << "HINT : " << hint_i << " " << hint_j << " : ";
    typename vector< Biarc<Vector> >::iterator i = c->begin() + hint_i;
    typename vector< Biarc<Vector> >::iterator j = c->begin() + hint_j;

    i->getBezierArc0(Ba0,Ba1,Ba2); j->getBezierArc0(Bb0,Bb1,Bb2);

    // Now double criticle all 4 possibilities
    // excluding next neighbors
    // arc a1 - b1
    if (double_critical_test_v2(Ba0,Ba1,Ba2,Bb0,Bb1,Bb2,global_min))
      CritC.push_back(Candi<Vector>(Ba0,Ba1,Ba2,Bb0,Bb1,Bb2));

    j->getBezierArc1(Bb0,Bb1,Bb2);
    // arc a1 - b2
    if (!(i==c->begin() && j==(c->end()-1))) {
      if (double_critical_test_v2(Ba0,Ba1,Ba2,Bb0,Bb1,Bb2,global_min))
        CritC.push_back(Candi<Vector>(Ba0,Ba1,Ba2,Bb0,Bb1,Bb2));
    }

    i->getBezierArc1(Ba0,Ba1,Ba2);
    // arc a2 - b2
    if (double_critical_test_v2(Ba0,Ba1,Ba2,Bb0,Bb1,Bb2,global_min))
      CritC.push_back(Candi<Vector>(Ba0,Ba1,Ba2,Bb0,Bb1,Bb2));

    // arc a2 - b1
    if (j!=i+1) {
      j->getBezierArc0(Bb0,Bb1,Bb2);
      if (double_critical_test_v2(Ba0,Ba1,Ba2,Bb0,Bb1,Bb2,global_min))
        CritC.push_back(Candi<Vector>(Ba0,Ba1,Ba2,Bb0,Bb1,Bb2));
    }

    for (unsigned int i=0;i<CritC.size();++i) {
      curr_min = mindist_between_arcs(CritC[i],global_min);
      if (curr_min < global_min) global_min = curr_min;
    }
//    cerr << "Init guess " << global_min;
    if (from!=NULL) *from = Vector(); // (*from)[0] = 69;
    if (to!=NULL) *to = Vector(); // (*to)[0] = 96;
  }
//  else cerr << "NO HINT : ";

  // Initial double critical test
  initial_dbl_crit_filter(c, tmp, global_min);
  // This first distance filter removes "good" candidates!!!
  // DO NOT CHANGE IT!
  // distance_filter(tmp, candidates,global_min);
  candidates = tmp;

  Vector lFrom, lTo;
  for (unsigned int i=0;i<candidates.size();++i) {
    curr_min = mindist_between_arcs(candidates[i],global_min,&lFrom,&lTo);
    if (curr_min<global_min) {
      global_min = curr_min;
      if (from!=NULL) *from = lFrom;
      if (to!=NULL)   *to = lTo;
    }
  }
//  cerr << ", final D=" << global_min << endl;
//  cerr << stop_time(myt) << endl;
  return global_min;
}


/*!
 Compute thickness for CurveBundle. "from" and "to" are the points where it is achieved.
*/
template<class Vector>
float compute_thickness(CurveBundle<Vector> *cb, Vector *from, Vector *to) {

  float min_diam = check_local_curvature(&((*cb)[0]));
  for (int i=1;i<cb->curves();++i) {
    float min_diam_tmp = check_local_curvature(&((*cb)[i]));
    if (min_diam_tmp<min_diam)
      min_diam = min_diam_tmp;
  }

  vector<Candi<Vector> > tmp, candidates;
  float global_min = min_diam, curr_min;

  for (int ic=0;ic<cb->curves();++ic) {
  for (int ic2=ic;ic2<cb->curves();++ic2) {
    Curve<Vector> *c1 = &((*cb)[ic]);
    Curve<Vector> *c2 = &((*cb)[ic2]);
  Vector Ba0,Ba1,Ba2,Bb0,Bb1,Bb2;

  for (int ib=0;ib<c1->nodes()+(c1->isClosed()?-2:-3);++ib) {
    for (int jb=ib+1;jb<c2->nodes()+(c2->isClosed()?-1:-2);++jb) {


      biarc_it i = c1->begin()+ib;
      biarc_it j = c2->begin()+jb;

      i->getBezierArc0(Ba0,Ba1,Ba2);
      j->getBezierArc0(Bb0,Bb1,Bb2);
      candidates.push_back(Candi<Vector>(Ba0,Ba1,Ba2,Bb0,Bb1,Bb2));

      j->getBezierArc1(Bb0,Bb1,Bb2);
      candidates.push_back(Candi<Vector>(Ba0,Ba1,Ba2,Bb0,Bb1,Bb2));

      i->getBezierArc1(Ba0,Ba1,Ba2);
      candidates.push_back(Candi<Vector>(Ba0,Ba1,Ba2,Bb0,Bb1,Bb2));

      if (j!=i+1) {
        j->getBezierArc0(Bb0,Bb1,Bb2);
          candidates.push_back(Candi<Vector>(Ba0,Ba1,Ba2,Bb0,Bb1,Bb2));
      }
    }
  }
  }
  }

  dbl_crit_filter(candidates, tmp, global_min);
  // This first distance filter removes "good" candidates!!!
  // DO NOT CHANGE IT!
  // distance_filter(tmp, candidates,global_min);
  candidates = tmp;

  Vector lFrom, lTo;
  for (unsigned int i=0;i<candidates.size();++i) {
    curr_min = mindist_between_arcs(candidates[i],global_min,&lFrom,&lTo);
    if (curr_min<global_min) {
      global_min = curr_min;
      if (from!=NULL) *from = lFrom;
      if (to!=NULL)   *to = lTo;
    }
  }
  return global_min;
}




/*!
  Compute the closest distance between two arcs of circles
  given by their bezier control points. Returns as
  well the two points corresponding to the closest distance
  \a from and \a to.
*/
template<class Vector>
float mindist_between_bezier_arcs(const Vector &a0,const Vector &a1,const Vector &a2,
                                  const Vector &b0,const Vector &b1,const Vector &b2,
                                  const float dCurrMin,
                                  Vector* from,
                                  Vector* to) {
  Candi<Vector> arcs(a0,a1,a2,b0,b1,b2);
  return mindist_between_arcs(arcs,dCurrMin,from,to);
}

/*!
  Compute the closest distance between two arcs of circles
  given by the arcs endpoints q00 and q01, resp q10 and q11
  and the tangents at the first point (t0 at q00, resp t1 at q10).
  Returns as well the two points corresponding to the closest distance
  \a from and \a to.
*/
template<class Vector>
float mindist_between_arcs(const Vector &q00,const Vector &q01,const Vector &t0,
                           const Vector &q10,const Vector &q11,const Vector &t1,
                           const float dCurrMin, Vector* from, Vector* to) {
  Vector a0 = q00, a2 = q10;
  Vector b0 = q10, b2 = q11;

  // Compute bezier points a1 and b1
  Vector d = q01 - q00;
  float l = d.norm2()*.5/d.dot(t0);
  Vector a1 = q00 + t0*l;

  d = q11 - q10;
  l = d.norm2()*.5/d.dot(t1);
  Vector b1 = q10 + t1*l;

  Candi<Vector> arcs(a0,a1,a2,b0,b1,b2);
  return mindist_between_arcs(arcs,dCurrMin,from,to);

}


/*!
  Compute the closest distance between two arcs of circles
  given a candidate (Candi class). Returns as
  well the two points corresponding to the closest distance \a from
  and \a to.
*/
template<class Vector>
float mindist_between_arcs(const Candi<Vector> &pair_of_arcs, const float dCurrMin,
                   Vector* from, Vector* to) {

  const float rel_err_tol = 1e-12;
  float D_lb,D_ub;
  float rel_err = 1e99, max_err = 1e99;

  biarc_it current, var;
  vector<Candi<Vector> > CritC, DistC;

  DistC.push_back(pair_of_arcs);
  candi_it min_candi = DistC.begin();

//  int iter = 1;
  while(rel_err > rel_err_tol) {

    // Bisect Candidates
    dbl_crit_filter(DistC,CritC,dCurrMin);
    // cout << "iter " << iter++ << endl;
    // dump_candi(CritC);

    if (CritC.size()==0) {
    //  cout << "dble crit empty\n";
      return 1e22;
    }

    // Distance Test
    distance_filter(CritC,DistC,dCurrMin);

    // Thickness Bounds
    if (DistC.size()==0) {
    //  cout << "dist empty\n";
      return 1e22;
    }

    // Bounds
    compute_thickness_bounds(DistC,dCurrMin,D_lb,D_ub,max_err,min_candi);

    rel_err = max_err/D_lb*2.;
  }

  if (from!=NULL && to!=NULL) {
    float param_s,param_t;
    Vector a0 = min_candi->a.b0, a2 = min_candi->a.b2;
    Vector b0 = min_candi->b.b0, b2 = min_candi->b.b2;

    (void)min_seg_dist(a0,a2,b0,b2,param_s,param_t);

    *from = a0+(a2-a0)*param_s;
    *to   = b0+(b2-b0)*param_t;
  }
  CritC.clear(); DistC.clear();
  return D_lb;
}

/*!
  The double criticality test is a translation
  of Jana's Matlab code (listing in her Thesis)
*/
template<class Vector>
int double_critical_test(const Vector &a0, const Vector &a1,
                         const Vector &t0a,const Vector &t1a,
                         const Vector &b0, const Vector &b1,
                         const Vector &t0b,const Vector &t1b,
                         const float dCurrentMin) {

  Vector w = (a0+a1-b0-b1);
  float denum = w.norm();
  float val0 = (a0-a1).norm(), val1 = (b0-b1).norm();

  // Jana's Thesis eq 7.23
  // !! prevents thicknesscomputation on curves with a
  //    small number of nodes !!
  // not sure wheter this check is necessary in practice !!
  // assert(denum > val0+val1);

  // do balls intersect?
  if (denum<=(val0+val1)) {
    cout << "Balls intersect " << denum << " <= " << val0 << " + " << val1 << "\n";
    // exit(1);
    return 1;
  }

  // Ben has this and it works ;)
  // if ( dCurrentMin < 1e12 && (denum - val0 - val1)*.5 > dCurrentMin) return 0;

  // tolerance in percent we let slide
  float eps = 1e-6;

  float sina = (val0+val1)/denum*(1.+eps) ;
  w.normalize();

  if ((w.dot(t0a)<-sina) && (w.dot(t1a)<-sina)) return 0;
  if ((w.dot(t0a)>sina)  && (w.dot(t1a)>sina)) return 0;
  if ((w.dot(t0b)<-sina) && (w.dot(t1b)<-sina)) return 0;
  if ((w.dot(t0b)>sina)  && (w.dot(t1b)>sina)) return 0;

  return 1;
}

template<class Vector>
int double_critical_test_v2(
             const Vector &a0,const Vector &a1,const Vector &a2,
             const Vector &b0,const Vector &b1,const Vector &b2,
             const float dCurrentMin
                           ) {
  // Possible optimization : since the Bezier triangles are
  // equilateral, only 2 of the 4 norms need to be calculated!
  // smthing like : invnorma = 1./(a1-a0).norm(); (a1-a0)*invnorma ...
  Vector ta0 = a1-a0, ta2 = a2-a1;
  ta0.normalize(); ta2.normalize();

  Vector tb0 = b1-b0, tb2 = b2-b1;
  tb0.normalize(); tb2.normalize();

  return double_critical_test(a0,a2,
                              ta0,ta2,
                              b0,b2,
                              tb0,tb2,dCurrentMin);
}

// segment/segment distance computation

float aux_edge(float tmp, float denum) {
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

void aux_corner(float s_in, float t_in, float Qs, float Qt,
                 float edge0, float denum0, float edge1,
                 float denum1, float &s, float &t) {
  if (Qs>0.0) { t=t_in;s=aux_edge(edge0,denum0); }
  else { s=s_in;
    if (Qt>0.0) t=aux_edge(edge1,denum1);
    else t = t_in;
  }
}

template<class Vector>
float min_seg_dist(
            const Vector &B0, const Vector &B0p, const Vector &B1,
            const Vector &B1p,float &s,float &t
                  ) {
  const float tol = 1e-12;
  Vector D = B0-B1, M0 = B0p-B0, M1 = B1p-B1;
  float a = M0.dot(M0), b = -M0.dot(M1), c = M1.dot(M1);
  float d = M0.dot(D), e = -M1.dot(D); // , f = D.dot(D);
  float det = fabsf(a*c-b*b);
  float invDet, tmp;
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

#endif // __ALGO_SRC__
