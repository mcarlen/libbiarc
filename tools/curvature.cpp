/*!
  \file curvature.cpp
  \ingroup ToolsGroup
  \brief Outputs the curvature of a biarc curve.
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
//  cb.link();
  cb.make_default();
  //cb.normalize();
  //cb.make_default();

  cout.precision(16);

  FLOAT_TYPE s = 0, r, r_global, r_global_fast;
  for (int i=0;i<cb.curves();i++) {

    float D = cb[i].thickness();
    Vector3 v, p, b0, b1, b2;
    vector<Biarc<Vector3> >::iterator current, prev, next;
    for (current=cb[i].begin();current!=cb[i].end();current++) {
      // point to point global radius of curvature (if we use the correct thickness to scale the plot, there is an offset w/r to 1 !!
      r_global_fast = 2.*cb[i].radius_global(*current);

      // point to somewhere global rad of curvature (offset to 1 smaller, but thickness is somewhere to somewhere, so still not perfectly 1!!
      p = current->getPoint(); r_global = 1e22;
      prev = current-1;
      if (current==cb[i].begin()) prev = cb[i].end()-1;
      next = current+1;
      if (current==cb[i].end()-1) next = cb[i].begin();
      for (vector<Biarc<Vector3> >::iterator bb = cb[i].begin();bb!=cb[i].end();++bb) {
        if (bb==current || bb==next || bb==prev) { continue; }
        bb->getBezierArc0(b0,b1,b2);
        if (rhopt(p,b0,b1,b2,bb->radius0(),v)) {
          r_global = min(r_global, (p-v).norm());
        }
        bb->getBezierArc1(b0,b1,b2);
        if (rhopt(p,b0,b1,b2,bb->radius1(),v)) {
          r_global = min(r_global, (p-v).norm());
        }
      }
      r = current->radius0();
      if (r < 0)  // r is infty
         {cout << i << ' ' << 2*current->id() << ' ' << s << ' ' << 0.0
               << ' ' << D/r_global << ' ' << 0.0 << endl;}
      else
         {cout << i << ' ' << 2*current->id() << ' ' << s << ' '
               << D/(2*r) << ' ' << D/r_global << ' ' << D/r_global_fast << ' ' << 1./r << endl;}
      s+=current->arclength0();

      r = current->radius1();
      if (r < 0)  // r is infty
         {cout << i << ' ' << 2*current->id()+1 << ' ' << s << ' ' << 0.0
               << ' ' << D/r_global << ' ' << 0.0 << endl;}
      else
         {cout << i << ' ' << 2*current->id()+1 << ' ' << s << ' '
               << D/(2*r) << ' ' << D/r_global << ' ' << D/r_global_fast << ' ' << 1./r << endl;}
      s+=current->arclength1();
    }
  }
  return 0;

}

#endif // DOXYGEN_SHOULD_SKIP_THIS
