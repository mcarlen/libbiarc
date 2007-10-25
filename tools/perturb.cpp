/*!
  \file perturb.cpp
  \ingroup ToolsGroup
  \brief Perturb a curve.

  This tool perturbs all the points of a given "closed" PKF curve. The perturbation
  takes place in the plane that is normal to the tangent vector at the point
  to be displaced. The angle is in the interval [0,2 pi] and the magnitude is
  randomly chosen according to the scaling factor given as an input.

*/

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include "../include/CurveBundle.h"
#include <time.h>
#include <stdlib.h>

int main(int argc, char **argv) {

  srand(time(NULL));

  if (argc!=4) {
    cerr << "Usage : "<<argv[0]<<" <magnitude> <pkf in> <pkf out>\n";
    exit(0);
  }

  CurveBundle<Vector3> cb;
  cout << "Read in PKF";
  cb.readPKF(argv[2]);
  cout << "\t\t\t\t[OK]\n";

  // FIXME : let the user chose if the curve is open or closed
  cb.link();

  float scale_mag = atof(argv[1]);
  for (int i=0;i<cb.curves();i++) {

    cout << "Perturb curve " << flush;

    vector<Biarc<Vector3> >::iterator current = cb[i].begin();

    Matrix3 Frame, tmp;
    Vector3 Tangent = current->getTangent();
    Vector3 Normal, Binormal, Theta, new_N;
    Vector3 cP, tmp_tan;

    float b, a, denom, tn, tb, Theta0, Theta1;
    float rand_magn;
    float rot_angle;

    // Init Normal
    Normal = Tangent.cross(Vector3(0,1,0));
    if (Normal.norm()<1e-8)
      Normal=Tangent.cross(Vector3(0,0,1));
    Normal.normalize();

    Binormal = Tangent.cross(Normal);
    Binormal.normalize();

    Frame = Matrix3(Normal,Binormal,Tangent);

    while (current!=(cb[i].end()-1)) {

      tmp_tan = current->getNext().getTangent();
      b = (1.0 - (Frame[2].dot(tmp_tan)));
      a = (2.0 - b)/2.0;
      if (a < 1e-5) cerr << "Warning : Possible div by 0.\n";
      denom = a;
  
      tn = tmp_tan.dot(Frame[0]);
      tb = tmp_tan.dot(Frame[1]);
      Theta0 = tb/denom;
      Theta1 = tn/denom;
      Theta = Vector3(Theta0,Theta1,0.0);
      
      Frame = (tmp.cay(Theta)*Frame);
      
      Normal = Frame[0];
      Normal.normalize();
      Binormal = Frame[1];
      Binormal.normalize();
      Tangent = Frame[2];
      Tangent.normalize();

      // Perturb
      rot_angle = (float)rand()/(float)RAND_MAX*2.0*M_PI;
      rand_magn = (float)rand()/(float)RAND_MAX*scale_mag;
      new_N = Normal.rotPtAroundAxis(rot_angle,
				     Tangent);
      cP = current->getPoint();
      // FIXME what to do with the new tangents? Perturb it as well?
      current->setPoint(cP+rand_magn*new_N);

      current++;

    }

    tmp_tan = current->getNext().getTangent();
    b = (1.0 - (Frame[2].dot(tmp_tan)));
    a = (2.0 - b)/2.0;
    if (a < 1e-5) cerr << "Warning : Possible div by 0.\n";
      denom = a;

    tn = tmp_tan.dot(Frame[0]);
    tb = tmp_tan.dot(Frame[1]);
    Theta0 = tb/denom;
    Theta1 = tn/denom;
    Theta = Vector3(Theta0,Theta1,0.0);

    Frame = (tmp.cay(Theta)*Frame);

    Normal = Frame[0];
    Normal.normalize();
    Binormal = Frame[1];
    Binormal.normalize();
    Tangent = Frame[2];
    Tangent.normalize();

    // Perturb
    rot_angle = (float)rand()/(float)RAND_MAX*2.0*M_PI;
    rand_magn = (float)rand()/(float)RAND_MAX*scale_mag;
    new_N = Normal.rotPtAroundAxis(rot_angle,
                                   Tangent);
    cP = current->getPoint();
    current->setPoint(cP+rand_magn*new_N);

    cout << "\t\t\t\t[OK]\n";
    cout << "Adjust tangents";
    cb[i].computeTangents();
    cout << "\t\t\t\t[OK]\n";
  }

  cout << "Write perturbed curve to file";
  cb.writePKF(argv[3]);
  cout << "\t\t[OK]\n";
  
  return 0;

}

#endif // DOXYGEN_SHOULD_SKIP_THIS
