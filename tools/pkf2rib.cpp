/*!
  \file pkf2ply.cpp
  \ingroup ToolsGroup
  \brief pkf to mesh to stanford ply format.
*/

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include "../include/TubeBundle.h"
#include "ri.h"

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

  TubeBundle<Vector3> knot(argv[1]);
  ofstream file (argv[5]);
  if (!file.good()) {
	cerr << "[FAILED]\n";
	exit(1);
  }
  cout << "Process all curves in PKF :\n";
  for (int i=0;i<knot.tubes();i++) {
    // knot[i].scale(10000.);
    knot[i].link();
    if (knot[i].nodes()!=N) {
      cout << "Resample curve " << i+1 << " with " << N << " points";
      knot[i].make_default();
      knot[i].resample(N);
      cout << "\t\t[OK]\n";
    }
    else
      knot[i].make_default();

    cout << "Generate mesh (N="<<N<<",S="<<S<<",R="<<R<<")";
    knot[i].makeMesh(N,S,R,Tol);
    knot[i].renderman_init();
    cout << "\t\t[OK]\n";
  
    cout << "Write to file : " << argv[5];
    knot[i].exportRIBFile(argv[5],800,600,Vector3(0,0,-1),Vector3(),0,Vector3(0,0,1));

    cout << "\t\t\t[OK]\n";
  }
  file.close();
  return 0;
}

#endif // DOXYGEN_SHOULD_SKIP_THIS
