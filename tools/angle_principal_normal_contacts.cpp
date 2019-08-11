/*!
  \file angle_principal_normal_contacts.cpp
  \ingroup ToolsGroup
  \brief Prints angle between contact chords and principal normal
         to stdout.

*/

#ifndef DOXYGEN_SHOULD_SKIP_THIS

// #define FOURIER_REPRESENTATION

#include "../include/Curve.h"
#include "../include/algo_helpers.h"
#ifdef FOURIER_REPRESENTATION
  #include "../fourierknots/fourier_syn.h"
#endif

#define b3 vector<Biarc<Vector3> >::iterator

int th_cond(float d,Vector3& p0, Vector3 &p1, float tol) {
  if ( std::abs((p0-p1).norm()-d)<d*tol ) return 1;
  return 0;
}

#ifdef FOURIER_REPRESENTATION
void curve2fourier(const Curve<Vector3>& c, FourierKnot* fk) {

  Curve<Vector3> curve(c);
  int Samples = 1000;
  int N = curve.nodes();
  Vector3 vsin, vcos;

  curve.link();
  curve.make_default();
  curve.resample(Samples);
  curve.make_default();

  vector<Biarc<Vector3> >::iterator it;
  float dx = 2*M_PI/(float)Samples, x;
  for (int n=1;n<=N;++n) {
    vsin.zero(); vcos.zero(); x = 0.0;
    for (it=curve.begin();it!=curve.end();++it) {
      vsin += it->getPoint()*sin(n*x);
      vcos += it->getPoint()*cos(n*x);
      x += dx;
    }
    fk->csin.push_back(vsin);
    fk->ccos.push_back(vcos);
  }
}
#endif

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

/*
  Convert angle from acos() to interval [0,1]
*/
float OPI2 = 1./M_PI;
float myangle(Vector3 &v1, Vector3 &v2, Vector3 &tan) {
  float angle = v1.dot(v2), direc = 1.;
  if ((v1.cross(v2)).dot(tan)<0)
    direc = -1.;
  // Transform to interval [-pi,pi]->[-1,1]
  float ret = direc*acos(angle)*OPI2;
  // Return transformed [-1,1]->[0,1]
  return (ret+1.)*.5;
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

  float thick = c.thickness();
  float L = c.length();
  int N = c.nodes();

  // Use fourier representation for normals
#ifdef FOURIER_REPRESENTATION
  FourierKnot fk;
  curve2fourier(c,&fk);
#endif
  cerr <<  "Nodes " << c.nodes() << endl;

  /*
     For every point p on the curve, compute
     the contacts. Then compute the angle
     between the contact line and the Normal
     at the point p
  */
#define b3 vector<Biarc<Vector3> >::iterator
  Vector3 b0,b1,b2,p,v,CDir,tan,nor;
  float r, slen, sigma;
  int ContactN = 0;
  for (int i=0;i<N;++i) {
    slen = arclength(c,i)/L;
    p = c[i%c.nodes()].getPoint();
    // XXX can't use point coming from Fourier, cause scaling is off!!!
    //     however scaling doesn't matter for tangents and normals!
    //     but they seem wrong too ... shift?
    // p   = fk(slen);
#ifdef FOURIER_REPRESENTATION
    tan = fk.prime(slen);      tan.normalize();
    nor = fk.primeprime(slen); nor.normalize();
#else
    tan = c[i%c.nodes()].getTangent(); tan.normalize();
    nor = c.normalVector(i%c.nodes()); nor.normalize();
#endif
    for (b3 it=c.begin();it!=c.end();++it) {
      if (it==(c.begin()+i)) continue;
      it->getBezierArc0(b0,b1,b2);
      r = it->radius0();
      if (rhopt(p,b0,b1,b2,r,v) && th_cond(thick,p,v,tol)) {
        CDir = (v-p); CDir.normalize();
        sigma = find_s(c,v);
        cout << slen << " " << myangle(nor,CDir,tan) << " " << sigma/L << endl;
        ++ContactN;
      }
      it->getBezierArc1(b0,b1,b2);
      r = it->radius1();
      if (rhopt(p,b0,b1,b2,r,v) && th_cond(thick,p,v,tol)) {
        CDir = (v-p); CDir.normalize();
        sigma = find_s(c,v);
        cout << slen << " " << myangle(nor,CDir,tan) << " " << sigma/L << endl;
        ++ContactN;
      }
    }
  }
  cerr << ContactN << " Contacts." << endl;
  return 0;
}

#endif // DOXYGEN_SHOULD_SKIP_THIS
