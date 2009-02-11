#ifndef __FOURIER_5_1__
#define __FOURIER_5_1__

#include "fourier_syn.h"

// Observation (for k5.1)
//
// cosx cosy cosz sinx siny sinz
//  0 0 0
//  0 A 0 B 0 C
//  0 D 0 E 0 F
//  ...
// Let's try : A B C D E F  as coeff file
class K51FourierKnot : public FourierKnot {

public:

  // Constructors
  K51FourierKnot();
  K51FourierKnot(const char* file, int Normal = 0);
  K51FourierKnot(const K51FourierKnot &tfk);

  K51FourierKnot& operator=(const K51FourierKnot &fk);
  FourierKnot toFourierKnot();
  Vector3 operator()(float t) const;
  Vector3 prime(float t) const; 

  K51FourierKnot operator+(const K51FourierKnot &fk);
  K51FourierKnot& operator*=(const float d);
  K51FourierKnot& operator/=(const float d);

  /*
     I/O classes not overloaded!
     We use the same structure as the normal coeff file
     (with constant 0 0 0). This means we can use the
     parent classe's operator<< and operator>>.
  */
};

#endif // __FOURIER_5_1__
