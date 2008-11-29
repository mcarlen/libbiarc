/*!
  \file curvature.cpp
  \ingroup ToolsGroup
  \brief Outputs the curvature of a biarc curve.

  Long explanation ... Bla bla bla
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
  cb.link();
  cb.make_default();
  //cb.normalize();
  //cb.make_default();
  
  float s = 0, r, r_global;
  for (int i=0;i<cb.curves();i++) {

    float D = cb[i].thickness();
    vector<Biarc<Vector3> >::iterator current;
    for (current=cb[i].begin();current!=cb[i].end();current++) {
      r_global = 2.*cb[i].radius_global(*current);
      r = current->radius0();
      if (r < 0)  // r is infty
         {cout << i << ' ' << 2*current->id() << ' ' << s << ' ' << 0.0
               << ' ' << D/r_global << endl;}
      else
         {cout << i << ' ' << 2*current->id() << ' ' << s << ' '
               << D/(2*r) << ' ' << D/r_global << endl;}
      s+=current->arclength0();

      r = current->radius1();
      if (r < 0)  // r is infty
         {cout << i << ' ' << 2*current->id()+1 << ' ' << s << ' ' << 0.0
               << ' ' << D/r_global << endl;}
      else
         {cout << i << ' ' << 2*current->id()+1 << ' ' << s << ' '
               << D/(2*r) << ' ' << D/r_global << endl;}
      s+=current->arclength1();
    }
  }
  return 0;

}

#endif // DOXYGEN_SHOULD_SKIP_THIS
