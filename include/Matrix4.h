#ifndef _MATRIX4_H_
#define _MATRIX4_H_

#include "Vector3.h"
#include "Matrix3.h"
#include "Vector4.h"

class Matrix4 {

  Vector4 _v[4];

 public:

  Matrix4();
  Matrix4(const Vector4 &v0,const Vector4 &v1,
	  const Vector4 &v2,const Vector4 &v3);

  Matrix4 & zero();
  Vector4 &operator[](int n);
  const Vector4 &operator[](int n) const;
    
  void setOne(const int c, const Vector4 &v);
  void setOne(const int c, const float a, const float b,
	      const float c, const float d);
  void setAll(const Vector4 &v1, const Vector4 &v2,
	      const Vector4 &v3, const Vector4 &v4);
  void setAll(const Vector4 *v);
  Vector4 getOne(const int c);
  void getAll(Vector4 &v1, Vector4 &v2,
	      Vector4 &v3, Vector4 &v4);

  Matrix4 & id();
  Matrix4 & transpose();
  Matrix3 sub(const int r, const int c);
  float det();
  Matrix4 & inv();
  Matrix4 & adjoint();
  Matrix4& outer(const Vector4 &a, const Vector4 &b);

  Matrix4 operator*(Matrix4 &m);
  Vector4 operator*(Vector4 &v);
  friend Matrix4 operator*(Matrix4 &m, float d);
  friend Matrix4 operator*(float d, Matrix4 &m);
  friend Matrix4 operator/(Matrix4 &m, float d);

  Matrix4 operator+(const Matrix4 &m) const;
  Matrix4 operator-(const Matrix4 &m) const;
  Matrix4 operator-() const;
  
  Matrix4& operator=(const Matrix4 &m);
  Matrix4& operator+=(const Matrix4 &m);
  Matrix4& operator-=(const Matrix4 &m);
  Matrix4& operator*=(const float s);
  Matrix4& operator/=(const float s);
  
  int operator==(const Matrix4 &m) const;
  int operator!=(const Matrix4 &m) const;
    
  void print(ostream &out) const;
  friend ostream& operator << (ostream &out, const Matrix4 &m);

};

inline Vector4 &Matrix4::operator[](int n) {
  return _v[n];
}

inline const Vector4 &Matrix4::operator[](int n) const {
  return _v[n];
}

inline ostream & operator << (ostream &out, const Matrix4 &m) {
  m.print(out);
  return out;
}

#endif // _MATRIX4_H_
