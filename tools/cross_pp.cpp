#include "../include/Curve.h"

int main(int argc, char** argv) {

  int N;
	float s, t0, t1;
  if (argc!=6) {
    cout << "Usage : " << argv[0] << " s t0 t1 N pkf\n";
		exit(0);
	}

  s = atof(argv[1]);
	t0 = atof(argv[2]);
	t1 = atof(argv[3]);
  N = atoi(argv[4]);
  Curve<Vector3> c(argv[5]);
	c.make_default();
	c.normalize();
	c.make_default();

  for (int i=0;i<N;++i) {
		float t = t0 + (t1-t0)*(float)i/(float)N;
		cout << t << " " << c.pp(s, t) << endl;
	}

  return 0;
}
