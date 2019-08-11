/*!
  \file sigma_and_tau_contacts.cpp
  \ingroup ToolsGroup
  \brief Extract contact functions sigma(s) and tau(s)
*/

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include "../include/Curve.h"
#include "../include/algo_helpers.h"

#define b3 vector<Biarc<Vector3> >::iterator

int th_cond(float d,Vector3& p0, Vector3 &p1, float tol) {
 if ( std::abs((p0-p1).norm()-d)<d*tol ) return 1;
  return 0;
}

float arclength(Curve<Vector3> &c, int n) {
  float s = 0;
  for (int i=0;i<n;++i)
    s += c[i].biarclength();
  return s;
}

float find_s(Curve<Vector3> &c, Vector3 &p) {
  // XXX possible optimization : not necessary to check against all the points ...
  // Find closest point to p on curve c
  float d = 1e22, dl; b3 close;
  for (b3 b=c.begin();b!=c.end();++b) {
    dl = (b->getPoint()-p).norm2();
    if (dl < d) {
      d = dl;
      close = b;
    }
  }

  b3 neighbor;
  if ( (close->getPoint()-close->getNext().getPoint()).norm2() < (close->getPoint()-close->getPrevious().getPoint()).norm2() ) {
    if (close == c.begin())
      neighbor = c.end()-1;
    else
      neighbor = close-1;
  }
  else {
    if (close == c.end()-1)
      neighbor = c.begin();
    else
      neighbor = close+1;
  }

  // recover s for our point p
  float s0 = arclength(c,close->id());
  float s1 = arclength(c,neighbor->id());

  if (s0>s1) {
    float tmp = s0;
    s0 = s1; s1 = tmp;
  }

  d = 1e22;
  float c_s0 = s0, c_s1 = s1, s_best = s0;
  Vector3 ptrial;

  // Make this bisection better!!!
  for (int i=0;i<100;++i) {
    ptrial = c.pointAt(c_s0 + (c_s1-c_s0)*(float)i/99.);
    dl = (ptrial - p).norm2();
    if (dl<d) {
      d = dl;
      s_best = c_s0 + (c_s1-c_s0)*(float)i/99.;
    }
  }

  return s_best;

}

int main(int argc, char **argv) {

  if (argc!=3) {
    cout << "Usage : " << argv[0]
         << " <pkf> <Contact Tol>\n";
    return 1;
  }

  float tol = atof(argv[2]);

  // Init Curve
  Curve<Vector3> c(argv[1]);
  c.link();
  c.make_default();
  c.normalize();
  c.make_default();

  float thick = c.thickness();
  float L = c.length();
  int N = c.nodes();

  cerr <<  "Nodes " << c.nodes() << endl;

  /*
     For every point p on the curve, compute
     the contacts. Then compute the angle
     between the contact line and the Normal
     at the point p
  */

  Vector3 b0,b1,b2,p,v;
  float r, slen;

  Vector3 close0, close1, close_tmp;
  float s_close0 = 0, s_close1 = 0, s_tmp;
  float d0 = 1e22, d1 = 1e22, d_tmp;

  int ContactN = 0;
  for (int i=0;i<N;++i) {
    slen = arclength(c,i)/L;
    p = c[i%c.nodes()].getPoint();

    ContactN = 0;
    // get 2 best contacts
    d0 = 1e22; d1 = 1e22;
    // init other stuff so that we can track if we were not able to find 2 contacts!
    for (b3 it=c.begin();it!=c.end();++it) {
      if (it==(c.begin()+i)) continue;

      it->getBezierArc0(b0,b1,b2);
      r = it->radius0();
      if (rhopt(p,b0,b1,b2,r,v) && th_cond(thick,p,v,tol)) {
        d_tmp = (p-v).norm2();
        if (d_tmp<d0) {
          // d0 -> d1
          d1 = d0; s_close1 = s_close0; close1 = close0;
          // new d0
          d0 = d_tmp; s_close0 = find_s(c,v); close0 = v;
        }
        else if (d_tmp<d1) {
          // new d1
          d1 = d_tmp; s_close1 = find_s(c,v); close1 = v;
        }
        ++ContactN;
      }

      it->getBezierArc1(b0,b1,b2);
      r = it->radius1();
      if (rhopt(p,b0,b1,b2,r,v) && th_cond(thick,p,v,tol)) {
        d_tmp = (p-v).norm2();
        if (d_tmp<d0) {
          // d0 -> d1
          d1 = d0; s_close1 = s_close0; close1 = close0;
          // new d0
          d0 = d_tmp; s_close0 = find_s(c,v); close0 = v;
        }
        else if (d_tmp<d1) {
          // new d1
          d1 = d_tmp; s_close1 = find_s(c,v); close1 = v;
        }
        ++ContactN;
      }

    }
    // assert(ContactN>1);
    // write s vs "closer along arc"
    float ss0 = s_close0/L, ss1 = s_close1/L;
    if (ss0<slen) ss0 += 1;
    if (ss1<slen) ss1 += 1;

    if (ss0 > ss1) {
      s_tmp = s_close0;
      s_close0 = s_close1; s_close1 = s_tmp;
    }
    // In order to check the angle condition we need : angle(strut1, normal) angle(strut2, normal) curvature_kappa
    cout << slen << " " << s_close0/L << " " << s_close1/L << endl;
//    cout << slen << " " << ss0/L << " " << ss1/L << endl;

  }
  cerr << ContactN << " Contacts." << endl;
  return 0;
}

#endif // DOXYGEN_SHOULD_SKIP_THIS
