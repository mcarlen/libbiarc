/*!
  \file projected_dij.cpp
  \ingroup ToolsGroup
  \brief Compute \f$\sum_{i \neq j} d_{i,j}\f$
  and \f$\sum_{i \neq j} d_{i,j}^{-1}\f$ of a given projection.

  This tool aligns the pkf curve along an axis given as an input to the
  program. The pkf filename is expected to be the last argument on the
  command line. Then the reoriented curve is projected onto the XY-plane
  and resampled, such that the curve has a uniform distribution in the
  sense of arc-length. Then the following quantity is computed :

  \f$
  \frac{1}{N\cdot(N-1)} \sum_{i \neq j} d_{i,j}
  and
  \frac{1}{N\cdot(N-1)} \sum_{i \neq j} \frac{1}{d_{i,j}},
  \f$
  where \f$N\f$ is the number of points on the curve.

  The values are written to the standart output. There is a result
  with the sum of the distances and a result for the sum of the inverse
  of each distance.

  Points that are closer than some tolerance are rejected and do
  not contribute to the sum.
*/

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include "../include/Curve.h"

int main(int argc, char **argv) {

  if (argc!=6) {
    cerr << "Usage : "<<argv[0]<<" <EigVal> <AxisX> <AxisY> <AxisZ> <pkf in>\n";
    exit(0);
  }

  float EigVal = atof(argv[1]);
  Vector3 AlignAxis(atof(argv[2]),atof(argv[3]),atof(argv[4]));
  AlignAxis.normalize();

  clog << "Read in curve from " << argv[5];
  Curve<Vector3> curve(argv[5]);
  if (&curve!=NULL)
    clog << "\t[OK]\n";
  else {
    clog << "\t[FAILED]\n";
    return 1;
  }

  curve.link();
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
  cerr << "\t[OK]\n";

  clog << "Project curve onto the XY plane";

  Vector3 p, t;
  vector<Biarc<Vector3> >::iterator current = curve.begin();
  for (int i=0;i<curve.nodes();i++) {
    p = current->getPoint();
    t = current->getTangent();
    p[2] = 0.0;
    t[2] = 0.0;
    current->setPoint(p);
    current->setTangent(t);
    current++;
  }
  clog << "\t\t\t[OK]\n";

  clog << "Resample flat curve";
  curve.make_default();
  curve.resample(curve.nodes());
  clog << "\t\t\t\t[OK]\n";

/*
  clog << "Write flat curve to flat.pkf";
  curve.writePKF("flat.pkf");
  clog << "\t\t\t[OK]\n";
*/
  clog << "Compute sum of the distances between the points";

  float Tolerance = 1.0/4.0/(float)curve.nodes();
  clog << "Nodes = " << curve.nodes() << ", Tolerance = " << Tolerance << endl;

  float SUM = 0, dij;
  float SUM_INV = 0;
  vector<Biarc<Vector3> >::iterator b_i = curve.begin(), b_j;
  for (int i=0;i<curve.nodes();i++) {
    b_j = curve.begin();
    for (int j=0;j<i;j++) {
      dij = (b_i->getPoint()-b_j->getPoint()).norm();
      if (dij >= Tolerance) { // if point is too close, skip
        SUM += dij;
        SUM_INV += 1.0/dij;
      }
      b_j++;
    }
    b_i++;  }
  clog << "\t[OK]\n";

  cout.precision(8);
  SUM/=((float)curve.nodes()*((float)curve.nodes()-1.0)/2.0);
  SUM_INV/=((float)curve.nodes()*((float)curve.nodes()-1.0)/2.0);

  /*
  cout << "\n1/N Sum d_ij   : " << SUM << endl;
  cout << "1/N Sum 1/d_ij : " << SUM_INV << endl;
  */

  // Nom EigVal SumInv 1/SUM_INV*30cm
  cout << curve.getName() << '\t' << EigVal << '\t'
	<< SUM_INV << '\t' << 1.0/SUM_INV*30.0 << endl;
  return 0;

}

#endif // DOXYGEN_SHOULD_SKIP_THIS
