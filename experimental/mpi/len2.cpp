#include "../../include/Curve.h"
#include "../../include/algo_helpers.h"

int main(int argc, char** argv) {
  if (argc!=2) {
    cout << "Usage : " << argv[0] << " pkf\n";
		exit(0);
	}

	Curve<Vector3> c(argv[1]);
	c.link();
	c.make_default();
	cout << c.length()/c.thickness() << endl;
	return 0;
}
