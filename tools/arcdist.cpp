/*!
  \file test.cpp
  \ingroup ToolsGroup
  \brief Program for testing purpose only.

  Long explanation ... Bla bla bla
*/

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include "../include/Vector3.h"
#include "../include/algo_helpers.h"

int main(int argc, char **argv) {

  Vector3 a0,a1,a2,b0,b1,b2;
  Vector3 from,to;

  a0 = Vector3(-1,1.2,0);
  a1 = Vector3(0,1,0);
  a2 = Vector3(1,1.2,0);
  b0 = Vector3(-1.01,-1.2,0);
  b1 = Vector3(0.01,-1,0);
  b2 = Vector3(1.01,-1.2,0);

  cout << "Min dist = "
       << mindist_between_arcs(a0,a1,a2,b0,b1,b2,&from,&to)
       << endl;
  cout << "Achieved between " << from << ", " << to << endl;

  return 0;
}

#endif // DOXYGEN_SHOULD_SKIP_THIS
