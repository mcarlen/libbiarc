#include "fourier_3_1.h"
#include "fourier_4_1.h"

float adjust(float x) {
  // return x+0.9/(3.*2.*M_PI)*sin(3.*2.*M_PI*x);
  float off = M_PI/2+.1;
  return x+0.95/(3.*M_PI)*sin(3.*M_PI*x+off)*sin(3.*M_PI*x+off);
}

float adjust2(float x) {
  float s = 0.95;
  return (x+s/(3.*2.*M_PI)*sin(3.*2.*M_PI*x) +
         s/(3.*2.*M_PI)*sin(3.*2.*M_PI*(x+s/(3.*2.*M_PI)*sin(3.*2.*M_PI*x))));
}

// This is clunky (3 funcs for doing the same)
void genn(const int N, const char* infile, const char* outfile) {
  Curve<Vector3> knot;
  FourierKnot fk(infile);
  fk.toCurve(N,&knot);
  knot.header("fourier normal","coeff2pkf","","");
  knot.writePKF(outfile);
}

void gen3(const int N, const char* infile, const char* outfile) {
  Curve<Vector3> knot;
  TrefoilFourierKnot fk(infile);
  fk.toCurve(adjust,N,&knot);
  knot.header("fourier trefoil","coeff2pkf","","");
  knot.writePKF(outfile);
}

void gen4(const int N, const char* infile, const char* outfile) {
  Curve<Vector3> knot;
  K41FourierKnot fk(infile);
  fk.toCurve(N,&knot);
  knot.header("fourier 4.1","coeff2pkf","","");
  knot.writePKF(outfile);
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
    cout << "Usage : " << argv[0] << " <n/3/4>"
            " <N> <coeff_file> <out.pkf>\n";
    exit(0);
  }

  int N = atoi(argv[2]);
  char *infile = argv[3], *outfile = argv[4];

  switch(argv[1][0]) {
  case 'n': genn(N,infile,outfile); break;
  case '3': gen3(N,infile,outfile); break;
  case '4': gen4(N,infile,outfile); break;
  default:
    cerr << "Wrong Fourier Knot type (n/3/4)\n";
    exit(1);
  }
}
