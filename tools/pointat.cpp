/*!
  \file pointat.cpp
  \ingroup ToolsGroup
  \brief Writes point/tangent at Curve(s), for s arclength and s in [0,1]
         to the standart output.

*/

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include "../include/CurveBundle.h"

#define write_vec(vec) (cout << vec[0] << ',' << vec[1] << ',' << (vec[2]))

CurveBundle<Vector3> *cb;

void printpointat(float s) {
  vector<Biarc<Vector3> >::iterator current;
  Vector3 p, t;

  for (int i=0;i<cb->curves();i++) {

    if (s==0.0) {
      write_vec((*cb)[i].begin()->getPoint()); cout << ';';
      write_vec((*cb)[i].begin()->getTangent()); cout << '\n';
    }
    else if (s==1.0) {
      if ((*cb)[i].isClosed()) current = (*cb)[i].begin();
      else current = (*cb)[i].end()-1;
      write_vec(current->getPoint()); cout << ';';
      write_vec(current->getTangent()); cout << '\n';
    }
    else {
      current = (*cb)[i].begin();

      // check if we have valid biarcs!
      assert(current->isBiarc());
 
      float Total = s*(*cb)[i].length();
      while (Total > current->biarclength()) {
        Total -= current->biarclength();
        current++;
      }

      p = current->pointOnBiarc(Total);
      if (Total > current->arclength0())
        t = current->getMidTangent().reflect(p-current->getMidPoint());
      else
        t = current->getTangent().reflect(p-current->getPoint());

      t.normalize();    

      write_vec(p); cout << ';'; write_vec(t); cout << '\n';
    }
  }
}

int main(int argc, char **argv) {

  if (argc!=4) {
    cerr << "Usage : "<<argv[0]<<" <closed=1;open=0> <pkf in> <s>\n";
    exit(0);
  }
  cb = new CurveBundle<Vector3>(argv[2]);

  float s = atof(argv[3]);
  assert(s>=0.0 && s<=1.0);

  if (atoi(argv[1]))
    cb->link();
  cb->make_default();

  cout.precision(8);
  float Total = (*cb)[0].length();
  cout << "Length:" << Total << endl;
  while (1) {
    printpointat(s);
    cin >> s;
    if (s<0 || s>1.0) break;
  }
  return 0;

}

#endif // DOXYGEN_SHOULD_SKIP_THIS
