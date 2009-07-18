#include "fourier_syn.h"
#include "CurveBundle.h"

#define OFFSET .4

int main(int argc, char ** argv) {

  if (argc!=2+9) {
    cout << "Usage : " << argv[0] << " coeffs out.pkf matrix3x3" << endl;
    exit(0);
  }

/*
  Matrix3 m;

  for (int i=0;i<3;++i)
    for (int j=0;j<3;++j)
      m[i][j] = atof(argv[3+i*3+j]);

  fk.apply(m);
  fk.rotate(Vector3(0,0,1),M_PI/2.);
  fk.shift(-.0858);
*/

  FourierKnot fk(argv[1]), final;
  fk.c0.zero();

  // Write aligned fourier knot
  cout << setprecision(16) << fk << endl;

  // Put the symmetries here :
  FourierKnot sym(fk);
  sym.shift(.25);
  sym.rotate(Vector3(0,1,0),M_PI/2.);
  sym.mirror(Vector3(0,1,0));

  sym.c0[1] = OFFSET;

  cerr << setprecision(16) << sym << endl;

  return 0;
  
}
