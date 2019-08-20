/*!
  \class Vector4 Vector4.h include/Vector4.h
  \ingroup BiarcLibGroup
  \brief The Vector4 class is a 4 dimensional Vector class with
  FLOAT_TYPEing point coordinates.

  This class provides storage for a 4 dimensional vector
  aswell as arithmetic operations on vectors, like the dot
  product, the cross product, the length of the vector
  in 4-dimensions.

  \sa class Vector3
*/
 
 
//
// documentation of inlined methods
//
 
/*!
  \fn FLOAT_TYPE & Vector4::operator[](const int c)
  Index operator. Returns modifiable x, y or z coordinate of the vector.
 
  \sa getValue() and setValue().
*/
 
/*!
  \fn FLOAT_TYPE Vector4::operator[](const int c) const
  Index operator. Returns x, y or z coordinate of vector.
 
  \sa getValue() and setValue().
*/

/*!
  \fn ostream & Vector4::operator<< (ostream &out, const Vector4 &v)

  Overloaded left shift operator. Returns the Vector \a v as an ostream
  that can be written to a file or to standart output.

  \sa print()
*/

/*!
  \fn ostream & Vector4::operator>> (istream &out, Vector4 &v)

  Overloaded right shift operator. Initialize vector \a v
	with istream \a out.

  \sa print() operator<<()
*/

#include "../include/Vector4.h"
#include "../include/Matrix4.h"

/*!
  Constructs an empty vector initialized to zero.

  \sa zero().
*/
Vector4::Vector4() {
  _v[0] = 0.0;
  _v[1] = 0.0;
  _v[2] = 0.0;
  _v[3] = 0.0;
}

/*!
  Constructs a Vector4 instance from \a x, \a y, \a z and \a w.
*/
Vector4::Vector4(const FLOAT_TYPE x, const FLOAT_TYPE y,
		 const FLOAT_TYPE z, const FLOAT_TYPE w) {
  _v[0] = x;
  _v[1] = y;
  _v[2] = z;
  _v[3] = w;
}

/*!
  Constructs a Vector4 with initial values from \a v.
*/
Vector4::Vector4(const FLOAT_TYPE v[4]) {
  _v[0] = v[0];
  _v[1] = v[1];
  _v[2] = v[2];
  _v[3] = v[3];
}

/*!
  Construct a Vector4 object from a Vector3 \a v and a FLOAT_TYPE \a f.
*/
Vector4::Vector4(const Vector3& v, FLOAT_TYPE f) {
  _v[0] = v[0];
  _v[1] = v[1];
  _v[2] = v[2];
  _v[3] = f;
}


/*!
  The destructor has nothing to do
*/
Vector4::~Vector4() {}

/*!
  Constructs a copy of \a v.
*/
Vector4::Vector4(const Vector4& v) {
  _v[0] = v[0];
  _v[1] = v[1];
  _v[2] = v[2];
  _v[3] = v[3];
}

/*!
  Sets all vector components to zero
*/
Vector4 & Vector4::zero() {
  this->_v[0]=0.0;
  this->_v[1]=0.0;
  this->_v[2]=0.0;
  this->_v[3]=0.0;
  return *this;
}

/*!
  Recover the vector components and put them into \a X, \a Y, \a Z and \a W.
*/
void Vector4::getValues(FLOAT_TYPE& X, FLOAT_TYPE& Y,
			FLOAT_TYPE& Z, FLOAT_TYPE& W) const {
  X = _v[0];
  Y = _v[1];
  Z = _v[2];
  W = _v[3];
}

/*!
  Set the vector to \a <X,Y,Z,W>. Returns a reference
  to itself.
*/
Vector4 & Vector4::setValues(const FLOAT_TYPE X, const FLOAT_TYPE Y,
			     const FLOAT_TYPE Z, const FLOAT_TYPE W) {
  this->_v[0] = X;
  this->_v[1] = Y;
  this->_v[2] = Z;
  this->_v[3] = W;
  return *this;
}

/*!
  Set new coordinates from the given array \a v. Returns a reference to itself.
*/
Vector4 & Vector4::setValues(const FLOAT_TYPE v[4]) {
  this->_v[0] = v[0];
  this->_v[1] = v[1];
  this->_v[2] = v[2];
  this->_v[3] = v[3];
  return *this;
}

/*!
  Returns the dot product between the current vector and \a v.
*/
FLOAT_TYPE Vector4::dot(const Vector4 &v) const {
  return(this->_v[0]*v._v[0] + this->_v[1]*v._v[1] +
	 this->_v[2]*v._v[2] + this->_v[3]*v._v[3]);
}

/*!
  Computes and returns the length of the vector.

  \f$|v|=\sqrt{x^2+y^2+z^2+w^2}.\f$

  \sa norm2(), normalize().
*/
FLOAT_TYPE Vector4::norm() const {
  return(sqrt(_v[0]*_v[0] + _v[1]*_v[1] +
	      _v[2]*_v[2] + _v[3]*_v[3]));
}

/*!
  Computes and returns the squared norm of the vector.

  \f$|v|^2=x^2+y^2+z^2+w^2.\f$

  \sa norm(), normalize().
*/
FLOAT_TYPE Vector4::norm2() const {
  return (_v[0]*_v[0] + _v[1]*_v[1] +
	  _v[2]*_v[2] + _v[3]*_v[3]);
}

/*!
  Normalizes the current vector and returns a reference to
  itself

  If the vector has zero norm, a warning message is posted to
  the error output and the Vector4 is Not modified.

  \sa norm(), norm2().
*/
Vector4 & Vector4::normalize() {
  FLOAT_TYPE Norm = norm();

  if (Norm == 0) {
    return *this;
    cerr << "Warning : Vector4 has zero length!\n";
  }
  else {
    FLOAT_TYPE iNorm = 1.0/Norm;
    this->_v[0] *= iNorm;
    this->_v[1] *= iNorm;
    this->_v[2] *= iNorm;
    this->_v[3] *= iNorm;
  }
  return *this;
}

/*!
  Returns the largest component of the current vector.
*/
FLOAT_TYPE Vector4::max() {
  FLOAT_TYPE max = -1000000.0;
  
  for (int i=0;i<4;i++)
    if (_v[i]>max)
      max = _v[i];

  return max;
}

/*!
  Returns the smallest component of the current vector.
*/
FLOAT_TYPE Vector4::min() {
  FLOAT_TYPE min = 1000000.0;
  
  for (int i=0;i<4;i++)
    if (_v[i]<min)
      min = _v[i];

  return min;
}

/*!
  Returns the mirror vector according to the given axe \a ref_ax.
  The reference axe needs not to be normalized, since this is done
  automatically.
*/
Vector4 Vector4::reflect(const Vector4 &ref_ax) const {
  Matrix4 m, id;
  Vector4 ax = ref_ax;

  id.id();
  ax.normalize();

  m = (2.0*(m.outer(ax,ax))-id);
  return (m*(*this));
}

/*!
  Returns a Vector4 that is the dot product of this
  vector and \a v.
*/
Vector4 Vector4::operator*(const Vector4 &v) const {
  return Vector4(_v[0]*v._v[0],_v[1]*v._v[1],
		 _v[2]*v._v[2],_v[3]*v._v[3]);
}

/*!
  Returns a Vector4 that is this vector scaled by a
  scalar \a s.
*/
/*
Vector4 Vector4::operator*(const FLOAT_TYPE s) const {
  return Vector4(s*_v[0], s*_v[1], s*_v[2], s*_v[3]);
}
*/

/*!
  Returns a Vector4 that is the vector \a v scaled
  by a scalar \a d.
*/
Vector4 operator *(const Vector4 & v, FLOAT_TYPE d) {
  return Vector4(d*v[0], d*v[1], d*v[2], d*v[3]);
}

/*!
  Returns a Vector4, that is the vector \a v
  scaled by a scalar \a s.
*/
Vector4 operator *(FLOAT_TYPE d, const Vector4 & v) {
  return Vector4(d*v[0],d*v[1],d*v[2],d*v[3]);
}

/*!
  Returns a Vector4 whose components are divided
  by \a d.
*/
/*
Vector4 Vector4::operator/(const FLOAT_TYPE d) const {
  FLOAT_TYPE fac = 1.0/d;
  return Vector4(_v[0]*fac, _v[1]*fac,
		 _v[2]*fac, _v[3]*fac);
}
*/

/*!
  Friend function that returns the vector v whose
  components are scaled by d.
*/
Vector4 operator /(const Vector4 & v, FLOAT_TYPE d) {
  FLOAT_TYPE fac = 1.0/d;
  return Vector4(v[0]*fac, v[1]*fac,
		 v[2]*fac, v[3]*fac);
}

/*!
  Sum of the vector with \a v.
*/
Vector4 Vector4::operator+(const Vector4 &v) const {
  return Vector4(this->_v[0]+v[0], this->_v[1]+v[1],
		 this->_v[2]+v[2], this->_v[3]+v[3]);
}

/*!
  Returns the difference between this vector and \a v.
*/
Vector4 Vector4::operator-(const Vector4 &v) const {
  return Vector4(this->_v[0]-v[0], this->_v[1]-v[1],
		 this->_v[2]-v[2], this->_v[3]-v[3]);
}

/*!
  Returns a new Vector4 that points in the opposite direction.
*/
Vector4 Vector4::operator-() const {
  return Vector4(-_v[0], -_v[1], -_v[2], -_v[3]);
}

/*!
  Assign operator. Does the same as the copy constructor.
*/
/*
Vector4& Vector4::operator=(const Vector4 &v) {
  this->_v[0] = v[0];
  this->_v[1] = v[1];
  this->_v[2] = v[2];
  this->_v[3] = v[3];
  
  return(*this);
} 
*/

/*!
  Multiplies this vector's components by \a s and returns
  an instance to itself.
*/
Vector4& Vector4::operator*=(const FLOAT_TYPE s) {
  _v[0] *= s;
  _v[1] *= s;
  _v[2] *= s;
  _v[3] *= s;

  return(*this);
}

/*!
  Divides this vector's components by \a s and returns
  an instance to itself.
*/
Vector4& Vector4::operator/=(const FLOAT_TYPE s) {
  FLOAT_TYPE fac = 1.0/s;
  _v[0] *= fac;
  _v[1] *= fac;
  _v[2] *= fac;
  _v[3] *= fac;

  return (*this);
}

/*!
  Adds the vector \a v to this vector and returns
  an instance to itself.
*/
Vector4& Vector4::operator+=(const Vector4 &v) {
  _v[0] += v[0];
  _v[1] += v[1];
  _v[2] += v[2];
  _v[3] += v[3];
  
  return(*this);
}

/*!
  Substracts the vector \a v from this vector and returns
  an instance to itself.
*/
Vector4& Vector4::operator-=(const Vector4 &v) {
  _v[0] -= v[0];
  _v[1] -= v[1];
  _v[2] -= v[2];
  _v[3] -= v[3];

  return(*this);
}

/*!
  Comparison operator. Returns 1 if the current vector and \a v
  are the same. Returns 0 otherwise.
*/
int Vector4::operator==(const Vector4 &v) const {
  if (_v[0]==v[0] && _v[1]==v[1] &&
      _v[2]==v[2] && _v[3]==v[3]) return 1;
  else return 0;
}

/*!
  If the current vector and \a v are not equal, this
  function returns 1, and 0 otherwise.
*/
int Vector4::operator!=(const Vector4 &v) const {
  return( !(*this == v) );
}

/*!
  Prints the components of this vector in a formatted way
  onto the stream \a out.
*/
void Vector4::print(ostream &out) const {
  out << _v[0] << " " << _v[1] << " "
      << _v[2] << " " << _v[3];
}
