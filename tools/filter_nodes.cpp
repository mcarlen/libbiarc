/*!
  \file filter_nodes.cpp
  \ingroup ToolsGroup
  \brief Filter a PKF curve. I.e. remove points that are too
         close to some neighbour.

*/

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include "../include/CurveBundle.h"
#define biarc3_it vector<Biarc<Vector3> >::iterator

int main(int argc, char **argv) {

  if (argc!=4) {
    cout << "Usage : "<<argv[0]
	 << " <tolerance> <pkf in> <pkf out>\n\n";
    cout << "Filter out all nodes that are to close to a neighbor "
	 << "considering the given tolerance.\n";
    exit(0);
  }

  CurveBundle<Vector3> cb(argv[2]);
  float tolerance = atof(argv[1]);

  int CountNodes = 0;
  cout << "Filter curve " << flush;
  for (int i=0;i<cb.curves();i++) {

    biarc3_it previous = cb[i].begin();
    biarc3_it current = previous+1;

    while (current!=cb[i].end()) {
      if((current->getPoint() - previous->getPoint()).norm() < tolerance) {
	cout << "remove "<<endl <<flush;
        previous = current;
        current++;
        cb[i].remove(previous);
	CountNodes++;
      }
      else {
        previous = current;
        current++;
      }
    }
  }
  cout << "\t[OK]\n";  

  cout << CountNodes << " nodes removed.\n";

  cout << "Write filtered curve to "<<argv[3];
  cb.writePKF(argv[3]);
  cout << "\t[OK]\n";  
  return 0;

}

#endif // DOXYGEN_SHOULD_SKIP_THIS
