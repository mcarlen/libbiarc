/*!
  \file pkf2obj.cpp
  \ingroup ToolsGroup
  \brief pkf to obj format. obj file has correct uv texture coords.
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
//    knot[i].scale(10.);
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

    // Vertices and Normals
    for (int j=0;j<N;++j) {
      for (int k=0;k<S;++k) {
          file << "v "  << knot[i].meshPoint(j*(S+1)+k) << endl;
          file << "vn " << knot[i].meshNormal(j*(S+1)+k) << endl;
      }
    }

    float alen = 0;
    for (int j=0;j<N+1;++j) {
      for (int k=0;k<S+1;++k) {
        file << "vt " << alen << " " << (float)k/(float)S<< endl;
      }
      if (j<N)
        alen += knot[i][j].biarclength()/knot[i].length();
    }

    // Triangles
    for (int j=0;j<N;++j) {
      for (int k=0;k<S;++k) {
        int xii=(j+1)%N, kii=(k+1)%S;
        file << "f " << j*(S)+k+1 << "/" << j*(S+1)+k+1 << "/" << j*(S)+k+1 << " "
             << xii*(S)+k+1 << "/" << (j+1)*(S+1)+k+1 << "/" << xii*(S)+k+1 << " "
             << xii*(S)+kii+1 << "/" << (j+1)*(S+1)+(k+1)+1 << "/" << xii*(S)+kii+1 << endl
             << "f " << j*(S)+k+1 << "/" << j*(S+1)+k+1 << "/" << j*(S)+k+1 << " "
             << xii*(S)+kii+1 << "/" << (j+1)*(S+1)+(k+1)+1 << "/" << xii*(S)+kii+1 << " "
             << j*(S)+kii+1 << "/" << j*(S+1)+(k+1)+1 << "/" <<j*(S)+kii+1 << endl;
      }
    }

    cout << "\t\t\t[OK]\n";
  }
  file.close();
  return 0;
}

#endif // DOXYGEN_SHOULD_SKIP_THIS
