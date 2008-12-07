#include "fourier_syn.h"
#include "CurveBundle.h"

#define OFFSET 0.5

int main(int argc, char ** argv) {

  if (argc!=3) {
    cout << "Usage : " << argv[0] << " coeffs out.pkf" << endl;
    exit(0);
  }

  FourierKnot fk(argv[1]), final;

  CurveBundle<Vector3> cb;
  Curve<Vector3> c;

  fk.c0.zero();
  fk.toCurve(100,&c);
  cb.newCurve(c);

  // Put the symmetries here :
  FourierKnot sym1(fk);

  // working k5.1 sym 1
/*
  sym1.shift(-.0865);
  sym1.rotate(Vector3(0,1,0),M_PI);
  sym1.flip_dir();
  sym1.shift(+.0765);
*/
  // test k5.1 sym 2 !! maybe same as above
  sym1.rotate(Vector3(1,0,0),M_PI);
  sym1 = sym1*(-1); // point sym
  sym1.mirror(Vector3(0,0,1));
  sym1.flip_dir();
  sym1.shift(.162);

/* looks same as first (match exactly)
  sym1.mirror(Vector3(0,0,1));
  sym1.rotate(Vector3(0,1,0),M_PI);
  sym1.mirror(Vector3(1,0,0));
*/

  sym1.c0[1] = OFFSET;

  c.flush_all();
  sym1.toCurve(100,&c);
  cb.newCurve(c);

  final = (fk+sym1)/2;
  c.flush_all();
  final.toCurve(100,&c);
  c.writePKF("final.pkf");

  cb.writePKF(argv[2]);

  cout << final << endl;

  return 0;
  
}
