#ifndef __FOURIER_8_18__
#define __FOURIER_8_18__

#include "fourier_syn.h"

// Structure (for k8.18)
//
// cosx cosy cosz sinx siny sinz
//  0 0 0  0  0  0
//  0 0 0  0  0  0
//  A B 0 -B  A  0
//  0 0 C  0  0  D
//  E F 0  F -E  0
//  0 0 0  0  0  0
//  0 0 0  0  0  0
//  0 0 0  0  0  0
//
// Let's try : A B C D E F  as coeff file
class K818FourierKnot : public FourierKnot {

public:

  // Constructors
  K818FourierKnot();
  K818FourierKnot(const char* file, int Normal = 0);
  K818FourierKnot(const K818FourierKnot &tfk);

  K818FourierKnot& operator=(const K818FourierKnot &fk);
  FourierKnot toFourierKnot();
  Vector3 operator()(float t) const;
  Vector3 prime(float t) const; 

  K818FourierKnot operator+(const K818FourierKnot &fk);
  K818FourierKnot& operator*=(const float d);
  K818FourierKnot& operator/=(const float d);

  /*
     I/O classes not overloaded!
     We use the same structure as the normal coeff file
     (with constant 0 0 0). This means we can use the
     parent classe's operator<< and operator>>.
  */
};

#endif // __FOURIER_8_18__
