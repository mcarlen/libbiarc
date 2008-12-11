#include "fourier_3_1.h"
#include "fourier_4_1.h"

void curvature(const FourierKnot &fk, const int N) {
  float isampling = 1./(float)N, t;
  Vector3 rp,rpp;
  for (int i=0;i<N;++i) {
    rpp.zero(); rp.zero();
    t = adjust((float)i*isampling);
    cout << t << " " << fk.curvature(t) << endl;
  }
}

int main(int argc, char **argv) {

  if (argc!=4) {
    cout << "Usage : " << argv[0] << " <n/3/4>"
            " <N> <coeff_file>\n";
    exit(0);
  }

  int N = atoi(argv[2]);
  char *infile = argv[3];

  if (argv[1][0]=='n') {
    FourierKnot fk(infile);
    curvature(fk,N);
  }
  else if (argv[1][0]=='3') {
    TrefoilFourierKnot fk(infile);
    curvature(fk,N);
  }
  else if (argv[1][0]=='4') {
    K41FourierKnot fk(infile);
    curvature(fk.toFourierKnot(),N);
  }
  else {
    cerr << "Wrong type : " << argv[1] << ". Must be n, 3 or 4.\n";
    exit(1);
  }

  return 0;
}
