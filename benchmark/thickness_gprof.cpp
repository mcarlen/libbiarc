#include "../include/algo_helpers.h"
#include "../objects/aux.h"

int main(int argc, char **argv) {

#ifdef CIRCLE
  Curve<Vector3>* c = gen_circle(1.0, 400);
  c->link();
#else
  Curve<Vector3>* c = new Curve<Vector3>(argv[1]);
  c->link();
  c->make_default();
#endif

  cout << c->thickness() << endl;

  delete c;

  return 0;
}
