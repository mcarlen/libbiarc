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
#include "../include/utils/qr.h"

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

  // Compute Principal Axis
  // Suppose the knot to be of uniform density => dm = cte
  Matrix3 ev;
  curve.principalAxis(ev);

 /* Not returned by principalAxis()
 cout << "Eigenvalues\n===========\n"
       << r[0][0] << ", "
       << r[1][1] << ", "
       << r[2][2] << endl << endl;
 */
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
