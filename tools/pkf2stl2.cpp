/*!
  \file pkf2stl2.cpp
  \ingroup ToolsGroup
  \brief Read data from a PKF file, produce a tubular mesh for each
  	curve in the file and write that to an STL file.
*/

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include <TubeBundle.h>
#include <algo_helpers.h>

int main(int argc, char **argv) {
  if(argc < 6 || argc > 7) {
    cerr << argv[0] << " <PFK file> <Node multiplier> "
	 << "<Segments> <Radius multiplier> <Out file> [Tolerance]\n";
    exit(0);
  }

  int N = atoi(argv[2]);
  int S = atoi(argv[3]);
  float R = atof(argv[4]);

  float Tol;
  Tol = (argc==7?atof(argv[6]):1e-03);

  TubeBundle<Vector3> knot(argv[1]);
  ofstream file (argv[5]);
  if (!file.good()) {
	cerr << "[FAILED]\n";
	exit(1);
  }

  // FIXME: presumably could do this on the TubeBundle, too.
  CurveBundle<Vector3> cb(argv[1]);
  cb.link();
  cb.make_default();
  R *= compute_thickness(&cb) / 2;

  cout << "Process all curves in PKF :\n";
  file << "solid " << knot.getName() << endl;
  for (int i=0;i<knot.tubes();i++) {
    knot[i].link();
    if (N != 1) {
      cout << "Resample curve " << i+1 << " with " << knot[i].nodes() * N
	   << " points";
      knot[i].make_default();
      knot[i].resample(knot[i].nodes() * N);
      cout << "\t\t[OK]\n";
    }
    else
      knot[i].make_default();

    cout << "Generate mesh (N=" << knot[i].nodes() << ",S=" << S
	 << ",R=" << R<<")";
    knot[i].makeMesh(knot[i].nodes(),S,R,Tol);
    cout << "\t\t[OK]\n";

    cout << "Write to file : " << argv[5];

    write_STL(file, knot[i], S);

    cout << "\t\t\t[OK]\n";
  }
  file << "endsolid " << knot.getName() << endl;
  file.close();
  return 0;
}

#endif // DOXYGEN_SHOULD_SKIP_THIS
