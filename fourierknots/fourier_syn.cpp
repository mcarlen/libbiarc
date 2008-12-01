#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include <iostream>
#include <fstream>
#include <vector>

#include <Vector3.h>
#include <Matrix3.h>
#include <Curve.h>
#include <algo_helpers.h>

//using namespace std;
//from hglib.vector import *
//from math import pi, sin, cos

typedef Vector3 Coeff;
typedef vector<Coeff> Coeffs;
typedef unsigned int uint;

class FourierKnot {
public:
  Coeff c0;
  Coeffs csin, ccos;
  
  friend istream& operator>>(istream &in, FourierKnot &fk);
  friend ostream& operator<<(ostream &out, const FourierKnot &fk);

  void clear() { csin.clear(); ccos.clear(); c0.setValues(0,0,0); }

  FourierKnot() {}
  ~FourierKnot() { clear(); }

  // We expect 2*n + 1 coefficient!
  FourierKnot(const char* file) {
    ifstream in(file);
    if (in.good()) in >> *this;
    else {
      cerr << "FourierKnot : Could not read " << file << endl;
      exit(2);
    }
  }

  FourierKnot(Coeffs lsin, Coeffs lcos) {
    assert(lsin.size()==lcos.size());
    csin = lsin; ccos = lcos;
  }

  FourierKnot(Coeff constant, Coeffs lsin, Coeffs lcos) {
    FourierKnot(lsin,lcos);
    c0 = constant;
  }

  void set(Coeff c, Coeffs lsin, Coeffs lcos) {
    c0 = c; csin = lsin; ccos = lcos;
  }
  void setConst(Coeff constant) { c0 = constant; }
  void setSin(Coeffs lsin) { csin = lsin; }
  void setCos(Coeffs lcos) { ccos = lcos; }

  // point at curve(s), s in (0,1)
  Vector3 operator()(float t) {
    float f;
    // Fourier constant term
    Vector3 r = this->c0/2.;
    // XXX optimise/cache this
    for (uint i=0;i<csin.size();++i) {
      f = (float)(i+1)*(2.*M_PI);
      r += cos(f*t)*ccos[i];
      r += sin(f*t)*csin[i];
    }
    return r;
  }

  // tangent at curve(t)
  Vector3 prime(float t) {
    Vector3 r; float f;
    // XXX optimise/cache this
    for (uint i=0;i<csin.size();++i) {
      f = (float)(i+1)*(2.*M_PI);
      r += (-f*sin(f*t))*ccos[i];
      r += ( f*cos(f*t))*csin[i];
    }
    return r;
  }

  // sample our fourier knot and put that in initialized
  // pointer to Curve curve
  void toCurve(const int sampling, Curve<Vector3> *curve) {
    float isampling = 1./(float)sampling, s;
    for (int i=0;i<sampling;++i) {
      s = (float)i*isampling;
      // Tangent gets normalized in biarc constructor
      curve->append((*this)(s),this->prime(s));
    }
  }

  // Rotate coefficients around axis v by angle alpha.
  // This is equal to rotating the object in Euclidean space
  // XXX HG has this in the neg trigo direction!!!
  void rotate(Vector3 v,float alpha) {
    // This rotation matrix has a trigo pos orientation!
    Matrix3 D; D.rotAround(v,alpha);
    c0 = D*c0;
    for (uint i=0;i<csin.size();++i) {
      csin[i] = D*csin[i];
      ccos[i] = D*ccos[i];
    }
  }

  // Flips the paramtrization orientation of the curve
  void flip_dir(float sh = 0) {
    if (sh!=0)
      this->shift(sh);
    for (Coeffs::iterator i=csin.begin();i!=csin.end();++i)
      *i = -(*i);
    if (sh!=0)
      this->shift(-sh);
  }

  // Shift parameter space of the coefficients
  void shift(float sh) {
    Coeffs tmpsin(csin), tmpcos(ccos);
    float f,vcos,vsin;
    for (uint i=0;i<csin.size();++i) {
      f = (float)(i+1)*(2.0*M_PI);
      vcos = cos(f*sh); vsin = sin(f*sh);
      csin[i] = vcos*tmpsin[i] + vsin*tmpcos[i];
      ccos[i] = vcos*tmpcos[i] - vsin*tmpsin[i];
    }
  }

};

inline ostream& operator<<(ostream &out, const FourierKnot &fk) {
  int oldp = out.precision(16);
  out << fk.c0 << endl;
  assert(fk.ccos.size()==fk.csin.size());
  for (uint i=0;i<fk.ccos.size();++i)
    out << fk.csin[i] << " " << fk.ccos[i] << endl;
  out.precision(oldp);
  return out;
}

// Read in a fourier coeff file
// structure :
// a00 a01 a02             | (constant)
// a10 a11 a12 b10 b11 b12 | (cos sin)
// ...
inline istream& operator>>(istream &in, FourierKnot &fk) {
  Coeff c,c0,c1; Coeffs csin, ccos;
  in >> c;
  while ((in >> c0 >> c1)) {
    ccos.push_back(c0); csin.push_back(c1);
  }
  if (in.fail() && !in.eof()) {
    cerr << "istream (FourierKnot) : Bad input!\n";
    exit(1);
  };
  fk.set(c,csin,ccos);
  return in;
}

#define TEST
#ifdef TEST
int main(int argc, char** argv) {
  
  FourierKnot fk;
  Curve<Vector3> knot, orig;

  fk.c0 = Vector3(0,1,0);
  fk.csin.push_back(Vector3(1,0,0));
  fk.ccos.push_back(Vector3(0,.5,0));

  fk.toCurve(10,&orig);

//  fk.rotate(Vector3(0,0,-1),M_PI/4.);
//  fk.shift(M_PI/2.);
  fk.flip_dir();

  fk.toCurve(10, &knot);
  knot.header("Fourier knot test","libbiarc","H. Gerlach","");
  orig.header("Fourier knot test","libbiarc","H. Gerlach","");
  orig.writePKF("orig.pkf");
  knot.writePKF("test.pkf");
  orig.writePKF("orig.pkf");

//  cout << "D=" << knot.thickness() << endl;

  return 0;
}
#else
int main(int argc, char** argv) {

  if (argc!=2) { cout << "Usage : " << argv[0] << " <coeff_file>\n"; exit(0); }

  FourierKnot fk(argv[1]);
  Curve<Vector3> knot;
  fk.toCurve(200,&knot);
  knot.header("fourier knot","coeff2pkf","H. Gerlach","");
  knot.writePKF("test.pkf");

  return 0;
}
#endif
