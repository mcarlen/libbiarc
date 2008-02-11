#include "Curve.h"
#include "curvealgos.h"

int main(int argc,char** argv) {

  if (argc!=2) {
    cout << "Usage : " << argv[0] << " pkffile\n";
    exit(1);
  }

  cout.precision(12);

  Curve<Vector3> curve(argv[1]);
  curve.link();
  curve.make_default();

  Vector3 dum1,dum2;
  double DD = compute_thickness(&curve,&dum1,&dum2);
  cout << DD << endl;

  return 0;

}
