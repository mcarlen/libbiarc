#ifndef __UTILS_QR__
#define __UTILS_QR__

/* Compute the QR decomposition of a matrix
   by the Gram-Schmidt Algo.

   At the end of the routine the matrix a is q!
*/
#include "../Matrix3.h"
const FLOAT_TYPE qr_eps = 1e-6;
void qr_decomp(Matrix3 &a, Matrix3 &r);

#endif // __UTILS_QR__
