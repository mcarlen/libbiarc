/*!
  \file resample.cpp
  \ingroup ToolsGroup
  \brief Resample an open or closed PKF curve.
*/

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include "../include/CurveBundle.h"
#include <stdlib.h>

void parallel_frame_normals(Curve<Vector3>* t, Vector3* nor, float TwistSpeed) {
  
  Vector3 tan, bin;
  Vector3 vec;

  Matrix3 Frame, tmp;
  float a,b,denom,Theta0,Theta1,tn,tb;
  Vector3 Theta;

  for (int i=0;i<t->nodes();++i) {

    tan = (*t)[i].getTangent();
    tan.normalize();

    if (i==0) {
      nor[i] = t->normalVector(0);
      nor[i].normalize();
      bin = nor[i].cross(tan);   bin.normalize();
      Frame = Matrix3(nor[i],bin,tan);
    }
    else {
      // Next frame

      b = (1.0 - (Frame[2].dot(tan)));
      a = (2.0 - b)/(2.0 + 0.5*TwistSpeed*TwistSpeed);
      if (a < 1e-5) cerr << "Parallel Framing : possible div by 0.\n";
      denom = a*(1.0+0.25*TwistSpeed*TwistSpeed);

      tn = tan.dot(Frame[0]);
      tb = tan.dot(Frame[1]);

      Theta0 = (0.5*TwistSpeed*tn - tb)/denom;
      Theta1 = (0.5*TwistSpeed*tb + tn)/denom;

      Theta = Vector3(Theta0,Theta1,TwistSpeed);

      // Get next local frame
      Frame = (Frame*tmp.cay(Theta));

      nor[i] = Frame[0];
      nor[i].normalize();

      bin = nor[i].cross(tan);   bin.normalize();
    }
  }
}

void usage(char* prog) {
  cerr << "Usage : " << prog
       <<" <speed> <distance> <file> [Outfile]\n";
  exit(1);
}

int main(int argc, char** argv) {
  // Check number of arguments
  if (argc!=4)
    usage(argv[0]);

  // Segmentation value
  float speed = atof(argv[1]);
  float dist  = atof(argv[2]);
  string infile(argv[3]);
  Curve<Vector3> original(infile.c_str());

  original.link();
  original.make_default();
 
  Vector3* normals = new Vector3[original.nodes()];
  parallel_frame_normals(&original, normals, speed);

  Curve<Vector3> curve;
  for (int i=0;i<original.nodes();++i)
    curve.append(original[i].getPoint()+normals[i]*dist, Vector3());
  curve.computeTangents();
  
  curve.writePKF("out.pkf");

  return 0;
}

#endif // DOXYGEN_SHOULD_SKIP_THIS
