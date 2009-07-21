/*!
   Compute the QR decomposition of a matrix
   by the Gram-Schmidt Algo.

   At the end of the routine the matrix a is q!
*/
#include "../include/utils/qr.h"

void qr_decomp(Matrix3 &a, Matrix3 &r) {
  for (int j=0;j<3;++j) {
    r[j][j] = a[j].norm();
    if (r[j][j] < qr_eps) {
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

