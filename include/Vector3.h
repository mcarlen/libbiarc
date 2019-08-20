#ifndef _VECTOR3_H_
#define _VECTOR3_H_

#include <math.h>
#include <iostream>

#ifdef PRECISION_HACK
#define FLOAT_TYPE float
#else
#define FLOAT_TYPE double
#endif

using namespace std;

class Vector3 {
 private:
  FLOAT_TYPE	_v[3];
 public:

  // FIXME : how many dims? is this necessary?
  static const unsigned int type = 3;
  
  Vector3() ;
  Vector3(const FLOAT_TYPE x, const FLOAT_TYPE y, const FLOAT_TYPE z) ;
  Vector3(const FLOAT_TYPE v[3]) ;
  Vector3(const Vector3& v) ;
  ~Vector3() ;

  FLOAT_TYPE &operator[](const int c);
  const FLOAT_TYPE &operator[](const int c) const;

  Vector3 & zero() ;

  void getValues(FLOAT_TYPE& X, FLOAT_TYPE& Y, FLOAT_TYPE& Z) const;
  Vector3 & setValues(const FLOAT_TYPE X, const FLOAT_TYPE Y, const FLOAT_TYPE Z);
  Vector3 & setValues(const FLOAT_TYPE v[3]);

  FLOAT_TYPE dot(const Vector3 &v) const;
  Vector3 cross(const Vector3 &v) const;
  FLOAT_TYPE norm() const;
  FLOAT_TYPE norm2() const;
  Vector3 &normalize();
  FLOAT_TYPE max();
  FLOAT_TYPE min();

  Vector3 reflect(const Vector3 &v) const;
  Vector3 rotPtAroundAxis(FLOAT_TYPE angle, Vector3 axis) const;

  Vector3 operator*(const Vector3 &v) const;
  // Vector3 operator*(const FLOAT_TYPE s) const;
  // Vector3 operator/(const FLOAT_TYPE d) const;
  friend Vector3 operator*(const Vector3 & v, FLOAT_TYPE d);
  friend Vector3 operator*(FLOAT_TYPE d, const Vector3 & v);
  friend Vector3 operator/(const Vector3 & v, FLOAT_TYPE d);

  Vector3 operator+(const Vector3 &v) const;
  Vector3 operator-(const Vector3 &v) const;
  Vector3 operator-() const;

//  Vector3& operator=(const Vector3 &v);
  Vector3& operator+=(const Vector3 &v);
  Vector3& operator-=(const Vector3 &v);
  Vector3& operator*=(const FLOAT_TYPE s);
  Vector3& operator/=(const FLOAT_TYPE s);

  int operator==(const Vector3 &v) const;
  int operator!=(const Vector3 &v) const;

  void print(ostream &out) const;
  friend ostream & operator << (ostream &out, const Vector3 &v);
  friend istream & operator >> (istream &in, Vector3 &v);

};

inline FLOAT_TYPE & Vector3::operator [](const int c) {
  return this->_v[c];
}

inline const FLOAT_TYPE & Vector3::operator [](const int c) const {
  return this->_v[c];
}

inline ostream & operator<< (ostream &out, const Vector3 &v) {
  v.print(out);
  return out;
}

inline istream & operator>> (istream &in, Vector3 &v) {
  in >> v[0] >> v[1] >> v[2];
  return in;
}

#endif	// _VECTOR3_H_
