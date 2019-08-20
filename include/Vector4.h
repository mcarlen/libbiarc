#ifndef _VECTOR4_H_
#define _VECTOR4_H_

#include <math.h>
#include <iostream>
#include "Vector3.h"

using namespace std;

class Vector4 {
 private:
  FLOAT_TYPE	_v[4];
 public:

  // FIXME : how many dims? is this necessary?
  static const unsigned int type = 4;

  Vector4() ;
  Vector4(const FLOAT_TYPE x, const FLOAT_TYPE y,
	  const FLOAT_TYPE z, const FLOAT_TYPE w) ;
  Vector4(const FLOAT_TYPE v[4]) ;
  Vector4(const Vector4& v) ;
  Vector4(const Vector3& v, const FLOAT_TYPE f) ;
  ~Vector4() ;

  FLOAT_TYPE &operator[](const int c);
  const FLOAT_TYPE &operator[](const int c) const;

  Vector4 & zero() ;

  void getValues(FLOAT_TYPE& X, FLOAT_TYPE& Y,
		 FLOAT_TYPE& Z, FLOAT_TYPE& W) const;
  Vector4 & setValues(const FLOAT_TYPE X, const FLOAT_TYPE Y,
		      const FLOAT_TYPE Z, const FLOAT_TYPE W);
  Vector4 & setValues(const FLOAT_TYPE v[4]);

  FLOAT_TYPE dot(const Vector4 &v) const;
  FLOAT_TYPE norm() const;
  FLOAT_TYPE norm2() const;
  Vector4 &normalize();
  FLOAT_TYPE max();
  FLOAT_TYPE min();

  Vector4 reflect(const Vector4 &v) const;
  /*
  Vector4 rotPtAroundAxis(FLOAT_TYPE angle, Vector4 axis);
  */

  Vector4 operator*(const Vector4 &v) const;
  // Vector4 operator*(const FLOAT_TYPE s) const;
  // Vector4 operator/(const FLOAT_TYPE d) const;
  friend Vector4 operator*(const Vector4 & v, FLOAT_TYPE d);
  friend Vector4 operator*(FLOAT_TYPE d, const Vector4 & v);
  friend Vector4 operator/(const Vector4 & v, FLOAT_TYPE d);

  Vector4 operator+(const Vector4 &v) const;
  Vector4 operator-(const Vector4 &v) const;
  Vector4 operator-() const;

  // Vector4& operator=(const Vector4 &v);
  Vector4& operator+=(const Vector4 &v);
  Vector4& operator-=(const Vector4 &v);
  Vector4& operator*=(const FLOAT_TYPE s);
  Vector4& operator/=(const FLOAT_TYPE s);

  int operator==(const Vector4 &v) const;
  int operator!=(const Vector4 &v) const;

  void print(ostream &out) const;
  friend ostream & operator << (ostream &out, const Vector4 &v);
  friend istream & operator >> (istream &in, Vector4 &v);

};

inline FLOAT_TYPE & Vector4::operator [](const int c) {
  return this->_v[c];
}

inline const FLOAT_TYPE & Vector4::operator [](const int c) const {
  return this->_v[c];
}

inline ostream & operator<< (ostream &out, const Vector4 &v) {
  v.print(out);
  return out;
}

inline istream & operator>> (istream &in, Vector4 &v) {
  in >> v[0] >> v[1] >> v[2] >> v[3];
  return in;
}


#endif	// _VECTOR4_H_
