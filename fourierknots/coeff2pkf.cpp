#include <fourier_syn.cpp>

float adjust(float x) {
  return x+0.9/(3.*2.*M_PI)*sin(3.*2.*M_PI*x);
}

float adjust2(float x) {
  float s = 0.95;
  return (x+s/(3.*2.*M_PI)*sin(3.*2.*M_PI*x) +
         s/(3.*2.*M_PI)*sin(3.*2.*M_PI*(x+s/(3.*2.*M_PI)*sin(3.*2.*M_PI*x))));
}

int main(int argc, char **argv) {

  if (argc!=2) {
    cout << "Usage : " << argv[0] << " <coeff_file>\n";
    exit(0);
  }

  int N = 200;
  float iN = 1./(float)N;

  TrefoilFourierKnot tref(argv[1]);
  Curve<Vector3> knot;
  float t;

  for (int i=0;i<N;++i) {
    t = adjust((float)i*iN);
    knot.append(tref(t),tref.prime(t));
  }
  knot.writePKF("bla.pkf");

}
