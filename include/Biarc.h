#ifndef _BIARC_H_
#define _BIARC_H_

#include "Vector3.h"
#include "Matrix3.h"

// FIXME : is there nice way to check for inflection points
// and straight line segments. So far dot product between
// start and end tangent is checked not to be close to one
// up to the StraightSegTol
#define StraightSegTol   1e-7

template<class Vector>
class Curve;

//template class Vector;

// Make this templated! with template for the Point, Tangent container!!

template<class Vector>
class Biarc {

  int _BIARC_;

  Vector _Point, _Tangent;
  Vector _MidPoint, _MidTangent;
  Vector _BezierPoint0, _BezierPoint1;

  // each Biarc has global knowledge of where it is in a curve!
  int _BiarcInCurve;
  Curve< Vector > *_Curve;

  // Biarc<Vector> *_Next,*_Previous;

  float _Radius0, _Radius1;
  float _ArcLength0, _ArcLength1;
  float _Length;

  float _radius(const Vector &q0, const Vector &q1, const Vector &t0) const;
  float _radius0() const;
  float _radius1() const;
  float _angle(const Vector &q0, const Vector &q1, const Vector &t0) const;
  float _angle0() const;
  float _angle1() const;

  float _arclength0() const;
  float _arclength1() const;
  float _biarclength() const;

  Vector a(const Vector &b0,const Vector &b1,const Vector &b2,float tau) const;
  void cache();

 public:

  Biarc() ;
  Biarc(const Vector &p,const Vector &t);
  Biarc(const Biarc<Vector>& b);
  ~Biarc();

  const Vector& getPoint() const;
  const Vector& getTangent() const;
  const Vector& getMidPoint() const;
  const Vector& getMidTangent() const;
  void setPoint(const Vector &p);
  // void setPoint(float &p0,float &p1,float &p2);

  // These functions should update cached values (i.e. radius,angles...)
  void setMidPoint(const Vector &p);
  // void setMidPoint(float &p0,float &p1,float &p2);
  void setMidTangent(const Vector &p);
  // void setMidTangent(float &p0,float &p1,float &p2);

  void setTangentUnnormalized(const Vector &t);
  void setTangent(const Vector &t);
  // void setTangent(float &t0,float &t1,float &t2);
  void get(Vector& p, Vector& t) const;
  void set(const Vector& p, const Vector& t);
  void clear();
  
  void reverse();
  int isProper() const;
  int isBiarc() const;
  void setBiarc();

  // FIXME : wrap around if closed and we're at the end
  //         using global knowledge about position in curve
  void make(float Gamma);
  
  float arclength0() const;
  float arclength1() const;
  float biarclength() const;

  float radius0() const;
  float radius1() const;

  int id() const;
  // XXX : these functions should not be available to the user
  //       friends?
  void setId(const int i);
  void setCurve(Curve<Vector>* c);
  const Curve<Vector>* getCurve();
  void setIdAndCurve(const int i, Curve<Vector>* c);
  Vector pointOnArc0(float s) const;
  Vector pointOnArc1(float s) const;
  Vector pointOnBiarc(float arclength) const;
  Vector tangentOnBiarc(float arclength) const;

  // replace & by * and set & explicitly when call
  void getBezierArc0(Vector& b0, Vector& b1, Vector& b2) const;
  void getBezierArc1(Vector& b0,Vector& b1,Vector& b2) const;
  void getBezier(Vector& b0_0, Vector& b1_0, Vector& b2_0,
		 Vector& b0_1, Vector& b1_1, Vector& b2_1) const;
  Vector a0(float tau) const;
  Vector a1(float tau) const;

  const Biarc<Vector>& getNext() const;
  const Biarc<Vector>& getPrevious() const;
  void setNext(const Biarc<Vector> &b);
  void setPrevious(const Biarc<Vector> &b);
  void setNextNULL();
  void setPreviousNULL();

  Biarc<Vector> operator*(const float s) const;
  /*
  friend Biarc operator *(Biarc & b, float d);
  friend Biarc operator *(float d, Biarc & b);
  friend Biarc operator /(Biarc & b, float d);
  */
  Biarc<Vector> operator+(const Vector &v) const;
  Biarc<Vector> operator-(const Vector &v) const;
  Biarc<Vector> &operator=(const Biarc<Vector> &b);
  Biarc<Vector> &operator+=(const Vector &v);
  Biarc<Vector> &operator-=(const Vector &v);
  Biarc<Vector> &operator/=(const float d);
  Biarc<Vector> &operator*=(const float d);

  void print(ostream &out) const ;
  int operator==(const Biarc<Vector> &b) const;
  int operator!=(const Biarc<Vector> &b) const;

//  friend ostream& operator << (ostream &out, const Biarc<Vector>& b);

};

template<class Vector>
inline ostream& operator << (ostream &out, const Biarc<Vector>& b) {
  b.print(out);
  return out;
}

// Templates need all code in the same file
// that's why we include here the source file!!!
#include "../lib/Biarc.cpp"

#endif // _BIARC_H_
