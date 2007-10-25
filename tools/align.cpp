/*!
  \file align.cpp
  \ingroup ToolsGroup
  \brief Orient a pkf curve according to a given axes.

  This tool reads a PKF curve from a file, then 3 arguments for the
  alignement axis (X,Y,Z) are given. The curve is reoriented and then
  written to an output PKF file.
*/

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include "../include/Curve.h"

int main(int argc, char **argv) {

  if (argc!=6) {
    cerr << "Usage : "<<argv[0]<<" <AxisX> <AxisY> <AxisZ> <pkf in> <pkf out>\n";
    exit(0);
  }

  clog << "Read in curve from " << argv[4];
  Curve<Vector3> curve(argv[4]);
  if (&curve!=NULL)
    clog << "\t[OK]\n";
  else {
    clog << "\t[FAILED]\n";
    return 1;
  }

  Vector3 AlignAxis(atof(argv[1]),atof(argv[2]),atof(argv[3]));
  AlignAxis.normalize();
  clog << "Align curve along " << AlignAxis;

  Matrix3 Frame;
  Vector3 vec_tmp;
  if (AlignAxis.dot(Vector3(1,0,0))-1>1e-8)
    vec_tmp = AlignAxis.cross(Vector3(1,0,0));
  else
    vec_tmp = AlignAxis.cross(Vector3(0,1,0));

  // Construct orthonormal frame
  Frame[2] = AlignAxis;
  Frame[1] = vec_tmp - AlignAxis*vec_tmp.dot(AlignAxis);	     
  Frame[0] = Frame[1].cross(Frame[2]);

  for (int i=0;i<2;i++)
    Frame[i].normalize();

  curve.apply(Frame.transpose());
  clog << "\t[OK]\n";

  clog << "Write curve to " << argv[5]; 
  if (curve.writePKF(argv[5]))
    clog << "\t\t\t[OK]\n";
  else {
    clog << "\t\t\t[FAILED]\n";
    return 2;
  }
  return 0;

}

#endif // DOXYGEN_SHOULD_SKIP_THIS
