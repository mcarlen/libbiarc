#ifndef __FOURIER_4_1__
#define __FOURIER_4_1__

#include "fourier_syn.h"

// Observation (for k4.1)
//
// cosx cosy cosz sinx siny sinz
//  A 0 B -B 0  A
//  0 C 0  0 D  0    // maybe C = 2*D
//  E 0 F  F 0 -E
//  0 G 0  0 H  0    // G and H very small
//
// Let's try : A B C D E F  as coeff file
class K41FourierKnot : public FourierKnot {

public:

  // Constructors
  K41FourierKnot();
  K41FourierKnot(const char* file, int Normal = 0);
  K41FourierKnot(const K41FourierKnot &tfk);

  K41FourierKnot& operator=(const K41FourierKnot &fk);
  Vector3 operator()(float t);
  Vector3 prime(float t); 

  K41FourierKnot operator+(const K41FourierKnot &fk);
  K41FourierKnot& operator*=(const float d);
  K41FourierKnot& operator/=(const float d);

  // Friend classes, we use the same structure as the normal coeff file (with constant 0 0 0)
//  friend ostream& operator<<(ostream &out, const K41FourierKnot &fk);
//  friend istream& operator>>(istream &in, K41FourierKnot &fk);
};

/*
inline ostream& operator<<(ostream &out, const K41FourierKnot &fk) {
  for (uint i=0;i<fk.csin.size();++i)
    out << fk.ccos[i] << " " << fk.csin[i] << endl;
  return out;
}

// Read in a figure eight fourier coeff file
// structure :
// sin_y_i sin_y_{i+1} sin_z_{i+2}        // this is 3 rows in the standart fourier coeff file
// ...
inline istream& operator>>(istream &in, K41FourierKnot &fk) {
  // csin not used in a trefoil fourier knot
  Coeff c;
  while ((in >> c)) {
    fk.csin.push_back(c);
  }
  if (in.fail() && !in.eof()) {
    cerr << "istream (K41FourierKnot) : Bad input!\n";
    exit(1);
  };
  return in;
}
*/

#endif // __FOURIER_4_1__
