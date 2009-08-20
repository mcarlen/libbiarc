#include "algo_helpers.h"

int main(int argc, char** argv) {
  CurveBundle<Vector3> cb(argv[1]);
  cb.link();
  cb.make_default();
  cout << "D=" << compute_thickness(&cb) << endl;
  return 0;
}
