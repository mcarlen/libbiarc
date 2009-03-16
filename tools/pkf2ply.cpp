/*!
  \file pkf2mesh.cpp
  \ingroup ToolsGroup
  \brief Read data from a PKF file, produce a tubular mesh for each
  	curve in the file and write that to a file.

	FIXME::
Here all the tubes will have the same number of nodes,segments,
same radius and if tolerance, same tolerance
  Long explanation ... Bla bla bla
*/

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include "../include/TubeBundle.h"

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
    cout << "\t\t[OK]\n";
  
    cout << "Write to file : " << argv[5];

    file << "ply\nformat ascii 1.0\ncomment created by libbiarc\n"
         << "element vertex " << ((S+1)*(N+1)) << "\nproperty float32 x\n"
         << "property float32 y\nproperty float32 z\nelement face "
				 // XXX change this if you change triangle 3 to quad 4
         << (2*S*N) << "\nproperty list uint8 int32 vertex_indices\n"
         << "end_header\n";

    cout << "N = " << N << ", S = " << S << endl;
    for (int j=0;j<=N;j++) {
      for (int k=0;k<=S;k++) {
        file << knot[i].meshPoint(j*(S+1)+k) << endl;
      }
    }

    for (int j=0;j<N;j++) {
      for (int k=0;k<S;k++) {
        int xii=(j+1)%N, kii=(k+1)%S;
/*
        file << "4 "
             << (j*(S+1)+k) << " "
             << (xii*(S+1)+k) << " "
             << (xii*(S+1)+kii) << " "
             << (j*(S+1)+kii) << endl;
*/
        file << "3 "
             << (j*(S+1)+k) << " "
             << (xii*(S+1)+k) << " "
             << (xii*(S+1)+kii) << endl;

        file << "3 "
             << (j*(S+1)+k) << " "
             << ((xii)*(S+1)+kii) << " "
             << (j*(S+1)+kii) << endl;
      }
    }

    cout << "\t\t\t[OK]\n";
  }
  file.close();
  return 0;
}

#endif // DOXYGEN_SHOULD_SKIP_THIS
