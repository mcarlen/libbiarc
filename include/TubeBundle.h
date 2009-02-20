#ifndef __TUBE_BUNDLE_H__
#define __TUBE_BUNDLE_H__

#include <fstream>
#include <assert.h>
#include <vector>
#include "PKFmanip.h"
#include "Tube.h"
#include "CurveBundle.h"

template <class Vector>
class TubeBundle : public PKFmanip {

  vector< Tube<Vector> > container;

  // Box that contains all the curves in container
  // Bottom left corner
  Vector _BBox_Min;
  // Upper right corner
  Vector _BBox_Max;
  // Center of mass
  Vector _Center;

  // this routine is called f.ex by makeMesh or scaleTubeRadius
  // to update the current bounging box and center value
  void computeBoundingBox();

  void clear_tb();
  void init();

 public:

  TubeBundle();
  TubeBundle(const char* filename);
  TubeBundle(const TubeBundle<Vector> &tb);
  TubeBundle(const CurveBundle<Vector> &cb);
  TubeBundle<Vector> &operator= (const TubeBundle<Vector> &tb);
  ~TubeBundle();

  int tubes() const;
  void newTube(const Tube<Vector>& t);
  void newTube(istream& in);
  Tube<Vector>& operator[](int c) ;

  void makeMesh(int N, int S, float R, float Tol = -1.0);

  void getBoundingBox(Vector & BBox_Min, Vector & BBox_Max);
  Vector & getCenter();
  void scaleTubeRadius(float NewRadius);

  /*
    TODO :

    scaleMesh, compute surface area and volume of the tube
    depending on coarsness of the mesh
  */

  int readPKF(const char* infile);
  int readXYZ(const char* infile);
  int readPKF(istream& in);
  int readData(const char* infile, const char* delimiter = " ");

 // friend ostream &operator<<(ostream &out, TubeBundle<Vector> &tb);

};

template <class Vector>
inline ostream &operator<<(ostream &out, TubeBundle<Vector> &tb) {

  for (int i=0;i<tb.tubes();i++)
    out << tb[i] << endl;

  return out;
}

// We have templates and need all code in same file
#include "../lib/TubeBundle.cpp"

#endif // __TUBE_BUNDLE_H__
