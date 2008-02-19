#ifndef __TUBE_H__
#define __TUBE_H__

#include <fstream>
#include <assert.h>
#include "../include/Curve.h"

#ifdef RENDERMAN
#warning libbiarc compiled with Pixie support!
#include "ri.h"
#endif

// chose the framing algo : OSCAR, FRENET, PARALLEL or WRITHEFRAME
// OSKAR is standart and works fine, the others are to be used carefully!!
#define OSCAR
// #define PARALLEL
// #define FRENET

template<class Vector>
class Tube : public Curve<Vector> {

  // number of circle segments
  int _Segments;

  // tube radius
  float _Radius;

  // tube mesh \_Segments+1 columns and \_NoNodes+1 lines
  Vector *_MeshPoints;

  // Normals for Gouraud Shading
  Vector *_MeshNormals;

  // Extensions of the knot curve
  // Bottom left corner
  Vector _BBox_Min;
  // Upper right corner
  Vector _BBox_Max;
  // Center of mass
  Vector _Center;

  // this routine is called f.ex by makeMesh or scaleTubeRadius
  // to update the current bounging box and center value
  void computeBoundingBox();

 public:

  Tube();
  Tube(istream &in);
  Tube(const Curve<Vector> &curve);
  Tube(const Tube<Vector> &tube);
  Tube &operator= (const Tube<Vector> &t);
  ~Tube();

  void init();
  void clear_tube();

  Vector & meshPoint(int i);
  Vector & meshNormal(int i);

  int segments();
  float radius();
  void makeMesh(int N, int S, float R, float Tol = -1.0);

  void getBoundingBox(Vector & BBox_BL, Vector & BBox_UR);
  Vector & getCenter();
  void scaleTubeRadius(float NewRadius);

#ifdef RENDERMAN
  RtInt NU; //  = knot->nodes()+1;
  RtInt NV; // = knot->segments()+1;
  RtPoint* MeshPoints; //[NV][NU];
  RtPoint* MeshNormals; //[NV][NU];

 public:
  void renderman_init();
  void renderman_draw();
#endif

  /*
    TODO :

    scaleMesh, compute surface area and volume of the tube
    depending on coarsness of the mesh
  */

  //friend ostream &operator<<(ostream &out, Tube<Vector> &t);

};

template<class Vector>
inline ostream &operator<<(ostream &out, Tube<Vector> &t) {

  /* FIXME : The templated class friend function doesn't work!!!!
  if (t._MeshPoints==NULL) {
    out << "-- Tube : Empty mesh --";
    return out;
  }
  */
  
  /* first and last circle point as well as
   * first and last point on curve are repeated !!
   * to keep connectivity of the mesh
   */
  if (t.isClosed())
    for (int i=0;i<=t.nodes();i++)
      for (int j=0;j<=t.segments();j++)
	out << t.meshPoint(i*(t.segments()+1)+j) << '\n';
  else
    for (int i=0;i<t.nodes();i++)
      for (int j=0;j<=t.segments();j++)
	out << t.meshPoint(i*(t.segments()+1)+j) << '\n';

  return out;
}

// Template thing
#include "../lib/Tube.cpp"

#endif // __TUBE_H__
