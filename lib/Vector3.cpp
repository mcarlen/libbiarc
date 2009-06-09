/*! \mainpage Documentation for libbiarc

  \section intro_sec Introduction
 
  This is the introduction.
 
  \section install_sec Installation
 
  \subsection step1 Step 1: Opening the box

  etc...

  whole stuff still under heavy construction
 */


/*!
  \defgroup BiarcLibGroup libbiarc
*/

/*!
  \class Vector3 include/Vector3.h include/Vector3.h
  \ingroup BiarcLibGroup
  \brief The Vector3 class is a 3 dimensional Vector class with
  floating point coordinates.

  This class provides storage for a 3 dimensional vector
  aswell as arithmetic operations on vectors, like the dot
  product, the cross product, the length of the vector.
*/
 
 
//
// documentation of inlined methods
//

/*!
  \fn float & Vector3::operator[](const int c)
  Index operator. Returns modifiable x, y or z coordinate of the vector.
 
  \sa getValue() and setValue().
*/
 
/*!
  \fn float Vector3::operator[](const int c) const
  Index operator. Returns x, y or z coordinate of vector.
 
  \sa getValue() and setValue().
*/

/*!
  \fn ostream & Vector3::operator<< (ostream &out, const Vector3 &v)

  Overloaded left shift operator. Returns the Vector \a v as an ostream
  that can be written to a file or to standart output.

  \sa print() operator>>()
*/

/*!
  \fn ostream & Vector3::operator>> (istream &out, Vector3 &v)

  Overloaded right shift operator. Initialize vector \a v
	with istream \a out.

  \sa print() operator<<()
*/


#include "../include/Vector3.h"
#include "../include/Matrix3.h"

/*!
  Constructs an empty vector initialized to zero.

  \sa zero().
*/
Vector3::Vector3() {
 _v[0]=0;_v[1]=0;_v[2]=0;
}

/*!
  Constructs a Vector3 instance from \a x, \a y and \a z.
*/
Vector3::Vector3(const float x, const float y, const float z) {
  _v[0]=x; _v[1]=y; _v[2]=z;
}

/*!
  Constructs a Vector3 with initial values from \a v.
*/
Vector3::Vector3(const float v[3]) {
  _v[0]=v[0];  _v[1]=v[1]; _v[2] = v[2];
}

/*!
  The destructor has nothing to do
*/
Vector3::~Vector3() {}

/*!
  Constructs a copy of \a v.
*/
Vector3::Vector3(const Vector3& v) {
  _v[0]=v[0]; _v[1]=v[1]; _v[2]=v[2];
}

/*!
  Sets all vector components to zero
*/
Vector3 & Vector3::zero() {
  this->_v[0]=0.0;
  this->_v[1]=0.0;
  this->_v[2]=0.0;
  return *this;
}

/*!
  Recover the vector components and put them into \a X, \a Y and \a Z.
*/
void Vector3::getValues(float& X, float& Y, float &Z) const {
  X = _v[0];
  Y = _v[1];
  Z = _v[2];
}

/*!
  Set the vector to \a <X,Y,Z>. Returns a reference to itself.
*/
Vector3 & Vector3::setValues(const float X, const float Y, const float Z) {
  this->_v[0] = X;
  this->_v[1] = Y;
  this->_v[2] = Z;
  return *this;
}

/*!
  Set new coordinates from the given array \a v. Returns a reference to itself.
*/
Vector3 & Vector3::setValues(const float v[3]) {
  this->_v[0] = v[0];
  this->_v[1] = v[1];
  this->_v[2] = v[2];
  return *this;
}

/*!
  Returns the dot product between the current vector and \a v.
*/
float Vector3::dot(const Vector3 &v) const {
  return(this->_v[0]*v._v[0] + this->_v[1]*v._v[1] + this->_v[2]*v._v[2]);
}

/*!
  Returns a vector \a w, that is the result of the cross product between the actual
  vector \a and a vector \a v.

  returns \f$w=a\times v\f$
*/
Vector3 Vector3::cross(const Vector3 &v) const {
  return Vector3(this->_v[1]*v._v[2] - this->_v[2]*v._v[1],
		 this->_v[2]*v._v[0] - this->_v[0]*v._v[2],
		 this->_v[0]*v._v[1] - this->_v[1]*v._v[0]);
}

/*!
  Computes and returns the length of the vector.

  \f$|v|=\sqrt{x^2+y^2+z^2}.\f$

  \sa norm2(), normalize().
*/
float Vector3::norm() const {
  return(sqrt(_v[0]*_v[0] + _v[1]*_v[1] + _v[2]*_v[2]));
}

/*!
  Computes and returns the squared norm of the vector.

  \f$|v|^2=x^2+y^2+z^2.\f$

  \sa norm(), normalize().
*/
float Vector3::norm2() const {
  return (_v[0]*_v[0] + _v[1]*_v[1] + _v[2]*_v[2]);
}

/*!
  Normalizes the current vector and returns a reference to
  itself

  If the vector has zero norm, a warning message is posted to
  the error output and the Vector3 is Not modified.

  \sa norm(), norm2().
*/
Vector3 & Vector3::normalize() {
  float Norm = norm();

  if (Norm == 0) {
    return *this;
    cerr << "Warning : Vector3 has zero length!\n";
  }
  else {
    float iNorm = 1.0/Norm;
    this->_v[0] *= iNorm;
    this->_v[1] *= iNorm;
    this->_v[2] *= iNorm;
  }
  return *this;
}

/*!
  Returns the largest component of the current vector.
*/
float Vector3::max() {
  if( fabs(_v[0]) > fabs(_v[1]) ){
    if( fabs(_v[2]) > fabs(_v[0])){
      return _v[2];
    } else {
      return _v[0];
    }
  } else if( fabs(_v[1]) > fabs(_v[2]) ){
    return _v[0];
  } else {
    return _v[2];
  }
  return 0.0;
}

/*!
  Returns the smallest component of the current vector.
*/
float Vector3::min() {
  float min = 1000000.0;

  for (int i=0;i<3;i++)
    if (_v[i]<min)
      min = _v[i];

  return min;
}



/*!
  Returns the mirror vector according to the given axe \a ref_ax.
  The reference axe needs not to be normalized, since this is done
  automatically.
*/
Vector3 Vector3::reflect(const Vector3 &ref_ax) const {
  Matrix3 m, id;
  Vector3 ax = ref_ax;

  id.id();
  ax.normalize();

  m = (2.0*(m.outer(ax,ax))-id);
  return (m*(*this));
}

/*!
  Rotate the current point at <x,y,z> around the given rotational axes \a axis
  and an angle \a angle. The value for the angle is given in radians.
*/
Vector3 Vector3::rotPtAroundAxis(float angle, Vector3 axis) const {

  Vector3 point = *this;

  // Rotate the point around a circle centered at (0,0,0)
  // Therefore substract the projection of point at axis to get this
  Vector3 proj_point = axis*(point.dot(axis));
  Vector3 direction  = point - proj_point;
  Vector3 tmp;
  
  float x = axis[0], y = axis[1], z = axis[2];
  
  // Calculate the sine and cosine of the angle once
  float cosTheta = (float)cos(angle);
  float sinTheta = (float)sin(angle);
      
  // Find the new x position for the new rotated point
  tmp[0]  = (cosTheta + (1 - cosTheta) * x * x)     * direction[0];
  tmp[0] += ((1 - cosTheta) * x * y - z * sinTheta) * direction[1];
  tmp[0] += ((1 - cosTheta) * x * z + y * sinTheta) * direction[2];
      
  // Find the new y position for the new rotated point
  tmp[1]  = ((1 - cosTheta) * x * y + z * sinTheta) * direction[0];
  tmp[1] += (cosTheta + (1 - cosTheta) * y * y)	* direction[1];
  tmp[1] += ((1 - cosTheta) * y * z - x * sinTheta) * direction[2];
  
  // Find the new z position for the new rotated point
  tmp[2]  = ((1 - cosTheta) * x * z - y * sinTheta) * direction[0];
  tmp[2] += ((1 - cosTheta) * y * z + x * sinTheta) * direction[1];
  tmp[2] += (cosTheta + (1 - cosTheta) * z * z)     * direction[2];

  // Put the point back to the no projectoed location
  tmp += proj_point;

  return tmp;
}

/*!
  Returns a Vector3 that is the dot product of this vector and \a v.
*/
Vector3 Vector3::operator*(const Vector3 &v) const {
  return Vector3(_v[0]*v._v[0],_v[1]*v._v[1],_v[2]*v._v[2]);
}

/*!
  Returns a Vector3 that is this vector scaled by a scalar \a s.
*/
/*
Vector3 Vector3::operator*(const float s) const {
  return Vector3(s*_v[0], s*_v[1], s*_v[2]);
}
*/

/*!
  Returns a Vector3 that is the vector \a v scaled by a scalar \a d.
*/
Vector3 operator*(const Vector3 & v, float d) {
  return Vector3(d*v[0], d*v[1], d*v[2]);
}

/*!
  Returns a Vector3, that is the vector \a v
  scaled by a scalar \a s.
*/
Vector3 operator*(float d, const Vector3 & v) {
  return Vector3(d*v[0],d*v[1],d*v[2]);
}

/*!
  Returns a Vector3 whose components are divided by \a d.
*/
/*
Vector3 Vector3::operator/(const float d) const {
  float fac = 1.0/d;
  return Vector3(_v[0]*fac, _v[1]*fac, _v[2]*fac);
}
*/

/*!
  Friend function that returns the vector v whose components are
  scaled by d.
*/
Vector3 operator /(const Vector3 & v, float d) {
  float fac = 1.0/d;
  return Vector3(v[0]*fac, v[1]*fac, v[2]*fac);
}

/*!
  Sum of the vector with \a v.
*/
Vector3 Vector3::operator+(const Vector3 &v) const {
  return Vector3(this->_v[0]+v[0], this->_v[1]+v[1], this->_v[2]+v[2]);
}

/*!
  Returns the difference between this vector and \a v.
*/
Vector3 Vector3::operator-(const Vector3 &v) const {
  return Vector3(this->_v[0]-v[0], this->_v[1]-v[1], this->_v[2]-v[2]);
}

/*!
  Returns a new Vector3 that points in the opposite direction.
*/
Vector3 Vector3::operator-() const {
  return Vector3(-_v[0], -_v[1], -_v[2]);
}

/*!
  Assign operator. Does the same as the copy constructor.
*/
/*
Vector3& Vector3::operator=(const Vector3 &v) {
  this->_v[0] = v[0];
  this->_v[1] = v[1];
  this->_v[2] = v[2];
  
  return(*this);
} 
*/

/*!
  Multiplies this vector's components by \a s and returns
  an instance to itself.
*/
Vector3& Vector3::operator*=(const float s) {
  _v[0] *= s;
  _v[1] *= s;
  _v[2] *= s;
  
  return(*this);
}

/*!
  Divides this vector's components by \a s and returns
  an instance to itself.
*/
Vector3& Vector3::operator/=(const float s) {
  float fac = 1.0/s;
  _v[0] *= fac;
  _v[1] *= fac;
  _v[2] *= fac;

  return (*this);
}

/*!
  Adds the vector \a v to this vector and returns
  an instance to itself.
*/
Vector3& Vector3::operator+=(const Vector3 &v) {
  _v[0] += v[0];
  _v[1] += v[1];
  _v[2] += v[2];
  
  return(*this);
}

/*!
  Substracts the vector \a v from this vector and returns
  an instance to itself.
*/
Vector3& Vector3::operator-=(const Vector3 &v) {
  _v[0] -= v[0];
  _v[1] -= v[1];
  _v[2] -= v[2];
  
  return(*this);
}

/*!
  Comparison operator. Returns 1 if the current vector and \a v
  are the same. Returns 0 otherwise.
*/
int Vector3::operator==(const Vector3 &v) const {
  if (_v[0]==v[0] && _v[1]==v[1] && _v[2]==v[2]) return 1;
  else return 0;
}

/*!
  If the current vector and \a v are not equal, this function returns 1,
  and 0 otherwise.
*/
int Vector3::operator!=(const Vector3 &v) const {
  return( !(*this == v) );
}

/*!
  Prints the components of this vector in a formatted way
  onto the stream \a out.
*/
void Vector3::print(ostream &out) const {
  out << _v[0] << " " << _v[1] << " " << _v[2];
}
