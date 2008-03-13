// XXX Doc

#include "../include/algo_helpers.h"

#ifndef __ALGO_SRC__
#define __ALGO_SRC__

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
void dump_candidates(vector<Candi<Vector > > *C,int iter = 0) {
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
void initial_dbl_crit_filter(Curve<Vector>* c,vector<Candi<Vector> > &CritC) {

  Vector a0,am,a1,b0,bm,b1,t0a,tma,t1a,t0b,tmb,t1b;
  // Temporary Bezier points
  Vector Ba0,Ba1,Ba2,Bb0,Bb1,Bb2;

  for (biarc_it i=c->begin();i!=c->end()-(c->isClosed()?0:1);i++) {
    a0  = i->getPoint();    t0a = i->getTangent();
    am  = i->getMidPoint(); tma = i->getMidTangent();
   
    if (c->isClosed()) {
      if (i==c->end()-1) {
        a1  = c->begin()->getPoint(); t1a = c->begin()->getTangent();
      }
      else {
        a1  = (i+1)->getPoint(); t1a = (i+1)->getTangent();
      }
    }
    else {
      a1  = (i+1)->getPoint(); t1a = (i+1)->getTangent();
    }

    for (biarc_it j=c->begin();j!=i;j++) {
      b0  = j->getPoint();     t0b = j->getTangent();
      bm  = j->getMidPoint();  tmb = j->getMidTangent();
      b1  = (j+1)->getPoint(); t1b = (j+1)->getTangent();
 
      // Now double criticle all 4 possibilities
      // excluding next neighbors
      // arc a1 - b1
      if (double_critical_test(a0,am,t0a,tma,b0,bm,t0b,tmb)) {
        i->getBezierArc0(Ba0,Ba1,Ba2); j->getBezierArc0(Bb0,Bb1,Bb2);
        CritC.push_back(Candi<Vector>(Ba0,Ba1,Ba2,Bb0,Bb1,Bb2));
      }

      // arc a1 - b2
      // do not compare neighboring arcs
      if ((i-1)!=j) {
        if (double_critical_test(a0,am,t0a,tma,bm,b1,tmb,t1b)) {
          i->getBezierArc0(Ba0,Ba1,Ba2); j->getBezierArc1(Bb0,Bb1,Bb2);
          CritC.push_back(Candi<Vector>(Ba0,Ba1,Ba2,Bb0,Bb1,Bb2));
        }
      }
      // arc a2 - b1
      // if the biarcs are neighbours we do not compare these
      // neighboring arcs!
      if (i!=c->end()-1 && j==c->begin()){
        if (double_critical_test(am,a1,tma,t1a,b0,bm,t0b,tmb)) {
          i->getBezierArc1(Ba0,Ba1,Ba2); j->getBezierArc0(Bb0,Bb1,Bb2);
          CritC.push_back(Candi<Vector>(Ba0,Ba1,Ba2,Bb0,Bb1,Bb2));
        }
      }
      // arc a2 - b2
      if (double_critical_test(am,a1,tma,t1a,bm,b1,tmb,t1b)) {
        i->getBezierArc1(Ba0,Ba1,Ba2); j->getBezierArc1(Bb0,Bb1,Bb2);
        CritC.push_back(Candi<Vector>(Ba0,Ba1,Ba2,Bb0,Bb1,Bb2));
      }
    }
  }

  //cout << "Criticality candidates : " << CritC.size() <<  endl;
}

/*!
  Double critical test (in subdivision loop)

  Filters non critical arc pair candidates from Candi vector C.
  The candidate arc paris passing the test are pushed to a Candi
  vector CritC. The reference to CritC is cleared at the beginning!
*/
template<class Vector>
void dbl_crit_filter(vector<Candi<Vector> > &C,vector<Candi<Vector> > &CritC) {

  // Double Critical Test
  CritC.clear();

  for (candi_it i=C.begin();i!=C.end();i++) {
  // for each candidate use the bezier points
  // in double_critical_test_v2 and
  // put the correct 3 Bezier points in the subdiv arc!!!
  // example if we need the left sub arc of a0,a1,a2,m
  // subarc Bezier points are given by a0,(a0+a1)/2,m !!!
    // arc a1 - b1
    if (double_critical_test_v2(i->a.b0,.5*(i->a.b0+i->a.b1),i->a.m,
                                i->b.b0,.5*(i->b.b0+i->b.b1),i->b.m)) {
       CritC.push_back(Candi<Vector>(
         i->a.b0,.5*(i->a.b0+i->a.b1),i->a.m,i->a.err/i->a.ferr,i->a.ferr,
         i->b.b0,.5*(i->b.b0+i->b.b1),i->b.m,i->b.err/i->b.ferr,i->b.ferr
                                    ));
    }
    // arc a1 - b2
    if (double_critical_test_v2(i->a.b0,.5*(i->a.b0+i->a.b1),i->a.m,
                                i->b.m,.5*(i->b.b1+i->b.b2),i->b.b2)) {
      CritC.push_back(Candi<Vector>(
        i->a.b0,.5*(i->a.b0+i->a.b1),i->a.m,i->a.err/i->a.ferr,i->a.ferr,
        i->b.m,.5*(i->b.b1+i->b.b2),i->b.b2,i->b.err/i->b.ferr,i->b.ferr
                                   ));
    }
    // arc a2 - b1
    if (double_critical_test_v2(i->a.m,.5*(i->a.b1+i->a.b2),i->a.b2,
                                i->b.b0,.5*(i->b.b0+i->b.b1),i->b.m)) {
      CritC.push_back(Candi<Vector>(
        i->a.m,.5*(i->a.b1+i->a.b2),i->a.b2,i->a.err/i->a.ferr,i->a.ferr,
        i->b.b0,.5*(i->b.b0+i->b.b1),i->b.m,i->b.err/i->b.ferr,i->b.ferr
                                   ));
    }
    // arc a2 - b2
    if (double_critical_test_v2(i->a.m,.5*(i->a.b1+i->a.b2),i->a.b2,
                                i->b.m,.5*(i->b.b1+i->b.b2),i->b.b2)) {
      CritC.push_back(Candi<Vector>(
        i->a.m,.5*(i->a.b1+i->a.b2),i->a.b2,i->a.err/i->a.ferr,i->a.ferr,
        i->b.m,.5*(i->b.b1+i->b.b2),i->b.b2,i->b.err/i->b.ferr,i->b.ferr
                                   ));
    }
  }
//    cout << ITERATION << " : No crit : " << CritC.size() << endl;

}


/*!
  Given a Candi vector C,
  compute the initial thickness bounds lb and ub and the maximal error err.
  The relative error is given by rel_err = err/D_lb*2.  // cf. Jana Page 94
  md is the smallest localdiameter so far.
*/
template<class Vector>
void compute_thickness_bounds(vector<Candi<Vector> > &C,float md, float &lb, float &ub, float &err) {
  // Initial Thickness Bounds
  float D_lb = 1e8, D_ub = 1e8;
  float max_err = 0, tmperr, tmpf; // ,relerr; 

  for (candi_it i=C.begin();i!=C.end();i++) {
    tmperr = i->a.err+i->b.err;
    if (tmperr>max_err) max_err = tmperr;

    tmpf = i->d - tmperr;
    if (tmpf<0) tmpf = 0.;

    if (tmpf<D_lb) D_lb = tmpf;
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
void distance_filter(vector<Candi<Vector> > &C,vector<Candi<Vector> > &Cfiltered) {
  //cout << "Distance Test\n";
  float d_b = 1e8, tmpf;
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
  //cout << "Distance Candidates : " << Cfiltered.size() << endl;
}

template<class Vector>
float compute_thickness(Curve<Vector> *c, Vector *from = NULL, Vector *to = NULL) {
  const float rel_err_tol = 1e-12;

  biarc_it current, var;
  // Vector *tmp, *tmpmin; static int cbiarc = 0;
  // float tmpf;
  float min_diam = 1e99; Vector thick_1, thick_2;
  min_diam = check_local_curvature(c);

  // Double critical candidates
  // Distance check passed candidates
  vector<Candi<Vector> > CritC, DistC;

  // Initial double critical test
  initial_dbl_crit_filter(c,CritC);

  // Initial Distance Test
  distance_filter(CritC,DistC);

  // Initial Thickness Bounds
  float D_lb = 1e99, D_ub = 1e99;
  float max_err = 0, rel_err;
  compute_thickness_bounds(DistC,min_diam,D_lb,D_ub,max_err);
  rel_err = max_err/D_lb*2.;

  ITERATION = 0;

#ifdef ITERATE

  // Bisection loop while relative error larger than our given
  // tolerance and while D_lb smaller than the minimal 2*radius
  // if (min_diam <= D_lb) cout << "Curvature active!\n";
  while(rel_err > rel_err_tol && min_diam > D_lb) {
    ++ITERATION;

    // Bisect Candidates
    dbl_crit_filter(DistC,CritC);   

    if (CritC.size()==0) {
      //  cout << "CritIter : CritC is empty (curvature active?)\n";
      D_lb = D_ub = min_diam;
      break;
    }

    // Bounds
    compute_thickness_bounds(DistC,min_diam,D_lb,D_ub,max_err);

    // Distance Test
    distance_filter(CritC,DistC);

    // Thickness Bounds
    if (DistC.size()==0) {
      // cout << "BoundsIter : DistC is empty (curvature active?)\n";
      D_lb = D_ub = min_diam;
      break;
    }

    // Bounds
    compute_thickness_bounds(DistC,min_diam,D_lb,D_ub,max_err);

    rel_err = max_err/D_lb*2.;
  }

#endif // ITERATE

  //cout << "Number of iterations : " << ITERATION << endl;
  //cout << "Thick upper bound    : " << D_ub << endl;
  //cout << "Thick lower bound    : " << D_lb << endl;
//  if (from!=NULL && to!=NULL) {
//    *from = thick_1; *to = thick_2;
  CritC.clear(); DistC.clear();
  return D_lb;
}

/*!
 9aThe double criticality test is a translation
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
int double_critical_test_v2(
             const Vector &a0,const Vector &a1,const Vector &a2,
             const Vector &b0,const Vector &b1,const Vector &b2
                           ) {
  // XXX :
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
                              tb0,tb2);
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
