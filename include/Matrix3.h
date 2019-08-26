#ifndef _MATRIX3_H_
#define _MATRIX3_H_

#include "Vector3.h"

class Matrix3 {

  Vector3 _v[3];

 public:

  Matrix3();
  Matrix3(const Vector3 &v0,const Vector3 &v1,const Vector3 &v2);
  Matrix3(const FLOAT_TYPE &x00, const FLOAT_TYPE &x01, const FLOAT_TYPE &x02,
          const FLOAT_TYPE &x10, const FLOAT_TYPE &x11, const FLOAT_TYPE &x12,
          const FLOAT_TYPE &x20, const FLOAT_TYPE &x21, const FLOAT_TYPE &x22);

  Matrix3 & zero();
  Vector3 &operator[](int n);
  const Vector3 &operator[](int n) const;
    
  void setOne(const int c, const Vector3 &v);
  void setOne(const int c, const FLOAT_TYPE v1, const FLOAT_TYPE v2, const FLOAT_TYPE v3);
  void setAll(const Vector3 &v1, const Vector3 &v2, const Vector3 &v3);
  void setAll(const Vector3 *v);
  Vector3 getOne(const int c);
  void getAll(Vector3 &v1, Vector3 &v2, Vector3 &v3);

  Matrix3 & id();
  Matrix3 & transpose();
  FLOAT_TYPE det();
  Matrix3 & inv();
  Matrix3& outer(const Vector3 &a, const Vector3 &b);
  Matrix3& vecCross(const Vector3 &v);
  Matrix3& cay(const Vector3 &v);
  Matrix3& rotAround(const Vector3 &v, FLOAT_TYPE angle);

  Matrix3 operator*(const Matrix3 &m);
  Vector3 operator*(const Vector3 &v);
  friend Matrix3 operator*(const Matrix3 &m, FLOAT_TYPE d);
  friend Matrix3 operator*(FLOAT_TYPE d, const Matrix3 &m);
  friend Matrix3 operator/(const Matrix3 &m, FLOAT_TYPE d);

  Matrix3 operator+(const Matrix3 &m) const;
  Matrix3 operator-(const Matrix3 &m) const;
  Matrix3 operator-() const;
  
  Matrix3& operator=(const Matrix3 &m);
  Matrix3& operator+=(const Matrix3 &m);
  Matrix3& operator-=(const Matrix3 &m);
  Matrix3& operator*=(const FLOAT_TYPE s);
  Matrix3& operator/=(const FLOAT_TYPE s);
  
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
