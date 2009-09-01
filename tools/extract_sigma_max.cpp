/*!
 \file extract_sigma_max.cpp
 \ingroup ToolsGroup
 \brief Find local max in pt valley.
*/
#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include "../include/Curve.h"

// Find the local max in the valley of torusknots
// i.e. like the extract_sigma tool, where we want
// to find the local mins in this valley

float find_max(const Curve<Vector3> &c, float s, float left, float right, int steps, float eps) {
	float cleft = left, cright = right;
	while (fabs(cright-cleft)>eps) {
    float t = (cright-cleft)/(float)steps;
		int im = 0;
		float vm = c.pp(s, cleft>1?cleft-1:cleft);
		for (int i=0;i<steps;++i) {
      float ct = cleft + (float)i*t;
			if (ct>1) ct -= 1;
      float val = c.pp(s, ct);
			if (val > vm) { im = i; vm = val; }
		}
		cleft = cleft+(float)(im-1)*t;
		cright = cleft+(float)(im+1)*t;
	}
	return cleft + (cright-cleft)*.5;
}

void build_sigma(const Curve<Vector3> &c, float start = 0.4921, int N = 2000) {

	float closeness = 0.0005;
	float step = 1./float(N);
	float curr = start;
	float s = 0.0;

  int steps = 50;
	float eps = 1e-12;

  while (s < 1.0) {
    curr = find_max(c, s, curr-closeness, curr+closeness, steps, eps);
		if (curr > 1) curr = curr - 1;
		cout << s << " " << curr << endl;
		s += step;
	}
	cerr << "Ended correctly " << s << "\n";
}

float first_max(const Curve<Vector3> &c) {
  return find_max(c, 0, .3, .7, 10000, 1e-12);
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
	float start = first_max(c);
	cerr << "min(0)=" << start << endl;
	// build_sigma(c, start, 2001);
	build_sigma(c, 0.5, 2001);
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

#endif // DOXYGEN
