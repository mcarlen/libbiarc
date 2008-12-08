#ifndef __FOURIER_SYN__
#define __FOURIER_SYN__

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

typedef Vector3 Coeff;
typedef vector<Coeff> Coeffs;
typedef unsigned int uint;

class FourierKnot {

public:

  // Members
  Coeff c0;
  Coeffs csin, ccos;
  

  // Constructor / Destructor
  FourierKnot();
  virtual ~FourierKnot();
  FourierKnot(const char* file);
  FourierKnot(Coeffs lsin, Coeffs lcos);
  FourierKnot(Coeff constant, Coeffs lsin, Coeffs lcos);

  void clear();
  void scale(float s);

  void set(Coeff c, Coeffs lsin, Coeffs lcos);
  void setConst(Coeff constant);
  void setSin(Coeffs lsin);
  void setCos(Coeffs lcos);

  virtual Vector3 operator()(float t);
  virtual Vector3 prime(float t);
  virtual void toCurve(const int sampling, Curve<Vector3> *curve);
  virtual void toCurve(float(*pt2func)(float), const int sampling, Curve<Vector3> *curve);
  void rotate(const Vector3 v,float alpha);
  void apply(Matrix3 &m);
  void mirror(const Vector3 v);
  void flip_dir(float sh = 0);
  void shift(float sh);

  FourierKnot operator*(const float d) const;
  FourierKnot operator/(const float d) const;
  FourierKnot operator+(const FourierKnot &fk) const;

  // Friend classes
  friend istream& operator>>(istream &in, FourierKnot &fk);
  friend ostream& operator<<(ostream &out, const FourierKnot &fk);

};


inline Vector3 cut(Vector3 v) {
  if (fabs(v[0])<1e-15) v[0] = 0;
  if (fabs(v[1])<1e-15) v[1] = 0;
  if (fabs(v[2])<1e-15) v[2] = 0;
  return v;
}

// FourierKnot Classes I/O Friends
inline ostream& operator<<(ostream &out, const FourierKnot &fk) {
  assert(fk.ccos.size()==fk.csin.size());
  out << fk.c0 << endl;
  for (uint i=0;i<fk.ccos.size();++i)
    out << cut(fk.ccos[i]) << " " << cut(fk.csin[i]) << endl;
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

#endif // __FOURIER_SYN__
