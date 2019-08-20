/*!
  \class Matrix3 include/Matrix3.h include/Matrix3.h
  \ingroup BiarcLibGroup
  \brief The Matrix3 class is a 3x3 dimensional Matrix class with
  FLOAT_TYPEing point entries.

  This class provides storage for a 3x3 FLOAT_TYPEing point table, representing
  a matrix. It is possible to compute determinant, inverse, transpose and
  other standart matrix operations.

  A matrix is given by 3 Vector3 column vectors \f$v_0,v_1,v_2\f$ and
  has the form
  \f$M=\left(
  \begin{array}{ccc}v_{00} & v_{10} & v_{20} \\
                    v_{01} & v_{11} & v_{21} \\
		    v_{02} & v_{12} & v_{22}
  \end{array}
  \right)
  \f$

  Caution : When accessing a matrix element by M[i][j], i is the
  column index, and j the row index! In the linear algebra literature
  most of the time this convention is the inverse. The structure is
  chosen like this such that a change of basis from a coordinate
  system A to a coordinate system B is given by the matrix M, composed
  by the basis column vectors of the final coordinate system.

*/
 
//
// documentation of inlined methods
//
 
/*!
  \fn Vector3 & Matrix3::operator[](int n)
  Index operator. Returns one of the three matrix columns as a Vector3 instance.
 
  \sa getOne(), getAll(), setOne(), setAll().
*/
 
/*!
  \fn const Vector3 &Matrix3::operator[](int n) const
  Index operator. Returns one of the three matrix columns as a Vector3 instance.
 
  \sa getOne(), getAll(), setOne(), setAll().
*/

/*!
  \fn ostream & Matrix3::operator << (ostream &out, const Matrix3 &m)

  Overloaded left shift operator. Returns the Matrix3 \a m as an ostream
  object that can be written to a file or to standart output.

  \sa print()
*/

#include "../include/Matrix3.h"

/*!
  The default constructor initializes all matrix elements to zero.
 */
Matrix3::Matrix3() {
  _v[0].zero();
  _v[1].zero();
  _v[2].zero();
}

/*!
  Constructs a Matrix3 given three row vectors \a v0, \a v1 and \a v2.
 */
Matrix3::Matrix3(const Vector3 &v0,const Vector3 &v1,const Vector3 &v2) {
  _v[0] = v0 ; _v[1] = v1 ; _v[2] = v2;
}

/*!
  Constructs a Matrix3 from 9 FLOAT_TYPEs. Given in row-first order.
*/
Matrix3::Matrix3(const FLOAT_TYPE &x00, const FLOAT_TYPE &x01, const FLOAT_TYPE &x02,
                 const FLOAT_TYPE &x10, const FLOAT_TYPE &x11, const FLOAT_TYPE &x12,
                 const FLOAT_TYPE &x20, const FLOAT_TYPE &x21, const FLOAT_TYPE &x22) {
  _v[0] = Vector3(x00,x10,x20);
  _v[1] = Vector3(x01,x11,x21);
  _v[2] = Vector3(x02,x12,x22);
}

/*!
  Set all matrix elements to zero. Returns an instance to itself.
*/
Matrix3 & Matrix3::zero() {
  _v[0].zero();
  _v[1].zero();
  _v[2].zero();
  return *this;
}

/*!
  Fill matrix column \a c with the components of the vector \a v.
*/
void Matrix3::setOne(int c, const Vector3 &v) { _v[c] = v; }

/*!
  Fill matrix column \a c from top to bottom with the values \a v1 to \a v3.
*/
void Matrix3::setOne(int c, const FLOAT_TYPE v1, const FLOAT_TYPE v2, const FLOAT_TYPE v3) {
  _v[c] = Vector3(v1,v2,v3);
}


/*!
  Reset the matrix by columns \a v1 to \a v3.
*/
void Matrix3::setAll(const Vector3 &v1, const Vector3 &v2, const Vector3 &v3) {
  _v[0] = v1; _v[1] = v2; _v[2] = v3;
}

/*!
  Fill the matrix with the array of Vector3 \a v.
*/
void Matrix3::setAll(const Vector3 *v) {
  for (int i=0;i<3;i++) _v[i] = *(v+i);
}

/*!
  Returns matrix column \a c as a Vector3 instance.
*/
Vector3 Matrix3::getOne(int c) { return _v[c]; }

/*!
  Put the matrix columns into the vectors \a v1 to \a v3.
*/
void Matrix3::getAll(Vector3 &v1, Vector3 &v2, Vector3 &v3) {
  v1 = _v[0]; v2 = _v[1]; v3 = _v[2];
}

/*!
  Set the current matrix to the identity and return a reference to
  itself.
*/
Matrix3 & Matrix3::id() {
  setAll(Vector3(1.0,0.0,0.0),
	 Vector3(0.0,1.0,0.0),
	 Vector3(0.0,0.0,1.0));

  return *this;
}

/*!
  Set the current matrix to its transpose and return a reference to
  itself.

  Columns become rows and rows become the columns. I.e, the transpose
  of a matrix \f$M=\f$ is

  \f$M^t=\left(
  \begin{array}{ccc}v_{00} & v_{01} & v_{02} \\
                    v_{10} & v_{11} & v_{12} \\
		    v_{20} & v_{21} & v_{22}
  \end{array}
  \right)
  \f$

*/
Matrix3 & Matrix3::transpose() {
  Vector3 tmp[3];
  tmp[0] = Vector3((*this)[0][0],(*this)[1][0],(*this)[2][0]);
  tmp[1] = Vector3((*this)[0][1],(*this)[1][1],(*this)[2][1]);
  tmp[2] = Vector3((*this)[0][2],(*this)[1][2],(*this)[2][2]);

  _v[0]=tmp[0];
  _v[1]=tmp[1];
  _v[2]=tmp[2];

  return *this;
}

/*!
  Computes and returns the determinant of the Matrix3.
*/
FLOAT_TYPE Matrix3::det() {
  return
    (*this)[0][0] * (*this)[1][1] * (*this)[2][2]
    + (*this)[1][0] * (*this)[2][1] * (*this)[0][2]
    + (*this)[2][0] * (*this)[0][1] * (*this)[1][2]
    - (*this)[0][2] * (*this)[1][1] * (*this)[2][0]
    - (*this)[1][2] * (*this)[2][1] * (*this)[0][0]
    - (*this)[2][2] * (*this)[0][1] * (*this)[1][0];
}

/*!
  Sets the current matrix to its inverse and return a reference to
  itself.

  Given the matrix \f$A\f$, its inverse is the matrix B, satisfying
  the relation

  \f$A\cdot B = id\f$,

  where \f$id\f$ is the identity matrix.

  If the matrix is hardly invertible, i.e. its determinant is close
  to zero, then a warning is posted to the error channel and the matrix
  itself is not changed.
*/
Matrix3 & Matrix3::inv() {
  FLOAT_TYPE d = det();
  FLOAT_TYPE x, y, z;

  if(fabs(d) < 0.0001) {
    cerr << "Matrix non or hardly invertible" << endl;
    return *this;
  }
  
  x = (*this)[1][1]*(*this)[2][2] - (*this)[1][2]*(*this)[2][1];
  y = (*this)[2][1]*(*this)[0][2] - (*this)[2][2]*(*this)[0][1];
  z = (*this)[0][1]*(*this)[1][2] - (*this)[0][2]*(*this)[1][1];
  _v[0] = Vector3(x/d, y/d, z/d);
  
  x = (*this)[1][2]*(*this)[2][0] - (*this)[1][0]*(*this)[2][2];
  y = (*this)[2][2]*(*this)[0][0] - (*this)[2][0]*(*this)[0][2];
  z = (*this)[0][2]*(*this)[1][0] - (*this)[0][0]*(*this)[1][2];
  _v[1] = Vector3(x/d, y/d, z/d);
  
  x = (*this)[1][0]*(*this)[2][1] - (*this)[1][1]*(*this)[2][0];
  y = (*this)[2][0]*(*this)[0][1] - (*this)[2][1]*(*this)[0][0];
  z = (*this)[0][0]*(*this)[1][1] - (*this)[0][1]*(*this)[1][0];
  _v[2] = Vector3(x/d, y/d, z/d);

  return *this;
}

/*!
  Computes the outer product between a vector \a a and a
  vector \a b and stores the result in the current Matrix3 object.
  
  Formally we have the product between a column vector \a a with
  a row vector \a b.

  
  \f$M=\left(
  \begin{array}{c}a_0 \\
                  a_1 \\
		  a_2
  \end{array}
  \right)
  \cdot
  \left(
  \begin{array}{ccc}b_0 & b_1 & b_2
  \end{array}
  \right)\f$

*/
Matrix3& Matrix3::outer(const Vector3 &a,const Vector3 &b) {
  (*this)[0] = a; (*this)[0] *= b[0];
  (*this)[1] = a; (*this)[1] *= b[1];
  (*this)[2] = a; (*this)[2] *= b[2];
  return (*this);
}

/*!
  Constructs an skew-symmetric matrix from a vector \a v of the
  form

  \f$M=\left(
  \begin{array}{ccc}0 & -v_3 & v_2 \\
                    v_3 & 0 & -v_1 \\
		    -v_2 & v_1 & 0
  \end{array}
  \right)
  \f$

  Applied to a vector, this matrix acts like a cross product
  between \a v and some other vector \a w. Therefore

  \f$M\cdot w = v\times w\f$

*/
Matrix3& Matrix3::vecCross(const Vector3 &v) {
  _v[0] = Vector3(0.0,v[2],-v[1]);
  _v[1] = Vector3(-v[2],0.0,v[0]);
  _v[2] = Vector3(v[1],-v[0],0.0);

  return *this;
}

/*!
  Construct a Cayleigh rotation matrix, defined by




  // Cay Rotation Matrix "Cay(v)", where v are the components
  // of the rot axis and |v| is magnitude of rotation

  

*/
Matrix3& Matrix3::cay(const Vector3 &v) {
  FLOAT_TYPE a = 1/(1 + v.norm2()/4);
  Matrix3 vcross, tmp, m;
  vcross.vecCross(v);
  tmp = vcross*vcross;
  id();
  m = vcross+0.5*tmp;
  m*=a;
  (*this)+= m;
  return (*this);
}


/*!
  Sets the current matrix to a rotation matrix.
  Where the rotation is in the positive trigonometric direction
  about the axis v by an angle alpha.
*/
Matrix3& Matrix3::rotAround(const Vector3 &v, FLOAT_TYPE alpha) {
  Vector3 ax = -v; ax.normalize();
  FLOAT_TYPE x  = ax[0], y  = ax[1], z  = ax[2];
  FLOAT_TYPE sa = sin(alpha), ca = cos(alpha);
  _v[0][0]= ca + x*x * (1.-ca);
  _v[0][1]=  x*y*(1.-ca) - z*sa;
  _v[0][2]= z*x*(1.-ca) + y*sa;
  _v[1][0]= x*y*(1.-ca) + z*sa;
  _v[1][1]= ca + y*y * (1.-ca);
  _v[1][2]= z*y*( 1.-ca) - x*sa;
  _v[2][0]= x*z* (1.-ca) - y*sa;
  _v[2][1]= y*z* (1.-ca) + x*sa;
  _v[2][2]= ca + z*z*(1.-ca);
  return (*this);
}

/*!
  Multiplies the current matrix on the right hand side by \a m and
  returns the result as a new Matrix3 instance.
*/
Matrix3 Matrix3::operator*(const Matrix3 &m) {
  Matrix3 tmp;
  for (int i=0;i<3;i++) {
    for (int j=0;j<3;j++) {
      for (int k=0;k<3;k++)
	tmp[j][i]+=(_v[k][i]*m[j][k]);
    }
  }
  return tmp;
}

/*!
  Applies the current matrix on the left side of a column vector \a v
  and returns a vector with the result.
*/
Vector3 Matrix3::operator*(const Vector3 &v) {
  FLOAT_TYPE a[3];
  for (int i=0;i<3;i++) {
    a[i] = 0;
    for (int j=0;j<3;j++) {
      a[i] += (v[j]*_v[j][i]);
    }
  }
  return Vector3(a[0],a[1],a[2]);
}

/*!
  Multiplies every element of the matrix \a m by a factor of \a d.
  and returns a Matrix3 object.
*/
Matrix3 operator*(const Matrix3 &m, FLOAT_TYPE d) {
  Matrix3 tmp = m;
  return Matrix3(d*tmp[0],d*tmp[1],d*tmp[2]);
}

/*!
  Multiplies every element of the matrix \a m by a factor of \a d.
  and returns a Matrix3 object.
*/
Matrix3 operator*(FLOAT_TYPE d, const Matrix3 &m) {
  Matrix3 tmp = m;
  return Matrix3(d*tmp[0],d*tmp[1],d*tmp[2]);
}

/*!
  Divides every element of the matrix \a m by \a d.
  and returns a Matrix3 object.
*/
Matrix3 operator/(const Matrix3 &m, const FLOAT_TYPE d) {
  Matrix3 tmp = m;
  return Matrix3(tmp[0]/d, tmp[1]/d, tmp[2]/d);
}

/*!
  Matrix summation. Returns a Matrix3 instance.
*/
Matrix3 Matrix3::operator+(const Matrix3 &m) const {
  return Matrix3(_v[0]+m[0],_v[1]+m[1],_v[2]+m[2]);
}

/*!
  Matrix substraction. Returns a Matrix3 instance.
*/
Matrix3 Matrix3::operator-(const Matrix3 &m) const {
  return Matrix3(_v[0]-m[0],_v[1]-m[1],_v[2]-m[2]);
}

/*!
  Matrix negation. Returns a Matrix3 instance where
  all the elements have now switched their sign.
*/
Matrix3 Matrix3::operator-() const {
  return Matrix3(-_v[0],-_v[1],-_v[2]);
}

/*!
  Assign operator. Makes a copy of \a m. Returns an instance to itself.
*/
Matrix3& Matrix3::operator=(const Matrix3 &m) {
  _v[0] = m[0];
  _v[1] = m[1];
  _v[2] = m[2];
  return (*this);
}

/*!
  Adds the matrix \a m to the current one. Returns an instance to itself.
*/
Matrix3& Matrix3::operator+=(const Matrix3 &m) {
  for(int i=0;i<3;i++)
    (*this)[i]+=m[i];
  return *this;
}

/*!
  Substracts the matrix \a m from the current one. Returns an
  instance to itself.
*/
Matrix3& Matrix3::operator-=(const Matrix3 &m) {
  for(int i=0;i<3;i++)
    (*this)[i]-=m[i];
  return *this;
}

/*!
  Divides all the elements of the current matrix by \a s and returns\
  an instance to itself.
*/
Matrix3& Matrix3::operator/=(const FLOAT_TYPE s) {
  for(int i=0;i<3;i++)
    (*this)[i]/=s;
  return *this;
}

/*!
  Multiplies all the elements of the current matrix by \a s and returns\
  an instance to itself.
*/
Matrix3& Matrix3::operator*=(FLOAT_TYPE d) {
  for (int i=0;i<3;i++)
    (*this)[i]*=d;
  return *this;
}

/*!
  Compare the current matrix to \a m. If all the elements are
  the same, the function return 1, 0 otherwise.
*/
int Matrix3::operator==(const Matrix3 &m) const {
  if ((_v[0]==m[0])&(_v[1]==m[1])&(_v[2]==m[2])) return 1;
  else
    return 0;
}

/*!
  Compare the current matrix to \a m. Returns 1 if they are different.
  0 if they are the same.
*/
int Matrix3::operator!=(const Matrix3 &m) const {
  return !(*this==m) ;
}

/*!
  Prints the components of this matrix in a formated way
  onto the stream \a out.
*/
void Matrix3::print(ostream &out) const {
  out << "[";
  for (int i=0;i<3;i++) {
    for (int j=0;j<3;j++) {
      if ((i!=0)&&!(j>0)) out << " ";
      out << (_v[j][i]);
      if (j!=2) out << " ";
    }
    if (i!=2) out << endl;
  }
  out << "]";
}
