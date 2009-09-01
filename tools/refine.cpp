/*!
  \file refine.cpp
  \ingroup ToolsGroup
  \brief Refine/Resample a part of an open or closed PKF curve.
*/

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include "../include/Curve.h"

int main(int argc, char **argv) {

  if (argc!=7) {
    cerr << "Usage : "<<argv[0]
	 << " <closed=1;open=0> <n1 n2 N> <pkf in> <pkf out>\n\n";
    cerr << "Refine the open/closed curve between "
	 << "node n1 and n n2 with N points.\n";
    cerr << "Not more than one component supported!\n";
    exit(0);
  }

  Curve<Vector3> curve(argv[5]);

  int first = atoi(argv[2]), last = atoi(argv[3]);
  int N = atoi(argv[4]);
  
  if (atoi(argv[1]))
    curve.link();
  
  curve.make_default();

  cout << "Refine curve " << argv[5];
  curve.refine(first,last,N);
  cout << "\t\t\t[OK]\n";

  cout << "Write resampled curve to "<<argv[6];
  curve.writePKF(argv[6]);
  cout << "\t[OK]\n";  
  return 0;

}

#endif // DOXYGEN_SHOULD_SKIP_THIS
