/*!
  \file torsion.cpp
  \ingroup ToolsGroup
  \brief Outputs the torsion of a biarc curve.
*/

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include "../include/CurveBundle.h"
#include "../include/algo_helpers.h"

int main(int argc, char **argv) {

  if (argc!=2) {
    cerr << "Usage : "<<argv[0]<<" <pkf in>\n";
    exit(0);
  }

  CurveBundle<Vector3> cb(argv[1]);
  // cb.link();
  cb.make_default();
  //cb.normalize();
  //cb.make_default();
  
  float s = 0;
  for (int i=0;i<cb.curves();i++) {
    vector<Biarc<Vector3> >::iterator current;
    for (current=cb[i].begin()+1;current!=cb[i].end()-1;current++) {
      cout << i << ' ' << current->id() << ' ' << s << ' '
           << cb[i].torsion(current->id(),0) << ' '
           << cb[i].torsion2(current->id()) << ' '
           << cb[i].signed_torsion(current->id(), 0) << endl;
      cout << i << ' ' << current->id() << ' ' << s+current->arclength0() << ' '
           << cb[i].torsion(current->id(),1) << ' '
           << cb[i].torsion2(current->id()) << ' '
           << cb[i].signed_torsion(current->id(), 1) << endl;
      s+=current->biarclength();
    }
  }
  return 0;

}

#endif // DOXYGEN_SHOULD_SKIP_THIS
