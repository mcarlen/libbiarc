/*!
  \file angle_vs_arclength.cpp
  \ingroup ToolsGroup
  \brief Prints the arclength and for that arclength the angle between
         the normal vector and the "2" contact struts.

*/

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include "../include/Curve.h"
#include "../include/algo_helpers.h"

#define b3 vector<Biarc<Vector3> >::iterator

int th_cond(float d,Vector3& p0, Vector3 &p1, float tol) {
  if ( fabsf((p0-p1).norm()-d)<d*tol ) return 1;
  return 0;
}

Vector3 propagate(Matrix3 &Frame, const Vector3 &Tangent, float TwistSpeed = 0.0) {
  float a,b,denom,Theta0,Theta1,tn,tb;
  Vector3 Theta; Matrix3 tmp;
  b = (1.0 - (Frame[2].dot(Tangent)));
  a = (2.0 - b)/(2.0 + 0.5*TwistSpeed*TwistSpeed);
  if (a < 1e-5) cerr << "generateMesh() : possible div by 0.\n";
  denom = a*(1.0+0.25*TwistSpeed*TwistSpeed);

  tn = Tangent.dot(Frame[0]);
  tb = Tangent.dot(Frame[1]);

  Theta0 = (0.5*TwistSpeed*tn - tb)/denom;
  Theta1 = (0.5*TwistSpeed*tb + tn)/denom;

  Theta = Vector3(Theta0,Theta1,TwistSpeed);

  Frame = (Frame*tmp.cay(Theta));
  Frame[0].normalize();

  return Frame[0];
}

void generateNormals(int N, Vector3 *Normals, Curve<Vector3> &c, int S, float Tol) {

  Matrix3 Frame, tmp;
  Vector3 vt0, ClosestDirection;

  vt0 = c[0].getTangent();
  Normals[0] = (c[1].getPoint()-c[0].getPoint()).cross(vt0);
  Normals[0].normalize();

  // Propagate initial Frame along the the curve
  // given the tangents. Try to match up start and
  // end normal if the curve is closed!
  float TwistSpeed = 0.0;
  int PermutationIndex = 0, Stop = 0, iterations = 0;
  Vector3 rot_point;
  float direction, AngularSpeedScale = 1.0, dist, angleDiff = 1e6;

  do {
    Frame = Matrix3(Normals[0],vt0.cross(Normals[0]),vt0);

    for (int i=1;i<N;i++)
      Normals[i] = propagate(Frame,c[i%c.nodes()].getTangent(),TwistSpeed);

    ClosestDirection = Normals[N-1];
    dist = (ClosestDirection-Normals[0]).norm();
    PermutationIndex = 0;
/* Disable Permutation 
    for (int j=0;j<S;++j) {
      rot_point = Normals[N-1].rotPtAroundAxis(2.0*M_PI/(float)S*(float)j,
                                               vt0);
      if ((rot_point-Normals[0]).norm()<dist) {
        if (iterations==0) {
          ClosestDirection = rot_point;
          PermutationIndex = j;
        }
        else if (iterations>0 && j==PermutationIndex)
          ClosestDirection = rot_point;
      }
    }
*/

    if (Normals[0].dot(ClosestDirection) > 1-Tol) {
      angleDiff = acos(Normals[0].dot(ClosestDirection));
      cerr << "Angle diff " << angleDiff << " rad." << endl;
      Stop = 1;
    }
    else {
      angleDiff = acos(Normals[0].dot(ClosestDirection));
      // get rotation handedness to correct the mesh 
      direction = (ClosestDirection.cross(Normals[0])).dot(vt0) ;
      AngularSpeedScale = angleDiff/N;

      if (direction>0)
        TwistSpeed+=AngularSpeedScale;
      else
        TwistSpeed-=AngularSpeedScale;

      iterations++;
      if (iterations > 10) {
        cerr << "Warning : 10 iterations. Stopped with angle difference: "
             << angleDiff << " rad." << endl;
        Stop = 1;
      }
    }
  } while (!Stop);
}

float arclength(Curve<Vector3> &c, int n) {
  float s = 0;
  for (int i=0;i<n;++i)
    s += c[i].biarclength();
  return s;
}

/*
  Convert angle from acos() to interval [0,1]
*/
float OPI2 = 1/M_PI;
float myangle(Vector3 &v1, Vector3 &v2, Vector3 &tan) {
  float angle = v1.dot(v2), direc = 1.;
  if ((v1.cross(v2)).dot(tan)<0)
    direc = -1.;
  // Transform to interval [-pi,pi]->[-1,1]
  float ret = direc*acos(angle)*OPI2;
  // Return transformed [-1,1]->[0,1]
  return (ret+1.)*.5;
}

int main(int argc, char **argv) {

  if (argc!=5) {
    cout << "Usage : " << argv[0]
         << " <pkf> <Segments> <Frame Tol> <Contact Tol>\n";
    return 1;
  }

  int S = atoi(argv[2]);
  float Tol = atof(argv[3]);
  float tol = atof(argv[4]);

  // Init Curve
  Curve<Vector3> c(argv[1]);
  c.link();
  c.make_default();

  float thick = c.thickness();
  float L = c.length();

  // Generate Frame Normals
  int N = c.nodes()+(c.isClosed()?1:0);
  Vector3 Normals[N];
  generateNormals(N,Normals,c,S,Tol);

  /*
     For every point p on the curve, compute
     the contacts. Then compute the angle
     between the contact line and the Normal
     at the point p
  */
#define b3 vector<Biarc<Vector3> >::iterator
  Vector3 b0,b1,b2,p,v,CDir,tan;
  float r, slen;
  int ContactN = 0;
  for (int i=0;i<N;++i) {
    p = c[i%c.nodes()].getPoint();
    tan = c[i%c.nodes()].getTangent();
    slen = arclength(c,i)/L;
    for (b3 it=c.begin();it!=c.end();++it) {
      if (it==(c.begin()+i)) continue;
      it->getBezierArc0(b0,b1,b2);
      r = it->radius0();
      if (rhopt(p,b0,b1,b2,r,v) && th_cond(thick,p,v,tol)) {
        CDir = (v-p); CDir.normalize();
        cout << slen << " " << myangle(Normals[i],CDir,tan) << endl;
        // cout << slen << " " << myangle(acos(Normals[i].dot(CDir))) << endl;
        ++ContactN;
      }
      it->getBezierArc1(b0,b1,b2);
      r = it->radius1();
      if (rhopt(p,b0,b1,b2,r,v) && th_cond(thick,p,v,tol)) {
        CDir = (v-p); CDir.normalize();
        cout << slen << " " << myangle(Normals[i],CDir,tan) << endl;
        // cout << slen << " " << myangle(acos(Normals[i].dot(CDir))) << endl;
        ++ContactN;
      }
    }
  }
 

  cerr << ContactN << " Contacts." << endl;
  // Output the Normals
/*
  for (int i=0;i<N;i++)
    cerr << Normals[i] << endl;
*/
  
  return 0;
}

#endif // DOXYGEN_SHOULD_SKIP_THIS
