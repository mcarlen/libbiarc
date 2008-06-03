/*!
  \file closest_neighbour.cpp
  \ingroup ToolsGroup
  \brief Reconstruct the curve using the nearest point algo.

*/

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include "../include/CurveBundle.h"
#define biarc3_it vector<Biarc<Vector3> >::iterator

#include <vector>

vector<Vector3> container;

Vector3 fillContainer(Curve<Vector3> &c) {
  container.clear();
  for (biarc3_it it = (c.begin()+1); it != c.end(); ++it)
    container.push_back(it->getPoint());
  return c.begin()->getPoint();
}

void dumpContainer() {
  cout << "Container\n";
  for (vector<Vector3>::iterator it = container.begin(); it!=container.end();
       ++it)
    cout << *it << endl;
  cout << "End\n";
}

Vector3 findClosest(Vector3 &v) {
  vector<Vector3>::iterator cur = container.begin();
  float d_min = (v-(*cur)).norm(), d;
  vector<Vector3>::iterator p_min = cur;
  for (cur=container.begin()+1;cur!=container.end();cur++) {
    d = (v-(*cur)).norm();
    if (d<d_min) {
      d_min = d;
      p_min = cur;
    }
  }
  Vector3 vback(*p_min);
  container.erase(p_min);
  return vback;
}

int main(int argc, char **argv) {

  if (argc!=3) {
    cout << "Usage : "<<argv[0]
	 << " <pkf in> <pkf out>\n\n";
    exit(0);
  }

  CurveBundle<Vector3> cb(argv[1]);

  Curve<Vector3> curve;
  Vector3 vec;

  for (int i=0;i<cb.curves();i++) {
    vec = fillContainer(cb[i]);
    curve.append(vec,Vector3(1,0,0));

    while(container.size()>0) {
      vec = findClosest(vec);
      curve.append(vec,Vector3(1,0,0));
    }
    cb[i].flush_all();
    cb[i] = curve;
    // XXX assume closed curve ... !!!
    cb[i].link();
    cb[i].computeTangents();
  }
  cb.writePKF(argv[2]);
  return 0;
}

#endif // DOXYGEN_SHOULD_SKIP_THIS
