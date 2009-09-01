/*!
  \file pkf2java.cpp
  \ingroup ToolsGroup
  \brief Read data from a PKF file, produce a tubular mesh for each
  	curve in the file and write it as C/C++/Java arrays.
*/

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include "../include/TubeBundle.h"

ofstream & vertint(const Vector3 &v, ofstream &out) {
  int a = v[0]*100000,
      b = v[1]*100000,
      c = v[2]*100000;
  out << "  " << ((int)a) << ", " << ((int)b) << ", " << ((int)c) << ",\n";
  return out;
}

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

    cout << "N = " << N << ", S = " << S << endl;
    file << "int vertices[] {\n";
    for (int j=0;j<=N;j++) {
      for (int k=0;k<=S;k++) {
        vertint(knot[i].meshPoint(j*(S+1)+k),file);
      }
    }
    file << "};\n\n";

    // RGBA colors for each vertex
    file << "int colors[] {\n";
    for (int j=0;j<=N;j++) {
      for (int k=0;k<=S;k++) {
        file << "  1, 0, 0, 1,\n";
      }
    }
    file << "};\n\n";


    file << "int indeces[] = {\n";
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
        file << "  " << (j*(S+1)+k) << ", " << (xii*(S+1)+k) << ", " << (xii*(S+1)+kii) << ",   ";
        file << "  " << (j*(S+1)+k) << ", " << ((xii)*(S+1)+kii) << ", " << (j*(S+1)+kii) << (j==N-1&&k==S-1?"\n":",\n");
      }
    }
    file << "};\n";

    cout << "\t\t\t[OK]\n";
  }
  file.close();
  return 0;
}

#endif // DOXYGEN_SHOULD_SKIP_THIS
