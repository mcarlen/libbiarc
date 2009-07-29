/*!
  \file sigma_and_tau_contacts.cpp
  \ingroup ToolsGroup
  \brief Extract contact functions sigma(s) and tau(s)
*/

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include "../include/Curve.h"
#include "../include/algo_helpers.h"

#define b3 vector<Biarc<Vector3> >::iterator

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

struct contact {
  Vector3 p0,p1,p2;
};
typedef contact contact;

int main(int argc, char **argv) {

  if (argc!=3) {
    cout << "Usage : " << argv[0]
         << " <pkf> <Contacts>\n";
    return 1;
  }

  // Init Curve
  Curve<Vector3> c(argv[1]);
  c.link();
  c.make_default();

  float L = c.length();

  contact con;
  float s, sigma;
  ifstream in(argv[2]);
  while (in >> con.p0 >> con.p1 >> con.p2) {
    s     = find_s(c,con.p0);
    sigma = find_s(c,con.p2); 
    cout << s/L << " " << sigma/L << " 0\n";
  }
  return 0;
}

#endif // DOXYGEN_SHOULD_SKIP_THIS
