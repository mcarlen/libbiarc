#include "../include/Curve.h"

// s arclength
// sigma, tau min in valley
// alpha max in valley

// given a file with
// s sigma s tau s alpha
// compute the pp=distance functions at these points
// and print some differences

struct sigmatau {
  float s, sigma, tau, alpha;
};

typedef sigmatau sigmatau;

int main(int argc, char** argv) {
  if (argc!=3) {
    cout << "Usage : " << argv[0] << " sigmatau pkf\n";
		exit(0);
	}

  vector<sigmatau> valley;
	sigmatau st;
	float dummy;
  ifstream in(argv[1]);
	while (in >> st.s >> st.sigma >> dummy >> st.tau >> dummy >> st.alpha) {
    valley.push_back(st);
	}

  Curve<Vector3> c(argv[2]);
	c.link();
	c.make_default();
	c.normalize();

  float fsigma, ftau, falpha;
  for (unsigned int i=0;i<valley.size();++i) {
		fsigma = c.pp(valley[i].s, valley[i].sigma);
		ftau   = c.pp(valley[i].s, valley[i].tau);
		falpha = c.pp(valley[i].s, valley[i].alpha);
    cout << valley[i].s << " " << falpha << " "
		     << fsigma << " " << ftau << endl;
	}

  return 0;
}
