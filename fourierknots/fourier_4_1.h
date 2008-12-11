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
  FourierKnot toFourierKnot();
  Vector3 operator()(float t);
  Vector3 prime(float t); 

  K41FourierKnot operator+(const K41FourierKnot &fk);
  K41FourierKnot& operator*=(const float d);
  K41FourierKnot& operator/=(const float d);

  /*
     I/O classes not overloaded!
     We use the same structure as the normal coeff file
     (with constant 0 0 0). This means we can use the
     parent classe's operator<< and operator>>.
  */
};

#endif // __FOURIER_4_1__
