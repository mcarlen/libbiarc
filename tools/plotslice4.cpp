/*!
  \file plotslice.cpp
  \ingroup ToolsGroup
  \brief Write a pt plot cross section at a given arclength.
*/

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include "../include/algo_helpers.h"
#include "../include/Vector4.h"
#include "../include/Curve.h"

int main(int argc, char **argv) {
  if(argc != 5) {
    cout << argv[0] << " <pp/pt/tt> <s> <sampling> <PFK file>\n";
    exit(0);
  }

  // default is pp
  int mode = 1;
  if (!strncmp("pt",argv[1],2))
    mode = 2;
  else if (!strncmp("tt",argv[1],2))
    mode = 3;

  Curve<Vector4> knot(argv[4]);
  knot.link(); knot.make_default();

  double thickness = knot.thickness();
  int N=atoi(argv[3]);
  double l = knot.length();
  double fac = l/(double)(N-1);
  double s = atof(argv[2]), t;
  Vector4 pt = knot.pointAt(s), tg = knot.tangentAt(s), pt2;

  for (int i=0;i<N;++i) {
    t = (double)i*fac;
cerr << "t="<<t<<endl;
    if (t>l) continue;
    pt2 = knot.pointAt(t);
    if (mode==2)
      cout << t << " " << .5*thickness*knot.radius_pt(pt,tg,pt2) << endl;
//    else if (mode==3)
//      cout << t << " " << knot.
    else
      cout << t << " " << 2*(pt-pt2).norm()/thickness << endl;
  }

  return 0;
}

#endif // DOXYGEN_SHOULD_SKIP_THIS
