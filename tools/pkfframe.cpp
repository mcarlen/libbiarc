/*!
  \file pkfframe.cpp
  \ingroup ToolsGroup
  \brief Generate a framing along the curve.
*/

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include "../include/Curve.h"
#include <stdlib.h>

void usage(char* prog) {
  cerr << "Usage : " << prog
       <<" <closed=1;open=0> <Number of points> <Infile> [Outfile]\n";
  exit(1);
}

int main(int argc, char** argv) {
  // Check number of arguments
  if (argc<4||argc>5) {
    usage(argv[0]);
  }

  // Segmentation value
  int N = atoi(argv[2]);

  // File to be resampled
  string infile(argv[3]);

  string outfile;
  if (argc==5)
    outfile = string(argv[4]);
  else {
    cout << "No output file specified, write to default.txt!\n";
    outfile = "default.txt";
  }

  // Process infile to outfile

  cout << "Read in file\t\t\t\t"<<flush;
  Curve<Vector3> original(infile.c_str());
  cout << "\t[OK]\n";
 
  int ToClose, Nloc;
  if (atoi(argv[1])) {
    original.link();
    ToClose = 1;
  } 
  else {
    ToClose = 0;
  }
  
  cout << "Interpolate pt/tan by biarcs\t\t";
  original.make_default();
  cout << "\t[OK]\n";
  
  if (N!=original.nodes()) {
    cout << "Resample curve with "<< N << " points\t\t" << flush;
    original.resample(N);
    original.link();
    original.make_default();
    cout << "\t[OK]\n";
  }
  
  Nloc = original.nodes();
  if (ToClose) Nloc++;
  // this code comes mainly from my makMesh function in Tube.cpp

  int TwistFlag = ToClose;
  float Tol = 0.001;

  Vector3* Normals = new Vector3[Nloc];
  Vector3* Points = new Vector3[Nloc];
  Vector3* Tangents= new Vector3[Nloc];
  Vector3 Binormal, Theta;
  Vector3 ClosestDirection;
  Matrix3 Frame, tmp;
  float b, a, denom, tn, tb, Theta0, Theta1, angleDiff = 100000.0;

  // Frame Twist Parameter
  float TwistSpeed = 0.0;
  int Stop = 1;
  float direction, AngularSpeedScale = 1.0;

  vector<Biarc<Vector3> >::iterator current = original.begin();
  for (int i=0;i<Nloc;i++) {
    Points[i] = original[i%original.nodes()].getPoint();
    Tangents[i] = original[i%original.nodes()].getTangent();
  }

  int iterations = 0;

  // Init condition for frame convention=(normal, binormal, tangent)
  // FIXME : If Points[1]-Point[0] is parallel to Tangents[0], than we have a problem !!!
  if (Tangents[0].cross(Points[1]-Points[0]).norm()>10e-8)
    Normals[0] = Tangents[0].cross(Points[1]-Points[0]);
  else
    Normals[0] = Tangents[0].cross(Vector3(0,1,0));

  // check cross product problem due to zero norm
  // FIXME : Tangents[0] could be || to 0,1,0 !!!
  if (Normals[0].norm()<0.0001) Normals[0]=Tangents[0].cross(Vector3(0,0,1));
  Normals[0].normalize();
  Binormal = Tangents[0].cross(Normals[0]);
  Binormal.normalize();

  Vector3 rot_point;
  float dist;

  do {

    Frame = Matrix3(Normals[0],Binormal,Tangents[0]);

    /*
     * Propagating the initial Normal[0] along the curve
     * the result is a sequence of smoothly chaning normal vectors
     * at each curve node
     */
    for (int i=1;i<Nloc;i++) {

      b = (1.0 - (Frame[2].dot(Tangents[i])));
      a = (2.0 - b)/(2.0 + 0.5*TwistSpeed*TwistSpeed);
      if (a < 1e-5) cerr << "generateMesh() : possible div by 0.\n";
      denom = a*(1.0+0.25*TwistSpeed*TwistSpeed);

      tn = Tangents[i].dot(Frame[0]);
      tb = Tangents[i].dot(Frame[1]);

      Theta0 = (0.5*TwistSpeed*tn - tb)/denom;
      Theta1 = (0.5*TwistSpeed*tb + tn)/denom;

      Theta = Vector3(Theta0,Theta1,TwistSpeed);

      // Get next local frame
      Frame = (Frame*tmp.cay(Theta));

      Normals[i] = Frame[0];
      Normals[i].normalize();

    }

    // only if curve closed
     if (!TwistFlag) {
       cout << "\nNo Twisting\n";
      Stop = 0;
     }
    else {
      ClosestDirection = Normals[Nloc-1];
      dist = (ClosestDirection-Normals[0]).norm();

// angle difference between first normal and closest last direction
      if (Normals[0].dot(ClosestDirection) > 1-Tol) Stop=0;

      else {
        angleDiff = acos(Normals[0].dot(ClosestDirection));
        // get rotation handedness to correct the mesh
        direction = (ClosestDirection.cross(Normals[0])).dot(Tangents[0]) ;
        AngularSpeedScale = angleDiff/Nloc; // maybe /10.0

        if (direction>0)
          TwistSpeed+=AngularSpeedScale;
        else
          TwistSpeed-=AngularSpeedScale;

        iterations++;
        if (iterations > 10) {
          cerr << "Warning : 10 iterations. Stopped with angle difference: "
               << angleDiff << " rad." << endl;
          Stop = 0;
        }
      }
    }
  } while (Stop);

  cout << "Write s,point,tangent,normal to "<<outfile.c_str()<< "\t";;
  ofstream fd(outfile.c_str());
  fd << "NoName " << original.nodes()
     << " " << original.length() << endl;
  for (int i=0;i<Nloc;i++)
    fd << Points[i] << " " << Tangents[i] << " " << Normals[i] << endl;
  fd.close();
  cout << "\t[OK]\n";

  cout << "Write inventor file normals.iv";
  ofstream ivout("normals.iv");
  ivout << "#Inventor V2.1 ascii\n\nSeparator {\n  Coordinate3 {\n    point [\n";
  float d = (Points[0]-Points[1]).norm();
  for (int i=0;i<original.nodes();++i)
    ivout << "      " << Points[i]
          << ", " << (Points[i]+d*Normals[i])
          << ",\n";
  ivout << "    ]\n  }\n  LineSet { numVertices [";
  for (int i=0;i<Nloc-1;++i) ivout << "2,";
  ivout << "2] }\n";
  ivout << "}\n";
  ivout.close();
  cout << "\t\t\t[OK]\n";

  return 0;
}

#endif // DOXYGEN_SHOULD_SKIP_THIS
