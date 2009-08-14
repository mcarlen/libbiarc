/*!
  \file convolutionfilter.cpp
  \ingroup ToolsGroup
  \brief Smooth a curve using a local gaussian convolution filter.

*/

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include "../include/Curve.h"
#include "../include/algo_helpers.h"

#define b3 vector<Biarc<Vector3> >::iterator


int main(int argc, char **argv) {

  if (argc!=5) {
    cout << "Usage : " << argv[0]
         << " <thickness> <stencil size> <iterations> <pkf>\n";
    return 1;
  }


  int Stencil = 1; // argv[2]
  int Iterations = atoi(argv[3]);
  Curve<Vector3> c(argv[4]);
// XXX chose if open or closed
//  c.link();
  c.make_default();

  cout << "Thickness is " << c.thickness_fast() << endl;

  float thick = atof(argv[1]);

#define b3 vector<Biarc<Vector3> >::iterator
  float r0, r1;
  int changed = 0;
  for (int i=1;i<=Iterations;++i) {
    for (b3 it=c.begin();it!=c.end();++it) {
     /*
        change only nodes that have the local radius smaller
        than the thickness of the curve is.
      */
     b3 prev, next;
     if (it == c.begin()) prev = c.end()-1;
     else prev = it-1;
     if (it == c.end()-1) next = c.begin();
     else next = it+1;
      r0 = prev->radius1();
      r1 = it->radius0();
// cout << "R " << r0 << " " << r1 << endl;
      // if (r0*2.0 < thick || r1*2.0 < thick) {
// Select more points accoring to the stencil variable
        it->setPoint(.25*prev->getPoint()+
                     .5*it->getPoint()+
                     .25*next->getPoint());
        changed++;
      // }
    }
    if (changed>0) {
      c.make_default();
//      cout << "Iteration " << i << " : Changed " << changed << " points\n";
      changed = 0;
    }
    else {
      cout << "Finished after " << i << " iterations\n";
      break;
    }
  }

  c.computeTangents();
  c.writePKF("out.pkf");
  
  return 0;
}

#endif // DOXYGEN_SHOULD_SKIP_THIS
