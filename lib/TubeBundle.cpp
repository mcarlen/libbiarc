/*!
  \class TubeBundle include/TubeBundle.h include/TubeBundle.h
  \ingroup BiarcLibGroup
  \brief The TubeBundle class contains the mesh of a tube around a
  curve in \f$\mathcal{R}^3\f$.

  This class is used to store the points and normals of a
  tubular mesh around a space-curve.

  The usage of TubeBundle objects are shown in the following example :

  \code

  #include "../include/TubeBundle.h"
  
  main(int argc, char **argv) {
  
    // Read curve data from a file
    TubeBundle t("curve.pkf");

    // Close the curve
    t.link();

    // Number of nodes on the curve
    int N = t.nodes();

    // We want 10 segments on the cross section
    int S = 10;

    // The radius is set to 1.0
    float R = 1.0;

    // We match up the mesh with this tolerance value
    float Tol = 1e-3;

    // Generate mesh points and normals
    t.makeMesh(N,S,R,Tol);

    ...

    // Write the output to standart out
    cout << t << endl;

    ...

    return 0;
  }
  \endcode
  
  \sa Curve, Biarc
  
*/
 
//
// documentation of inlined methods
//
 
/*!
  \fn ostream &TubeBundle::operator<<(ostream &out, TubeBundle &t) {

  Overloaded left shift operator. Writes all the mesh points
  to the stream \a out. For a closed curve the points on the
  circle of the first and the last node are repeated to keep
  the connectivity. Sometimes a permutation is done to diminish
  twist in the mesh. Therefore to keep the connectivity of all
  faces in the mesh, these points occur twice. As well as the
  first and the last point on the circular cross section.
  If we have S segments, there will be written S+1 points
  to the stream.
*/

#include "../include/TubeBundle.h"

#ifndef __TUBE_BUNDLE_SRC__
#define __TUBE_BUNDLE_SRC__

/*!
*/
template<class Vector>
void TubeBundle<Vector>::clear_tb() {
  container.resize(0);
}

/*!
  Initialize TubeBundle object. Number of segments and the radius
  are set to 0.0. The mesh point table and the mesh normal
  table are set to NULL.
*/
template <class Vector>
void TubeBundle<Vector>::init() {
  container.clear();
}

/* TODO : curve to tube and tube to curve correct conversion */

/*!
  Default constructor. Creates an empty tube bundle.
*/
template <class Vector>
TubeBundle<Vector>::TubeBundle() {
  init();
}

/*!
  Constructor reads the curves data from \a infile and
  stores it in a Tube container. No interpolation
  is done, and no mesh points are generated. A call
  to makeMesh() is necessary to generate the mesh points for
  all the curves.

  \sa makeMesh(),Tube::makeMesh()
*/
template <class Vector>
TubeBundle<Vector>::TubeBundle(const char* filename) {
  init();
  if(!readPKF(filename)) {
    cerr << "Portable Knot File Error.\n";
    exit(1);
  }
}

/*!
  Constructor copies a Curve object \a curve. No interpolation
  and no mesh generation is done. To do that makeMesh() must be
  called.

  \sa makeMesh()
*/
template <class Vector>
TubeBundle<Vector>::TubeBundle(const CurveBundle<Vector> &cb) {
  init();
  CurveBundle<Vector> tmp = cb;
  for (int i=0;i<tmp.curves();i++) {
    Tube<Vector> t(tmp[i]);
    this->newTube(t);
  }
}

/*!
  Constructor copies a TubeBundle object \a tube. Only the curve
  data points are copied and the curve is closed if necessary.
  No interpolation and no mesh generation is done in the new
  TubeBundle instance. To do that makeMesh() must be called.

  Maybe this will be changed in the future to copy
  all the mesh points if the \a tube object has mesh points.

  \sa makeMesh()
*/
template <class Vector>
TubeBundle<Vector>::TubeBundle(const TubeBundle<Vector> &tb) {
  init();
  *this = tb;
}

/*!
  Assign operator. Copies the curve data and
  calls link() if TubeBundle \a c is closed.
*/
template <class Vector>
TubeBundle<Vector> &TubeBundle<Vector>::operator= (const TubeBundle<Vector> &tb) {
  if (container.size()!=0) container.clear();
  for (int i=0;i<tb.tubes();i++)
    container.push_back(tb.container[i]);
  return *this;
}

/*!
  Destructor. Clears the memory used by the mesh points and normals.
*/
template <class Vector>
TubeBundle<Vector>::~TubeBundle() {
  container.clear();
}

/*!
  Add a tube \a t to the bundle.
*/
template <class Vector>
void TubeBundle<Vector>::newTube(const Tube<Vector>& t) {
  Tube<Vector> nt = t;
  container.push_back(nt);
}

/*!
  Read the curve data from a stream \a in. The
  curve is converted into a Tube object and added
  to the TubeBundle.
*/
template <class Vector>
void TubeBundle<Vector>::newTube(istream &in) {
  Tube<Vector> t(in);
  newTube(t);
}

/*!
  Return a reference to the tube number \a c
  in the TubeBundle container.
*/
template <class Vector>
Tube<Vector>& TubeBundle<Vector>::operator[](int c) {
  return container[c];
}

/*!
  Returns the number of tubes int this bundle.
*/
template <class Vector>
int TubeBundle<Vector>::tubes() const {
  return (int)container.size();
}

/*!
  Puts the position of the minimum and the maximum corner
  of a box containing the whole tubular object into the vectors
  \a BBox_Min and \a BBox_Max.
*/
template <class Vector>
void TubeBundle<Vector>::getBoundingBox(Vector & BBox_Min, Vector & BBox_Max) {

  // Return knot dimensions (computed in : makeMesh())
  BBox_Min = _BBox_Min;
  BBox_Max = _BBox_Max;

}

/*!
  Returns the center of mass of the tubular mesh.
*/
template <class Vector>
Vector & TubeBundle<Vector>::getCenter() {

  // Return center of mass of the tube !!
  return _Center;
}

// this guy is private
/*!
  Computes the bounding box of the TubeBundle instance. The
  of the bounding box touches the tube on its extremal extensions.

  This function precomputes the center and the bounding box!
  For caching purpose.

FIXME:change doc
*/
template <class Vector>
void TubeBundle<Vector>::computeBoundingBox() {

  if (tubes()<1) return;

  // Get first bounding box
  container[0].getBoundingBox(this->_BBox_Min,this->_BBox_Max);
  _Center = container[0].getCenter();

  Vector BBoxMin, BBoxMax;
  // if more than one tube
  for (int k=1;k<tubes();k++) {
    container[k].getBoundingBox(BBoxMin,BBoxMax);
    _BBox_Min = Vector(BBoxMin[0]<_BBox_Min[0]?
		       BBoxMin[0]:_BBox_Min[0],
		       BBoxMin[1]<_BBox_Min[1]?
		       BBoxMin[1]:_BBox_Min[1],
		       BBoxMin[2]<_BBox_Min[2]?
		       BBoxMin[2]:_BBox_Min[2]);
    _BBox_Max = Vector(BBoxMax[0]>_BBox_Max[0]?
		       BBoxMax[0]:_BBox_Max[0],
		       BBoxMax[1]>_BBox_Max[1]?
		       BBoxMax[1]:_BBox_Max[1],
		       BBoxMax[2]>_BBox_Max[2]?
		       BBoxMax[2]:_BBox_Max[2]);
    
    _Center += container[k].getCenter();
  }
  _Center /= tubes();
}

/*!
  Inflates or deflates the existing mesh according to
  the new radius given. All the cross sectional circles
  are changed to have radius \a NewRadius. This function
  drops an error message if there are no valid mesh points.

  The bounding box values are updated at the end of the
  process.

  FIXME:change doc
*/
template <class Vector>
void TubeBundle<Vector>::scaleTubeRadius(float NewRadius) {
  for (int i=0;i<tubes();i++)
    container[i].scaleTubeRadius(NewRadius);
}

/*!
  Creates a tubular mesh with radius \a R around the
  curve with N nodes and S segments on each cross
  sectional circle. If the actully stored curve has
  not exactly N nodes, the Curve::resample() routine
  is called automatically.

  The parameter \a Tol is the precision used to math up
  the mesh of a closed curve. \a Tol is the difference
  in radians of the angle between the first and the last
  normal of the sequence of curve frames.

  Use \a Tol = -1 (default) for no torsion adjustment and open
  curves.

  Computes also the mesh normals at each point where
  the normal is an average of the neighbouring face
  normals. (The normals are used for Gouraud shading
  in visualisation programs).

  At the end the center of mass and the bounding box
  are computed.

  The interpolation in case of a resampling is done
  with Gamma = 0.5.

FIXME ; change doc
*/
template <class Vector>
void TubeBundle<Vector>::makeMesh(int N, int S, float R, float Tol) {
  for (int i=0;i<tubes();i++)
    container[i].makeMesh(N,S,R,Tol);
  // cache bounding box and center
  computeBoundingBox();
}

/*!
  FIXME doc
*/
template <class Vector>
int TubeBundle<Vector>::readPKF(const char* infile) {
  ifstream in(infile, ios::in);
  if (!in.good()) {
    cerr << "TubeBundle::readPKF() : could not read " << infile << endl;
    return 0;
  }
  int B = readPKF(in);
  in.close();
  return B;
}

/*!
  FIXME doc
*/
template <class Vector>
int TubeBundle<Vector>::readPKF(istream& in) {
  readHeader(in);

  char tmp[1024];

  // Read NoComp and NoNodes
  in.getline(tmp,sizeof tmp);
  if(strncmp(tmp,"NCMP ",5)) {
    cerr << "Expected NCMP: " << tmp << '\n';
    return 0;
  }
  int NoCurves = atoi(tmp+5);
  if (NoCurves<1||NoCurves>1000) {
    cerr << "at NCMP tag : You have either 0 or more\n"
	 << " than 1000 curves in your file!\n";
    return 0;
  }

  for (int i=0;i<NoCurves;i++)
    this->newTube(in);

  return 1;
}

/*!
  FIXME doc
*/
template <class Vector>
int TubeBundle<Vector>::readData(const char* infile, const char* delimiter) {
  ifstream in(infile, ios::in);
  if(!in.good()) {
    cerr<<"TubeBundle::readData() : File "
	<< infile <<" problem.\n";
    return 0;
  }

  char tmp[1024];
  in.getline(tmp,sizeof tmp);

  // Read in number of curves
  int NoCurves  = atoi(tmp);

  Tube<Vector> ttmp;
  for (int i=0;i<NoCurves;i++) {
    if (!ttmp.readData(in,delimiter)) {
      cerr << "TubeBundle::readData() : Could not read curve " << i << " !\n";
      return 0;
    }
    newTube(ttmp);
  }
  in.close();
  return 1;
}

/*!
  Read XYZ file.

  \sa Curve::readXYZ()
*/
template <class Vector>
int TubeBundle<Vector>::readXYZ(const char* infile) {
  ifstream in(infile, ios::in);
  if (!in.good()) {
    cerr<<"TubeBundle::readXYZ() : could not read " << infile <<endl;
    return 0;
  }
  Tube<Vector> t;
  int B = t.readXYZ(in);
  in.close();
  newTube(t);
  return B;
}


#endif // __TUBE_BUNDLE_SRC__
