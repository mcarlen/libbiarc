#include "fourier_3_1.h"
#include "fourier_4_1.h"
#include "fourier_5_1.h"

// This is clunky (3 funcs for doing the same)
Curve<Vector3> genn(const int N, const char* infile, const char* outfile) {
  Curve<Vector3> knot;
  FourierKnot fk(infile);
  fk.toCurve(N,&knot);
  knot.header("fourier normal","coeff2pkf","","");
  return knot;
}

Curve<Vector3> gen3(const int N, const char* infile, const char* outfile) {
  Curve<Vector3> knot;
  TrefoilFourierKnot fk(infile);
  fk.toCurve(adjust3,N,&knot);
//  fk.toCurve(adjust_with_spline,N,&knot);
  knot.header("fourier trefoil","coeff2pkf","","");
  return knot;
}

Curve<Vector4> genn_on_s3(const int N, const char* infile, const char* outfile) {
  Curve<Vector4> knot;
  FourierKnot fk(infile);
  fk.toCurveOnS3(N,&knot);
  knot.header("fourier normal","coeff2pkf","","");
  return knot;
}


Curve<Vector3> gen4(const int N, const char* infile, const char* outfile) {
  Curve<Vector3> knot;
  K41FourierKnot fk(infile);
  fk.toCurve(N,&knot);
  knot.header("fourier 4.1","coeff2pkf","","");
  return knot;
}

Curve<Vector3> gen5(const int N, const char* infile, const char* outfile) {
  Curve<Vector3> knot;
  K51FourierKnot fk(infile);
  fk.toCurve(adjust,N,&knot);
  knot.header("fourier 5.1","coeff2pkf","","");
  return knot;
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
    cout << "Usage : " << argv[0] << " <n/3/4/5/X>"
            " <N> <coeff_file> <out.pkf>\n";
    exit(0);
  }

  Curve<Vector3> knot;
  int N = atoi(argv[2]);
  char *infile = argv[3], *outfile = argv[4];

  switch(argv[1][0]) {
  case 'n': knot=genn(N,infile,outfile); break;
  case '3': knot=gen3(N,infile,outfile); break;
  case 'X': genn_on_s3(N,infile,outfile).writePKF(outfile); exit(0); break;
  case '4': knot=gen4(N,infile,outfile); break;
  case '5': knot=gen5(N,infile,outfile); break;
  default:
    cerr << "Wrong Fourier Knot type (n/3/4/5/X)\n";
    exit(1);
  }
#if 0
  cout << "normalizing knot." << endl;
  knot.link();
  knot.make_default();
  knot.normalize();
#endif

  knot.writePKF(outfile);
}
