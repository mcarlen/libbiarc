#ifndef __FOURIER_3_1__
#define __FOURIER_3_1__

#include "fourier_syn.h"

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
  // Members
  float _shift;

  // Constructors
  TrefoilFourierKnot();
  TrefoilFourierKnot(const char* file);
  TrefoilFourierKnot(const TrefoilFourierKnot &tfk);

  TrefoilFourierKnot& operator=(const TrefoilFourierKnot &tfk);
  Vector3 operator()(float t);
  Vector3 prime(float t); 
  void scale(float s);
  void shift(float sh);

  // Friend classes
  friend ostream& operator<<(ostream &out, const TrefoilFourierKnot &fk);
  friend istream& operator>>(istream &in, TrefoilFourierKnot &fk);
};

// Write trefoil fourier coeffs to stream
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


float adjusthelper(float x, float a1=0.01, float h1=0.01, float a2=0.07, float h2=0.005);
float adjust3(float x);
float adjust2(float x);
float adjust(float x);

#endif // __FOURIER_3_1__
