/*!
  \file inertiatensor.cpp
  \ingroup ToolsGroup
  \brief Compute the inertia tensor and principal axes.

  Compute the inertia tensor of the curve (assuming uniform mass
  distribution of the curve). Then it prints out the principal
  axes of the given curve.
*/

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include "../include/Curve.h"

/* Compute the QR decomposition of a matrix
   by the Gram-Schmidt Algo.

   At the end of the routine the matrix a is q!
*/
const float eps = 1e-6;
void qr_decomp(Matrix3 &a, Matrix3 &r) {
  for (int j=0;j<3;++j) {
    r[j][j] = a[j].norm();
    if (r[j][j] < eps) {
      cout << "Linearly dependent columns!\n";
      a.zero(); r.zero();
      return;
    }
    for (int i=0;i<3;++i)
      a[j][i] /= r[j][j];
    for (int k=j+1;k<3;++k) {
      r[k][j] = a[j].dot(a[k]);
      for (int i=0;i<3;++i) {
        a[k][i] -= a[j][i]*r[k][j];
      }
    }
  }
}

int main(int argc, char **argv) {

  if (argc!=2) {
    cerr << "Usage : "<<argv[0]<<" <pkf in>\n";
    exit(0);
  }

  Curve<Vector3> curve(argv[1]);
  if (curve.nodes()<1) {
    cout << "Problem reading curve " << argv[1] << endl;
    exit(1);
  }

  // Compute Inertia Tensor
  // Suppose the knot to be of uniform density => dm = cte
  Vector3 p;
  Matrix3 m,r,ev;

  ev.id();
  m.zero();

  vector<Biarc<Vector3> >::iterator it;
  for (it=curve.begin();it!=curve.end();++it) {
    p = it->getPoint();
    // Diagonal terms
    m[0][0] += p[1]*p[1]+p[2]*p[2];
    m[1][1] += p[0]*p[0]+p[2]*p[2];
    m[2][2] += p[0]*p[0]+p[1]*p[1];
    // Off diagonal
    m[0][1] -= p[0]*p[1];
    m[0][2] -= p[0]*p[2];
    m[1][2] -= p[1]*p[2];
  }
  m[1][0] = m[0][1];
  m[2][0] = m[0][2];
  m[2][1] = m[1][2];

  cout << "Inertia tensor\n===============\n";
  cout << m << endl << endl;

  // Compute the principal axes
  // using the QR Algorithm to find the eigenvalues
  // and eigenvectors of a matrix.
  int iter = 0;
  while((fabsf(m[0][1])+fabsf(m[0][2])+fabsf(m[1][2])+
         fabsf(m[1][0])+fabsf(m[2][0])+fabsf(m[2][1]))>eps) {
    qr_decomp(m,r);
    ev = ev*m;
    m  = r*m;
    ++iter;
  }

  cout << "Finished after " << iter << " iterations." << endl << endl;
  cout << "Eigenvalues\n===========\n"
       << r[0][0] << ", "
       << r[1][1] << ", "
       << r[2][2] << endl << endl;

  cout << "Eigenvectors\n============\n" << ev << endl;

  cout << "\npkfrender convenience :\n";
  for (int i=0;i<3;++i)
    for (int j=0;j<3;++j)
      cout << ev[j][i] << " ";
  cout << endl;

  for (int i=0;i<3;++i)
    for (int j=0;j<3;++j)
      cout << ev[(j+1)%3][i] << " ";
  cout << endl;

  for (int i=0;i<3;++i)
    for (int j=0;j<3;++j)
      cout << ev[(j+2)%3][i] << " ";
  cout << endl;

  return 0;

}

#endif // DOXYGEN_SHOULD_SKIP_THIS
