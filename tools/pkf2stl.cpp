/*!
  \file pkf2stl.cpp
  \ingroup ToolsGroup
  \brief Read data from a PKF file, produce a tubular mesh for each
  	curve in the file and write that to an STL file.
*/

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include <TubeBundle.h>
#include <algo_helpers.h>

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
  file << "solid " << knot.getName() << endl;
  for (int i=0;i<knot.tubes();i++) {
    knot[i].scale(10.);
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
//   cout << "N = " << N << ", S = " << S << endl;

    knot[i].write_STL(file);

    cout << "\t\t\t[OK]\n";
  }
  file << "endsolid " << knot.getName() << endl;
  file.close();
  return 0;
}

#endif // DOXYGEN_SHOULD_SKIP_THIS
