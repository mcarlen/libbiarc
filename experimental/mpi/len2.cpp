#include "../../include/Curve.h"
#include "../../include/algo_helpers.h"

int main(int argc, char** argv) {
  if (argc!=2) {
    cout << "Usage : " << argv[0] << " pkf\n";
		exit(0);
	}

  
	Curve<Vector3> c(argv[1]);
	c.link();
  float maket = start_time();
	c.make_default();
	cout << "Time make  : " << stop_time(maket) << endl;
	float tht = start_time();
	cout << c.length()/c.thickness() << endl;
	cout << "Time thick : " << stop_time(tht) << endl;
	return 0;
}
