/*!
  \file mesh4stokes.cpp
  \ingroup ToolsGroup
  \brief Produce a tube mesh from a curve and write that to a file.

  Long explanation ... Bla bla bla
*/

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include "../include/Tube.h"

int main(int argc, char **argv) {
  if(argc < 6 || argc > 7) {
    cerr << argv[0] << " <PFK file> <Nodes> "
	 << "<Segments> <Radius> <Out file> [Tolerance]\n";
    exit(0);
  }
  
  int N = atoi(argv[2]), S = atoi(argv[3]);
  float R = atof(argv[4]);
  
  float Tol;
  Tol = (argc==7?atof(argv[6]):1e-03);

  Tube<Vector3> *knot = new Tube<Vector3>(argv[1]);

  knot->link();

  ofstream file (argv[5]);

  if (knot->nodes()!=N) {
    cout << "Resample curve with " << N << " points";
    knot->make_default();
    knot->resample(N);
    cout << "\t\t[OK]\n";
  }
  else
    knot->make_default();

  cout << "Generate mesh (N="<<N<<",S="<<S<<",R="<<R<<")";
  knot->makeMesh(N,S,R,Tol);
  cout << "\t\t[OK]\n";
  
  cout << "Write to file : " << argv[5];
  
  if (!file.is_open()) {
    cerr << "\t\t\t[FAILED]\n";
    exit(1);
  }
  
  file << knot->nodes()    << "\t# number of nodes"   << endl;
  file << knot->segments() << "\t# circular segments" << endl;
  file << *knot;
  file.close();

  cout << "\t\t\t\t[OK]\n";
  
  return 0;
}

#endif // DOXYGEN_SHOULD_SKIP_THIS
