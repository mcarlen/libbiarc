/*!
 \file link_thickness.cpp
 \ingroup ToolsGroup
 \brief Compute thickness of a CurveBundle.
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include "algo_helpers.h"

int main(int argc, char** argv) {
  CurveBundle<Vector3> cb(argv[1]);
  cb.link();
  cb.make_default();
  cout << "D=" << compute_thickness(&cb) << endl;
  return 0;
}

#endif // DOXYGEN_SHOULD_SKIP_THIS
