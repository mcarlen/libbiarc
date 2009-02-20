/*!
  \file angle_vs_arclength.cpp
  \ingroup ToolsGroup
  \brief Prints the arclength and for that arclength the angle between
         the normal vector and the "2" contact struts.

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
  if ( fabsf((p0-p1).norm()-d)<d*tol ) return 1;
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
  float r, slen;
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
        cout << slen << " " << myangle(nor,CDir,tan) << endl;
        ++ContactN;
      }
      it->getBezierArc1(b0,b1,b2);
      r = it->radius1();
      if (rhopt(p,b0,b1,b2,r,v) && th_cond(thick,p,v,tol)) {
        CDir = (v-p); CDir.normalize();
        cout << slen << " " << myangle(nor,CDir,tan) << endl;
        ++ContactN;
      }
    }
  }
  cerr << ContactN << " Contacts." << endl;
  return 0;
}

#endif // DOXYGEN_SHOULD_SKIP_THIS
