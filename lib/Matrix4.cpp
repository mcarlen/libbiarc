/*!
  \class Matrix4 include/Matrix4.h include/Matrix4.h
  \ingroup BiarcLibGroup
  \brief The Matrix4 class is a 4x4 dimensional Matrix class with
  floating point entries.

  This class provides storage for a 4x4 floating point table, representing
  a matrix. It is possible to compute determinant, inverse, transpose and
  other matrix operations in 4-dimensions.

  A matrix is given by 4 Vector4 column vectors \f$v_0,v_1,v_2,v_3\f$ and
  has the form
  \f$M=\left(
  \begin{array}{cccc}v_{00} & v_{10} & v_{20} & v_{30} \\
                     v_{01} & v_{11} & v_{21} & v_{31} \\
		     v_{02} & v_{12} & v_{22} & v_{32} \\
		     v_{03} & v_{13} & v_{23} & v_{33}
  \end{array}
  \right)
  \f$

  Caution : When accessing a matrix element by M[i][j], i is the
  column index, and j the row index! In the linear algebra literature
  most of the time this convention is the inverse. The structure is
  chosen like this such that a change of basis from a coordinate
  system A to a coordinate system B is given by the matrix M, composed
  by the basis column vectors of the final coordinate system.

  \sa class Vector4

*/
 
//
// documentation of inlined methods
//
 
/*!
  \fn Vector4 & Matrix4::operator[](int n)
  Index operator. Returns one of the three matrix columns as a Vector4 instance.
 
  \sa getOne(), getAll(), setOne(), setAll().
*/
 
/*!
  \fn const Vector4 &Matrix4::operator[](int n) const
  Index operator. Returns one of the three matrix columns as a Vector4 instance.
 
  \sa getOne(), getAll(), setOne(), setAll().
*/

/*!
  \fn ostream & Matrix4::operator << (ostream &out, const Matrix4 &m)

  Overloaded left shift operator. Returns the Matrix4 \a m as an ostream
  object that can be written to a file or to standart output.

  \sa print()
*/

#include "../include/Matrix4.h"

/*!
  The default constructor initializes all matrix elements to zero.
 */
Matrix4::Matrix4() {
  _v[0].zero();
  _v[1].zero();
  _v[2].zero();
  _v[3].zero();
}

/*!
  Constructs a Matrix4 given three row vectors \a v0, \a v1,
  \a v2 and \a v3.
 */
Matrix4::Matrix4(const Vector4 &v0,const Vector4 &v1,
		 const Vector4 &v2,const Vector4 &v3) {
  _v[0] = v0 ; _v[1] = v1 ; _v[2] = v2 ; _v[3] = v3;
}

/*!
  Set all matrix elements to zero. Returns an instance to itself.
*/
Matrix4 & Matrix4::zero() {
  _v[0].zero();
  _v[1].zero();
  _v[2].zero();
  _v[3].zero();
  return *this;
}

/*!
  Fill matrix column \a c with the components of the vector \a v.
*/
void Matrix4::setOne(int c, const Vector4 &v) { _v[c] = v; }

/*!
  Fill matrix column \a c from top to bottom with the values \a v1 to \a v4.
*/
void Matrix4::setOne(int c, const float v1, const float v2,
		     const float v3, const float v4) {
  _v[c] = Vector4(v1,v2,v3,v4);
}

/*!
  Reset the matrix by columns \a v1 to \a v4.
*/
void Matrix4::setAll(const Vector4 &v1, const Vector4 &v2,
		     const Vector4 &v3, const Vector4 &v4) {
  _v[0] = v1; _v[1] = v2; _v[2] = v3; _v[3] = v4;
}

/*!
  Fill the matrix with the array of Vector4 \a v.
*/
void Matrix4::setAll(const Vector4 *v) {
  for (int i=0;i<4;i++) _v[i] = *(v+i);
}

/*!
  Returns matrix column \a c as a Vector4 instance.
*/
Vector4 Matrix4::getOne(int c) { return _v[c]; }

/*!
  Put the matrix columns into the vectors \a v1 to \a v4.
*/
void Matrix4::getAll(Vector4 &v1, Vector4 &v2,
		     Vector4 &v3, Vector4 &v4) {
  v1 = _v[0]; v2 = _v[1]; v3 = _v[2]; v4 = _v[3];
}

/*!
  Set the current matrix to the identity and return a reference to
  itself.
*/
Matrix4 & Matrix4::id() {
  setAll(Vector4(1.0,0.0,0.0,0.0),
	 Vector4(0.0,1.0,0.0,0.0),
	 Vector4(0.0,0.0,1.0,0.0),
	 Vector4(0.0,0.0,0.0,1.0));

  return *this;
}

/*!
  Set the current matrix to its transpose and return a reference to
  itself.

  Columns become rows and rows become the columns. I.e, the transpose
  of a matrix \f$M=\f$ is

  \f$M^t=\left(
  \begin{array}{cccc}v_{00} & v_{01} & v_{02} & v_{03} \\
                     v_{10} & v_{11} & v_{12} & v_{13} \\
		     v_{20} & v_{21} & v_{22} & v_{23} \\
		     v_{30} & v_{31} & v_{32} & v_{33}
  \end{array}
  \right)
  \f$

*/
Matrix4 & Matrix4::transpose() {
  Vector4 tmp[4];
  tmp[0] = Vector4((*this)[0][0],(*this)[1][0],(*this)[2][0],(*this)[3][0]);
  tmp[1] = Vector4((*this)[0][1],(*this)[1][1],(*this)[2][1],(*this)[3][1]);
  tmp[2] = Vector4((*this)[0][2],(*this)[1][2],(*this)[2][2],(*this)[3][2]);
  tmp[3] = Vector4((*this)[0][3],(*this)[1][3],(*this)[2][3],(*this)[3][3]);

  _v[0]=tmp[0];
  _v[1]=tmp[1];
  _v[2]=tmp[2];
  _v[3]=tmp[3];

  return *this;
}

/*!
  Computes and returns the determinant of the Matrix4.

  \sa sub(),inv(),adjoint()
*/
float Matrix4::det() {

  float Det = 0;
  for (int i=0;i<4;i++) {
    if (i%2)
      Det -= _v[i][0]*this->sub(0,i).det();
    else
      Det += _v[i][0]*this->sub(0,i).det();
  }
  return Det;
}

/*!
  Returns a Matrix3 object that is the current Matrix4
  without line \a r and column \a c. Let's take as an
  example a matrix M. A call to M.sub(2,1), where the first argument
  is the line 3 and the second one the column 2 (since we start
  counting at 0), yields the following Matrix3 :

  \f$M^{4x4}=\left(
  \begin{array}{c|ccc}
  v_{00}\quad & \!\!\!\!\!v_{10} & v_{20} & v_{30} \\
  v_{01}\quad & \!\!\!\!\!v_{11} & v_{21} & v_{31} \\[3mm]
  \hline
  \vspace{-7.5mm}
  v_{03}\quad & \!\!\!\!\!v_{13} & v_{23} & v_{33} \\
  v_{02}\quad & \!\!\!\!\!v_{12} & v_{22} & v_{32} \\
  \end{array}
  \right)

  \quad\Rightarrow\quad
  M^{3x3}=\left(
  \begin{array}{ccc}v_{00} & v_{20} & v_{30} \\
                    v_{01} & v_{21} & v_{31} \\
		    v_{03} & v_{23} & v_{33}
  \end{array}
  \right)
  \f$
*/
Matrix3 Matrix4::sub(const int r, const int c) {

  Matrix3 m;
  int row = 0, col = 0;
  for (int i=0;i<4;i++) {
    if (i!=r) {
      for (int j=0;j<4;j++) {
	if (j!=r)
	  m[col++][row] = _v[j][i];
      }
      row++;
    }
  }
  return m;
}

/*!
  Computes the adjoint matrix A. Replaces the currently stored
  matrix by A and returns a reference to it.

  The transposition of the cofactor matrix is the adjoint matrix.
  A cofactor is given by :

  \f$cof_{ij}=(-1)^{i+j}det\:M_{ij}\f$,

  where \f$M_{ij}\f$ is the matrix obtained by erasing line \f$i\f$
  and column \f$j\f$.

  \sa inv(), det(), sub()
*/
Matrix4 & Matrix4::adjoint() {

  Matrix4 tmp;
  // Careful : the sub() has row major order. the Matrix4 struct
  // however has column major order thus the transposition of the
  // matrix is here done immediately!
  for (int i=0;i<4;i++)
    for (int j=0;j<4;j++) {
      if (i%2)
	tmp[i][j] = -this->sub(i,j).det();
      else
	tmp[i][j] = this->sub(i,j).det();
    }
  *this = tmp;
  return *this;

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

  \sa adjoint(),sub(),det(),Matrix3::inv()

*/
Matrix4 & Matrix4::inv() {

  float Det ;
  Det = this->det();

  if ( fabs( Det ) < 0.0001) {
    cout << "MESSAGE : Matrix4::inv() : Matrix is singular!";
    return *this;
  }

  this->adjoint();
  for (int i=0; i<4; i++)
    for(int j=0; j<4; j++)
      _v[i][j] /= Det;

  return *this;
}

/*!
  Computes the outer product between a vector \a a and a
  vector \a b and stores the result in the current Matrix4 object.
  
  Formally we have the product between a column vector \a a with
  a row vector \a b.

  
  \f$M=\left(
  \begin{array}{c}a_0 \\
                  a_1 \\
		  a_2 \\
		  a_3 
  \end{array}
  \right)
  \cdot
  \left(
  \begin{array}{cccc}b_0 & b_1 & b_2 & b_3
  \end{array}
  \right)\f$

*/
Matrix4& Matrix4::outer(const Vector4 &a,const Vector4 &b) {
  (*this)[0] = a; (*this)[0] *= b[0];
  (*this)[1] = a; (*this)[1] *= b[1];
  (*this)[2] = a; (*this)[2] *= b[2];
  (*this)[3] = a; (*this)[3] *= b[3];
  return (*this);
}

/*!
  Multiplies the current matrix on the right hand side by \a m and
  returns the result as a new Matrix4 instance.
*/
Matrix4 Matrix4::operator*(const Matrix4 &m) {
  Matrix4 tmp;
  for (int i=0;i<4;i++) {
    for (int j=0;j<4;j++) {
      for (int k=0;k<4;k++)
	tmp[j][i]+=(_v[k][i]*m[j][k]);
    }
  }
  return tmp;
}

/*!
  Applies the current matrix on the left side of a column vector \a v
  and returns a vector with the result.
*/
Vector4 Matrix4::operator*(const Vector4 &v) {
  float a[4];
  for (int i=0;i<4;i++) {
    a[i] = 0;
    for (int j=0;j<4;j++) {
      a[i] += (v[j]*_v[j][i]);
    }
  }
  return Vector4(a[0],a[1],a[2],a[3]);
}

/*!
  Multiplies every element of the matrix \a m by a factor of \a d.
  and returns a Matrix4 object.
*/
Matrix4 operator*(const Matrix4 &m, float d) {
  return Matrix4(d*m[0],d*m[1],d*m[2],d*m[3]);
}

/*!
  Multiplies every element of the matrix \a m by a factor of \a d.
  and returns a Matrix4 object.
*/
Matrix4 operator*(float d, const Matrix4 &m) {
  return Matrix4(d*m[0],d*m[1],d*m[2],d*m[3]);
}

/*!
  Divides every element of the matrix \a m by \a d.
  and returns a Matrix4 object.
*/
Matrix4 operator/(const Matrix4 &m, float d) {
  return Matrix4(m[0]/d, m[1]/d, m[2]/d, m[3]/d);
}

/*!
  Matrix summation. Returns a Matrix4 instance.
*/
Matrix4 Matrix4::operator+(const Matrix4 &m) const {
  return Matrix4(_v[0]+m[0],_v[1]+m[1],
		 _v[2]+m[2],_v[3]+m[3]);
}

/*!
  Matrix substraction. Returns a Matrix4 instance.
*/
Matrix4 Matrix4::operator-(const Matrix4 &m) const {
  return Matrix4(_v[0]-m[0],_v[1]-m[1],
		 _v[2]-m[2],_v[3]-m[3]);
}

/*!
  Matrix negation. Returns a Matrix4 instance where
  all the elements have now switched their sign.
*/
Matrix4 Matrix4::operator-() const {
  return Matrix4(-_v[0],-_v[1],-_v[2],-_v[3]);
}

/*!
  Assign operator. Makes a copy of \a m. Returns an instance to itself.
*/
Matrix4& Matrix4::operator=(const Matrix4 &m) {
  _v[0] = m[0];
  _v[1] = m[1];
  _v[2] = m[2];
  _v[3] = m[3];
  return (*this);
}

/*!
  Adds the matrix \a m to the current one. Returns an instance to itself.
*/
Matrix4& Matrix4::operator+=(const Matrix4 &m) {
  for(int i=0;i<4;i++)
    (*this)[i]+=m[i];
  return *this;
}

/*!
  Substracts the matrix \a m from the current one. Returns an
  instance to itself.
*/
Matrix4& Matrix4::operator-=(const Matrix4 &m) {
  for(int i=0;i<4;i++)
    (*this)[i]-=m[i];
  return *this;
}

/*!
  Divides all the elements of the current matrix by \a s and returns\
  an instance to itself.
*/
Matrix4& Matrix4::operator/=(const float s) {
  for(int i=0;i<4;i++)
    (*this)[i]/=s;
  return *this;
}

/*!
  Multiplies all the elements of the current matrix by \a s and returns\
  an instance to itself.
*/
Matrix4& Matrix4::operator*=(float d) {
  for (int i=0;i<4;i++)
    (*this)[i]*=d;
  return *this;
}

/*!
  Compare the current matrix to \a m. If all the elements are
  the same, the function return 1, 0 otherwise.
*/
int Matrix4::operator==(const Matrix4 &m) const {
  if ((_v[0]==m[0])&&(_v[1]==m[1])&&
      (_v[2]==m[2])&&(_v[3]==m[3])) return 1;
  else
    return 0;
}

/*!
  Compare the current matrix to \a m. Returns 1 if they are different.
  0 if they are the same.
*/
int Matrix4::operator!=(const Matrix4 &m) const {
  return !(*this==m) ;
}

/*!
  Prints the components of this matrix in a formated way
  onto the stream \a out.
*/
void Matrix4::print(ostream &out) const {
  out << "[";
  for (int i=0;i<4;i++) {
    for (int j=0;j<4;j++) {
      if (i!=0&!j>0) out << " ";
      out << (_v[j][i]);
      if (j!=3) out << " ";
    }
    if (i!=3) out << endl;
  }
  out << "]";
}
