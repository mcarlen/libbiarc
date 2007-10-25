#ifndef _MATRIX3_H_
#define _MATRIX3_H_

#include "Vector3.h"

class Matrix3 {

  Vector3 _v[3];

 public:

  Matrix3();
  Matrix3(const Vector3 &v0,const Vector3 &v1,const Vector3 &v2);
  Matrix3(const float &x00, const float &x01, const float &x02,
          const float &x10, const float &x11, const float &x12,
          const float &x20, const float &x21, const float &x22);

  Matrix3 & zero();
  Vector3 &operator[](int n);
  const Vector3 &operator[](int n) const;
    
  void setOne(const int c, const Vector3 &v);
  void setOne(const int c, const float a, const float b, const float c);
  void setAll(const Vector3 &v1, const Vector3 &v2, const Vector3 &v3);
  void setAll(const Vector3 *v);
  Vector3 getOne(const int c);
  void getAll(Vector3 &v1, Vector3 &v2, Vector3 &v3);

  Matrix3 & id();
  Matrix3 & transpose();
  float det();
  Matrix3 & inv();
  Matrix3& outer(const Vector3 &a, const Vector3 &b);
  Matrix3& vecCross(const Vector3 &v);
  Matrix3& cay(const Vector3 &v);

  Matrix3 operator*(const Matrix3 &m);
  Vector3 operator*(const Vector3 &v);
  friend Matrix3 operator*(const Matrix3 &m, float d);
  friend Matrix3 operator*(float d, const Matrix3 &m);
  friend Matrix3 operator/(const Matrix3 &m, float d);

  Matrix3 operator+(const Matrix3 &m) const;
  Matrix3 operator-(const Matrix3 &m) const;
  Matrix3 operator-() const;
  
  Matrix3& operator=(const Matrix3 &m);
  Matrix3& operator+=(const Matrix3 &m);
  Matrix3& operator-=(const Matrix3 &m);
  Matrix3& operator*=(const float s);
  Matrix3& operator/=(const float s);
  
  int operator==(const Matrix3 &m) const;
  int operator!=(const Matrix3 &m) const;
    
  void print(ostream &out) const;
  friend ostream& operator << (ostream &out, const Matrix3 &m);

};

inline Vector3 &Matrix3::operator[](int n) {
  return _v[n];
}

inline const Vector3 &Matrix3::operator[](int n) const {
  return _v[n];
}

inline ostream & operator << (ostream &out, const Matrix3 &m) {
  m.print(out);
  return out;
}

#endif // _MATRIX3_H_
