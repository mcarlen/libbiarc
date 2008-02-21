#ifndef _VECTOR4_H_
#define _VECTOR4_H_

#include <math.h>
#include <iostream>
#include "Vector3.h"

using namespace std;

class Vector4 {
 private:
  float	_v[4];
 public:

  // FIXME : how many dims? is this necessary?
  static const unsigned int type = 4;

  Vector4() ;
  Vector4(const float x, const float y,
	  const float z, const float w) ;
  Vector4(const float v[4]) ;
  Vector4(const Vector4& v) ;
  Vector4(const Vector3& v, const float f) ;
  ~Vector4() ;

  float &operator[](const int c);
  const float &operator[](const int c) const;

  Vector4 & zero() ;

  void getValues(float& X, float& Y,
		 float& Z, float& W) const;
  Vector4 & setValues(const float X, const float Y,
		      const float Z, const float W);
  Vector4 & setValues(const float v[4]);

  float dot(const Vector4 &v) const;
  float norm() const;
  float norm2() const;
  Vector4 &normalize();
  float max();
  float min();

  Vector4 reflect(const Vector4 &v) const;
  /*
  Vector4 rotPtAroundAxis(float angle, Vector4 axis);
  */

  Vector4 operator*(const Vector4 &v) const;
  // Vector4 operator*(const float s) const;
  // Vector4 operator/(const float d) const;
  friend Vector4 operator*(const Vector4 & v, float d);
  friend Vector4 operator*(float d, const Vector4 & v);
  friend Vector4 operator/(const Vector4 & v, float d);

  Vector4 operator+(const Vector4 &v) const;
  Vector4 operator-(const Vector4 &v) const;
  Vector4 operator-() const;

  // Vector4& operator=(const Vector4 &v);
  Vector4& operator+=(const Vector4 &v);
  Vector4& operator-=(const Vector4 &v);
  Vector4& operator*=(const float s);
  Vector4& operator/=(const float s);

  int operator==(const Vector4 &v) const;
  int operator!=(const Vector4 &v) const;

  void print(ostream &out) const;
  friend ostream & operator << (ostream &out, const Vector4 &v);

};

inline float & Vector4::operator [](const int c) {
  return this->_v[c];
}

inline const float & Vector4::operator [](const int c) const {
  return this->_v[c];
}

inline ostream & operator<< (ostream &out, const Vector4 &v) {
  v.print(out);
  return out;
}

#endif	// _VECTOR4_H_
