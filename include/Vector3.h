#ifndef _VECTOR3_H_
#define _VECTOR3_H_

#include <math.h>
#include <iostream>

// #define float double
using namespace std;

class Vector3 {
 private:
  float	_v[3];
 public:

  // FIXME : how many dims? is this necessary?
  static const unsigned int type = 3;
  
  Vector3() ;
  Vector3(const float x, const float y, const float z) ;
  Vector3(const float v[3]) ;
  Vector3(const Vector3& v) ;
  ~Vector3() ;

  float &operator[](const int c);
  const float &operator[](const int c) const;

  Vector3 & zero() ;

  void getValues(float& X, float& Y, float& Z) const;
  Vector3 & setValues(const float X, const float Y, const float Z);
  Vector3 & setValues(const float v[3]);

  float dot(const Vector3 &v) const;
  Vector3 cross(const Vector3 &v);			
  float norm() const;
  float norm2() const;
  Vector3 &normalize();
  float max();
  float min();

  Vector3 reflect(const Vector3 &v) const;
  Vector3 rotPtAroundAxis(float angle, Vector3 axis) const;

  Vector3 operator*(const Vector3 &v) const;
  Vector3 operator*(const float s) const;
  Vector3 operator/(const float d) const;
  friend Vector3 operator *(Vector3 & v, float d);
  friend Vector3 operator *(float d, Vector3 & v);
  friend Vector3 operator /(Vector3 & v, float d);

  Vector3 operator+(const Vector3 &v) const;
  Vector3 operator-(const Vector3 &v) const;
  Vector3 operator-() const;

//  Vector3& operator=(const Vector3 &v);
  Vector3& operator+=(const Vector3 &v);
  Vector3& operator-=(const Vector3 &v);
  Vector3& operator*=(const float s);
  Vector3& operator/=(const float s);

  int operator==(const Vector3 &v) const;
  int operator!=(const Vector3 &v) const;

  void print(ostream &out) const;
  friend ostream & operator << (ostream &out, const Vector3 &v);

};

inline float & Vector3::operator [](const int c) {
  return this->_v[c];
}

inline const float & Vector3::operator [](const int c) const {
  return this->_v[c];
}

inline ostream & operator<< (ostream &out, const Vector3 &v) {
  v.print(out);
  return out;
}

#endif	// _VECTOR3_H_
