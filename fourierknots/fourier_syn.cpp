#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include <iostream>
#include <iomanip>
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

  void scale(float s) {
    c0 *= s;
    for (uint i=0;i<csin.size();++i) {
      csin[i] *= s;
      ccos[i] *= s;
    }
  }

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
  virtual Vector3 operator()(float t) {
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
  virtual Vector3 prime(float t) {
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
  virtual void toCurve(const int sampling, Curve<Vector3> *curve) {
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
  out << fk.c0 << endl;
  assert(fk.ccos.size()==fk.csin.size());
  for (uint i=0;i<fk.ccos.size();++i)
    out << fk.ccos[i] << " " << fk.csin[i] << endl;
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

// Observation (for trefoil) : cos_y = 0, cos_z = 0, sin_x = 0
// AND cos_x[i] = -sin_y[i], cos_x[i+1] = sin_y[i+1]
//
// 3/24 => 1/8 info needed
// cosx cosy cosz sinx siny sinz
// -A 0 0 0 A 0
//  B 0 0 0 B 0
//  0 0 0 0 0 C
class TrefoilFourierKnot : public FourierKnot {
public:
  TrefoilFourierKnot() {}

  TrefoilFourierKnot(const char* file) {
    ifstream in(file);
    if (in.good()) in >> *this;
    else {
      cerr << "TrefoilFourierKnot : Could not read " << file << endl;
      exit(2);
    }
  }

  // point at curve(s), s in (0,1)
  Vector3 operator()(float t) {
    float f1,f2,f3;
    Vector3 r(0,0,0);
    // XXX optimise/cache this (precompute cos(f1*t) ... and swap values 1<-2<-3, precomp 3 iterate
    for (uint i=0;i<csin.size();++i) {
      f1 = (float)(3*i+1)*(2.*M_PI);
      f2 = (float)(3*i+2)*(2.*M_PI);
      f3 = (float)(3*i+3)*(2.*M_PI);
      // formula for cos(a)-sin(a)?
      r += Vector3(-csin[i][0]*cos(f1*t)+csin[i][1]*cos(f2*t),
                    csin[i][0]*sin(f1*t)+csin[i][1]*sin(f2*t),
                    csin[i][2]*(sin(f3*t)));
    }
    return r;
  }

  // tangent at curve(t)
  Vector3 prime(float t) {
    Vector3 r; float f1,f2,f3;
    // XXX optimise/cache this
    for (uint i=0;i<csin.size();++i) {
      f1 = (float)(3*i+1)*(2.*M_PI);
      f2 = (float)(3*i+2)*(2.*M_PI);
      f3 = (float)(3*i+3)*(2.*M_PI);
      // formula for cos(a)-sin(a)?
      r += Vector3(f1*csin[i][0]*sin(f1*t)-f2*csin[i][1]*sin(f2*t),
                   f1*csin[i][0]*cos(f1*t)+f2*csin[i][1]*cos(f2*t),
                   f3*csin[i][2]*(cos(f3*t)));
    }
    r.normalize();
    return r;
  }
 
  void scale(float s) {
    c0 *= s;
    for (uint i=0;i<csin.size();++i)
      csin[i] *= s;
  }


  friend ostream& operator<<(ostream &out, const TrefoilFourierKnot &fk);
  friend istream& operator>>(istream &in, TrefoilFourierKnot &fk);
};

inline ostream& operator<<(ostream &out, const TrefoilFourierKnot &fk) {
  for (uint i=0;i<fk.csin.size();++i)
    out << fk.csin[i] << endl;
  return out;
}

// Read in a trefoil fourier coeff file
// structure :
// sin_y_i sin_y_{i+1} sin_z_{i+2}        // this is 3 rows in the standart fourier coeff file
// ...
inline istream& operator>>(istream &in, TrefoilFourierKnot &fk) {
  // csin not used in a trefoil fourier knot
  Coeff c;
  while ((in >> c)) {
    fk.csin.push_back(c);
  }
  if (in.fail() && !in.eof()) {
    cerr << "istream (TrefoilFourierKnot) : Bad input!\n";
    exit(1);
  };
  return in;
}


// Translate a normal coeff file to trefoil sparse coeff file
void coeffs2fourier(const char* file, FourierKnot *fk) {
  FourierKnot tmp(file); Vector3 v;
  fk->clear();
  for (uint i=0;i<tmp.csin.size();i+=3) {
    v.setValues(tmp.csin[i][1],tmp.csin[i+1][1],tmp.csin[i+2][2]);
    // cout << v << endl;
    fk->csin.push_back(v);
  }
}


// #define TEST
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

  TrefoilFourierKnot fk;
  coeffs2fourier(argv[1],&fk);
  fk.scale(4);
  cout << fk.csin.size() << " coefficiant rows!\n";

  Curve<Vector3> knot;
  fk.toCurve(200,&knot);

  knot.header("trefoil fourier knot","coeff2pkf","H. Gerlach","");
  knot.writePKF("test.pkf");

#ifdef THICKNESS
  knot.link();
  knot.make_default();
  float D = knot.thickness();
  float L = knot.length();

  cout << "D=" << D << ",L/D="<<L/D<<endl;
#endif
  return 0;
}
#endif
