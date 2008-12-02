#include <fourier_syn.h>

float adjust(float x) {
  return x+0.9/(3.*2.*M_PI)*sin(3.*2.*M_PI*x);
}

float adjust2(float x) {
  float s = 0.95;
  return (x+s/(3.*2.*M_PI)*sin(3.*2.*M_PI*x) +
         s/(3.*2.*M_PI)*sin(3.*2.*M_PI*(x+s/(3.*2.*M_PI)*sin(3.*2.*M_PI*x))));
}

/*!
  Given a path to a coefficient file, sample the Fourier Knot
  and write it to out.pkf.

  The optional flag tref is in case of a "trefoil coefficient file".
  i.e.
  row : sin_y[i] sin_y[i+1] sin_z[i+2]
*/
int main(int argc, char **argv) {

  if (argc!=5) {
    cout << "Usage : " << argv[0] << " <normal/tref>"
            " <N> <coeff_file> <out.pkf>\n";
    exit(0);
  }

  int TREF = !strncmp(argv[1],"tref",4);
  int N = atoi(argv[2]);
  char *infile = argv[3], *outfile = argv[4];

  float iN = 1./(float)N;

  TrefoilFourierKnot tref;
  FourierKnot fk;
  if (TREF) tref = TrefoilFourierKnot(infile);
  else fk = FourierKnot(infile);

  Curve<Vector3> knot;
  float t;
  for (int i=0;i<N;++i) {
    t = adjust((float)i*iN);
    knot.append(TREF?tref(t):fk(t),TREF?tref.prime(t):fk.prime(t));
  }
  knot.writePKF(outfile);

}
