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
  ~FourierKnot();
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
  void rotate(Vector3 v,float alpha);
  void flip_dir(float sh = 0);
  void shift(float sh);

  // Friend classes
  friend istream& operator>>(istream &in, FourierKnot &fk);
  friend ostream& operator<<(ostream &out, const FourierKnot &fk);

};


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

  // Constructors
  TrefoilFourierKnot();
  TrefoilFourierKnot(const char* file);

  Vector3 operator()(float t);
  Vector3 prime(float t); 
  void scale(float s);

  // Friend classes
  friend ostream& operator<<(ostream &out, const TrefoilFourierKnot &fk);
  friend istream& operator>>(istream &in, TrefoilFourierKnot &fk);
};

// (Trefoil)FourierKnot Classes I/O Friends

inline ostream& operator<<(ostream &out, const FourierKnot &fk) {
  assert(fk.ccos.size()==fk.csin.size());
  out << fk.c0 << endl;
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

