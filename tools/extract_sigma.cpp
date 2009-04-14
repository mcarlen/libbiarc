#include "../include/Curve.h"

float find_min(const Curve<Vector3> &c, float s, float left, float right, int steps, float eps) {
	float cleft = left, cright = right;
	while (fabs(cright-cleft)>eps) {
    float t = (cright-cleft)/(float)steps;
		int im = 0;
		float vm = c.pp(s, cleft>1?cleft-1:cleft);
		for (int i=0;i<steps;++i) {
      float ct = cleft + (float)i*t;
			if (ct>1) ct -= 1;
      float val = c.pp(s, ct);
			if (val < vm) { im = i; vm = val; }
		}
		cleft = cleft+(float)(im-1)*t;
		cright = cleft+(float)(im+1)*t;
	}
	return cleft + (cright-cleft)*.5;
}

void build_sigma(const Curve<Vector3> &c, float start = 0.4921, int N = 2000) {

	float closeness = 0.01;
	float step = 1./float(N);
	float curr = start;
	float s = 0.0;

  int steps = 500;
	float eps = 1e-8;

  while (s < 1.0) {
    curr = find_min(c, s, curr-closeness, curr+closeness, steps, eps);
		if (curr > 1) curr = curr - 1;
		cout << s << " " << curr << endl;
		s += step;
	}
	cerr << "Ended correctly " << s << "\n";
}

float first_min(const Curve<Vector3> &c) {
  return find_min(c, 0, .2, .8, 500, 1e-8);
}

#define BLA
#ifdef BLA
int main(int argc, char** argv) {
  if (argc!=2) {
    cout << "Usage : " << argv[0] << " pkf\n";
		exit(0);
	}

  Curve<Vector3> c(argv[1]);
	c.link();
	c.make_default();
	c.normalize();

  cerr << "L="<<c.length()<<endl;
	float start = first_min(c);
	cerr << "min(0)=" << start << endl;
	build_sigma(c, start);
	c.writePKF("out.pkf");

  return 0;
}
#else
int main(int argc, char** argv) {
  Curve<Vector3> c(argv[1]);
	c.link();
	c.make_default();
  cout << c.pp(.1,.2) << endl;

}
#endif
