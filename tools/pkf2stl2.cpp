/*!
  \file pkf2stl2.cpp
  \ingroup ToolsGroup
  \brief Read data from a PKF file, produce a tubular mesh for each
  	curve in the file and write that to an STL file.
*/

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include <TubeBundle.h>
#include <algo_helpers.h>

static void write_facet(ostream &file,
			const Vector3 a, const Vector3 b, const Vector3 c) {
  const char fn[] = "facet normal ";
  const char ol[] = "  outer loop\n";
  const char v[]  = "    vertex ";
  const char el[] = "  endloop\n";
  const char ef[] = "endfacet\n";

  file << fn
       << (a - b).cross(b - c).normalize() << endl
       << ol << v
       << a << endl << v
       << b << endl << v
       << c << endl
       << el << ef;
}

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

    file << "solid " << knot[i].getName() << endl;
    for (int j=0;j<knot[i].nodes();++j) {
      for (int k=0;k<S;++k) {
      int xii=(j+1)%knot[i].nodes();
      int kii=(k+1)%S;

      write_facet(file,
		  knot[i].meshPoint(j*(S+1)+k),
		  knot[i].meshPoint(xii*(S+1)+k),
		  knot[i].meshPoint(xii*(S+1)+kii));

      write_facet(file, knot[i].meshPoint(j*(S+1)+k),
		  knot[i].meshPoint(xii*(S+1)+kii),
		  knot[i].meshPoint(j*(S+1)+kii));
      }
    }
    file << "endsolid " << knot[i].getName() << endl;

    cout << "\t\t\t[OK]\n";
  }
  file.close();
  return 0;
}

#endif // DOXYGEN_SHOULD_SKIP_THIS
