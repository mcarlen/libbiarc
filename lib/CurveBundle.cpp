/*!
  \class CurveBundle CurveBundle.h include/CurveBundle.h
  \ingroup BiarcLibGroup
  \brief The CurveBundle class for storing and manipulating biarc curves
  in \f$\mathcal{R}^3\f$.

  This class is used to store and manipulate a set of curves.
  This data can be interpolated to a biarc curve. The class is
  for open and closed curves, but this must be specified (how
  to do that is explained later in this text). 

  FFIXME ;some code here ...
  
  \sa Curve,Biarc

FIXME:change doc!!!
  
*/
 
//
// documentation of inlined methods
//
 
/*!
  \fn ostream & CurveBundle::operator<<(ostream &out, CurveBundle &c)

  Overloaded left shift operator. Writes the current CurveBundle object \a c
  to the ostream object \a out. If there is an interpolated curve,
  this function prints point/tangent,matching point/tangent of all
  the biarcs of each curve. For non valid biarcs only the point/tangent
  data is written to the stream.
*/

#include "../include/CurveBundle.h"

#ifndef __CURVE_BUNDLE_SRC__
#define __CURVE_BUNDLE_SRC__

/*!
  Internal function to initialize a CurveBundle object. Sets
  The header strings to the default values. The number of curves
  is set to zero.
*/
// init to Null
template<class Vector>
void CurveBundle<Vector>::init() {
  bundle.clear();
}

/*!
  Constructs an empty curve and sets the header to
  "No name","","",""
*/
template<class Vector>
CurveBundle<Vector>::CurveBundle() { init(); }

/*!
  Constructs a CurveBundle object from a portable knot format data
  file (PKF file).

  \sa readPKF().
*/
template<class Vector>
CurveBundle<Vector>::CurveBundle(const char* filename) {
  init();
  if(!readPKF(filename)) {
    cerr << "Portable Knot File Error.\n";
    exit(1);
  }
}

/*!
  Copy constructor.

  \sa operator=
*/
template<class Vector>
CurveBundle<Vector>::CurveBundle(const CurveBundle<Vector> &curve) {
  init();
  *this = curve;
}

/*!
  Assign operator. Copies the header structure from \a c. Copies
  all the curves in this bundle (i.e. all the point/tangent data
  and closes the curve if necessary).
*/
template<class Vector>
CurveBundle<Vector>& CurveBundle<Vector>::operator= (const CurveBundle<Vector> &c) {
  if (bundle.size()!=0) bundle.clear();
  for (int i=0;i<c.curves();i++)
    bundle.push_back(c.bundle[i]);
  return *this;
}

/*!
  Destructor.
*/
template<class Vector>
CurveBundle<Vector>::~CurveBundle() {
  bundle.clear();
}

/*!
  Returns the number of curves currently stored in the bundle.
*/
template<class Vector>
int CurveBundle<Vector>::curves() const {return (int)bundle.size();}

/*!
  Returns the total number of nodes of the Bundle (i.e. the sum of the
  number of nodes of each curve)
*/
template<class Vector>
int CurveBundle<Vector>::nodes() const {
  int N = bundle[0].nodes();
  for (int i=1;i<curves();i++)
    N += bundle[i].nodes();
  return N;
}


/*!
  Returns the thickness (biggest possible tube radius without self-intersection)
  of the curve bundle
  XXX : Only single component knots are supported for now
*/
template<class Vector>
float CurveBundle<Vector>::thickness() {
  assert(curves()==1);
  return bundle[0].thickness();
}

/*!
  Returns the "fast" thickness (pt radii only)
  of the curve bundle
  XXX : Only single component knots are supported for now
*/
template<class Vector>
float CurveBundle<Vector>::thickness_fast() {
  assert(curves()==1);
  return bundle[0].thickness_fast();
}


/*!
  Returns the total length of the bundle. This is the
  sum of the arc-length of all the stored curves.
*/
template<class Vector>
float CurveBundle<Vector>::length() {
  float L = 0.0;
  for (int i=0;i<curves();i++)
    L += bundle[i].length();

  return L;
}

/*!
  This is the way to close all the curves at once.
  
  \sa unlink(), Curve::link(), Curve::unlink()
*/
template<class Vector>
void CurveBundle<Vector>::link() {
  for (int i=0;i<curves();i++)
    bundle[i].link();
}

/*!
  Opens all curves in bundle.

  \sa link(), Curve::link(), Curve::unlink()
*/
template<class Vector>
void CurveBundle<Vector>::unlink() {
  for (int i=0;i<curves();i++)
    bundle[i].unlink();
}

/*!
  Add a new curve \a c to the bundle.
*/
template<class Vector>
void CurveBundle<Vector>::newCurve(Curve<Vector>& c) {
  bundle.push_back(c);
}

/*!
  Read a new curve from the stream \a in
  and add it to the bundle.
*/
template<class Vector>
void CurveBundle<Vector>::newCurve(istream &in) {
  Curve<Vector> c(in);
  newCurve(c);
}

/*!
  Takes an already valid curve object pointer and
  adds this as a component to the current Bundle
*/
template<class Vector>
void CurveBundle<Vector>::newCurve(Curve<Vector>* c) {
  newCurve(*c);
}



/*!
  Return a reference to the curve number \a c.
*/
template<class Vector>
Curve<Vector>& CurveBundle<Vector>::operator[](int c) {
  return bundle[c];
}

/*!
  Interpolate the curves in the bundle with biarcs.
  This function uses the same \f$\Gamma\f$ value for
  all curves!

  \sa Curve::make(), makeMidpointRule()
*/
template<class Vector>
void CurveBundle<Vector>::make(float f) {
  for (int i=0;i<curves();i++)
    bundle[i].make(f);
}

/*!
  Interpolate all the curves in the bundle with the midpoint
  matching rule.

  \sa Curve::make(), make()
*/
template<class Vector>
void CurveBundle<Vector>::makeMidpointRule() {
  for (int i=0;i<curves();i++)
    bundle[i].makeMidpointRule();
}

/*!
  This function resamples all curves in the bundle with
  \a NewNoNodes nodes.

  resample() can only be done if we have an interpolated
  set of curves!

  \sa refine(),make().
*/
template<class Vector>
void CurveBundle<Vector>::resample(int NewNoNodes) {
  for (int i=0;i<curves();i++)
    bundle[i].resample(NewNoNodes);
}

/*!
  Normalize the length of the sum of curve lengths! This means
  that for 10 curves with length 1 in the bundle, the length will
  be 1/10 after normalisation!

  An interpolated curve is necessary to compute the length of it.

  \sa scale(), Curve::normalize(), Curve::scale()
*/
template<class Vector>
void CurveBundle<Vector>::normalize() {
  float invL = 1.0/length();
  for (int i=0;i<curves();i++)
    bundle[i].scale(invL);
}

/*!
  Scales the length of the curves by \a s.
  The curves need not to be interpolated, since only the
  data points are changed. Returns a reference to itself.

  \sa normalize()
*/
template<class Vector>
CurveBundle<Vector>& CurveBundle<Vector>::scale(float s) {
  for (int i=0;i<curves();i++)
    bundle[i].scale(s);
  return *this;
}

/*!
  Translates all the curves by \a v.

  Redo the interpolation after this operation if
  the initial curve was biarc interpolated, since
  the matching points and bezier points are no longer
  correct.

  \sa center(), operator-=()

*/
template<class Vector>
CurveBundle<Vector>& CurveBundle<Vector>::operator+=(const Vector &v) {
  for (int i=0;i<curves();i++)
    bundle[i]+=v;
  return *this;
}

/*!
  Translate all the curves by \a -v.

  Redo the interpolation after this operation if
  the initial curve was biarc interpolated, since
  the matching points and bezier points are no longer
  correct.

  \sa operator+=()
*/
template<class Vector>
CurveBundle<Vector>& CurveBundle<Vector>::operator-=(const Vector &v) {
  for (int i=0;i<curves();i++)
    bundle[i]-=v;
  return *this;
}

/*!
  Applies the rotation specified by a rotation matrix \a m
  to each curve in the bundle. No check is done for \a m, the user must
  know what matrix he wants to apply.

  This is not the standart 4x4 transformation matrix approach
  known from homogeneous coordinates stuff. 
*/
template<class Vector>
CurveBundle<Vector>& CurveBundle<Vector>::rotate(Matrix3 &m) {
  for (int i=0;i<curves();i++)
    bundle[i].rotate(m);
  return *this;
}

/*!
  This function shifts the center of mass of the
  bundle to <0,0,0>. This is different from : centering
  each particular the curve in the bundle to <0,0,0>!!!

  \sa getCenter()
*/
template<class Vector>
void CurveBundle<Vector>::center() {
  Vector delta_center = getCenter();
  (*this) -= delta_center;
}

/*!
  Returns the bundles's center of mass.

  \sa center()
*/
template<class Vector>
Vector CurveBundle<Vector>::getCenter() {
  Vector sum(0,0,0);
  for (int i=0;i<curves();i++)
    sum += bundle[i].getCenter();
  sum /= curves();
  return sum;
}

/*!
  Read data from a PKF file \a infile. More details
  are in the class Curve documentation. This class
  can store more than 1 curve!

  Returns 1 if all went well, zero otherwise.

  \sa writePKF(),Curve::readPKF(),Curve::writePKF()
*/
template<class Vector>
int CurveBundle<Vector>::readPKF(const char *infile) {
  ifstream in(infile, ios::in);
  if (!in.good()) {
    cerr<<"CurveBundle::readPKF() : could not read " << infile <<endl;
    return 0;
  }
  int B = readPKF(in);
  in.close();
  return B;
}

/*!
  Read the curves from a stream \a in.
  More details about the PKF data format are given
  in the Curve class documentation.

  \sa writePKF(),Curve::readPKF()
*/
template<class Vector>
int CurveBundle<Vector>::readPKF(istream &in) {

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
    cerr << "at NCMP tag : You have either 0 or more"
	 << " than 1000 curves in your file!\n";
    return 0;
  }

  for (int i=0;i<NoCurves;i++)
    newCurve(in);

  return 1;
}

/*!
  Writes the current curves to a PKF file
  \a outfile. This goes through all components
  and writes them to the file.

  Returns 1 if all went well, zero otherwise.

  \sa readPKF()
*/
template<class Vector>
int CurveBundle<Vector>::writePKF(const char *outfile) {

  ofstream out(outfile,ios::trunc|ios::out);
  if(!out.good()) {
    cerr<<"CurveBundle::writePKF() : File "
	<< outfile <<" problem.\n";
    return 0;
  }
  return writePKF(out);
}

/*!
  Writes the current CurveBundle instance to the
  stream \a out. (I.e. header, number of curves and
  the curve data).

  \sa readPKF()
*/
template<class Vector>
int CurveBundle<Vector>::writePKF(ostream &out) {

  writeHeader(out);

  out << "NCMP " << curves() << endl;
  for (int i=0;i<curves();i++) {
    if (!bundle[i].writePKF(out,0)) {
      cerr << "CurveBundle::writeCurvePKF() : Problem with curve "
	   << i << endl;
      return 0;
    }
  }
	
  return 1;
}

/*!
  This function reads the curve data from a file \a infile.
  The file structure is a list of x,y,z coordinates. The default
  for the delimiter is " ", but can be changed with the second
  argument of the function.

  The first line gives the number of nodes in the following
  format {#nodes}. Then the coordinates are read in. The
  \a delimiter argument is any string that separates the
  coordinate values from each other, the default value is
  a space ' ' delimiter.

  Returns 1 if all went well, zero otherwise.

  \sa computeTangents(),polygonalToArcs(),arcsToPolygonal()

  TODO : not ready, change DOC!!!
*/
template<class Vector>
int CurveBundle<Vector>::readData(const char* infile, const char* delimiter) {

  ifstream in(infile, ios::in);
  if(!in.good()) {
    cerr<<"CurveBundle::readData() : File "
	<< infile <<" problem.\n";
    return 0;
  }

  char tmp[1024];
  in.getline(tmp,sizeof tmp);

  // Read in number of curves
  int NoCurves  = atoi(tmp);
  Curve<Vector> ctmp;
  assert(NoCurves>0);

  for (int i=0;i<NoCurves;i++) {
    if (!ctmp.readData(in,delimiter)) {
      cerr << "CurveBundle::readData() : Could not read curve " << i << " !\n";
      return 0;
    }
    newCurve(ctmp);
  }
  in.close();
  return 1;
}

/*!
  Writes the curve to a data file. First line is the number
  of points and then follows a list of x,y,z coordinates.
  If the \a tangents_flag is set to 1, the tangents of the
  points are also dropped (Default is 0).

  \sa readData()
  TODO : not ready, change doc!!!
  TODO : not ready!!!
*/
template<class Vector>
int CurveBundle<Vector>::writeData(const char* outfile, const char* delimiter,
			   int tangents_flag) {
  ofstream ofs(outfile,ios::trunc|ios::out);
  if(!ofs.good()) {
    cerr<<"CurveBundle::writeData() : File "
	<< outfile <<" problem.\n";
    return 0;
  }

  // Number of points on curve
  ofs << curves() << endl;
  for (int i=0;i<curves();i++) {
    if (!bundle[i].writeData(ofs,delimiter,tangents_flag)) {
      cerr << "CurveBundle::writeCurveData() : Problem writing curve "
	   << i << endl;
      return 0;
    }
  }
  return 1;
}

/*!
  Read a file in VECT format. Infos about the format at

  http://www.geomview.org/docs/html/geomview_42.html

  Returns 1 if all went well, zero otherwise.
  Does not support the whole VECT spec.

  \sa computeTangents(),polygonalToArcs(),arcsToPolygonal(),writeVECT()

*/
template<class Vector>
int CurveBundle<Vector>::readVECT(const char* infile) {

  ifstream in(infile, ios::in);
  if(!in.good()) {
    cerr<<"CurveBundle::readData() : File "
        << infile <<" problem.\n";
    return 0;
  }

  char tmp[1024];
  in.getline(tmp,sizeof tmp);
  if (strncmp(tmp,"VECT",4)) {
    cerr << "VECT expected, got " << tmp << endl;
    return 0;
  }

  // Read in number of curves
  in.getline(tmp,sizeof tmp);
  char *tval = strtok(tmp," ");
  char tmp_coord[1024];
  int NoCurves  = atoi(tval);
  // tval = strtok(NULL," ");
  // int NoVertices = atoi(tval);
  // ignore number of colors
  Vector v;
  Curve<Vector> ctmp;
  assert(NoCurves>0);

  // number of nodes for each
  in.getline(tmp,sizeof tmp);
  int NoNodes = atoi(strtok(tmp," "));
  in.getline(tmp_coord,sizeof tmp_coord); // skip colors per comp
  in.getline(tmp_coord,sizeof tmp_coord); // skip comment ...
  for (int i=0;i<NoCurves;i++) {
    in.getline(tmp_coord, sizeof tmp_coord); // skip Component X comment
    for (int j=0;j<abs(NoNodes);j++) {
      in.getline(tmp_coord,sizeof tmp_coord);
      v[0] = atof(strtok(tmp_coord," "));
      for (int k=1;k<(int)v.type;k++) {
        v[k] = atof(strtok(NULL," "));
      }
      ctmp.append(v,Vector3(0,0,0));
    }
    newCurve(ctmp);
    if (i<NoCurves-1)
      NoNodes = atoi(strtok(NULL," "));
  }
  // ignore the color part
  in.close();
  return 1;
}

/*!
  Not implemented
*/
template<class Vector>
int CurveBundle<Vector>::writeVECT(const char* outfile) {
  return 0;
}

/*!
  Comptes the tangents on each curve in the bundle.

  \sa Curve::computeTangents()
*/
template<class Vector>
void CurveBundle<Vector>::computeTangents() {
  for (int i=0;i<curves();i++)
    bundle[i].computeTangents();
}

/*!
  This function converts all polygonal curves into
  curves made of arcs of circles.

  \sa arcsTolPolygonal(), Curve::polygonalToArcs()
*/
template<class Vector>
void CurveBundle<Vector>::polygonalToArcs() {
  for (int i=0;i<curves();i++)
    bundle[i].polygonalToArcs();
}

/*!
  This function converts all the biarc curves into
  polygonal curves.

  \sa polygonalToArcs(),Curve::arcsToPolygonal 
*/
template<class Vector>
void CurveBundle<Vector>::arcsToPolygonal() {
  for (int i=0;i<curves();i++)
    bundle[i].arcsToPolygonal();
}

#endif //
