/*!
  \class Biarc include/Biarc.h include/Biarc.h
  \ingroup BiarcLibGroup
  \brief The Biarc class describes a single biarc.

  This class is used to store a single data point of an
  entire space curve in \f$\mathcal{R}^3\f$. A biarc
  object contains a point and the tangent at this particular
  point. Aligning one biarc after another makes up a biarc
  approximated space-curve.

  The space-curve is a linked-list of Biarc elements. Every
  biarc element in the "curve list" has two neighbours on its
  left and right side, reachable by getNext() and getPrevious().
  If the point is the start or endpoint of an open curve, then
  it will only have a single neighbour. The other pointer will
  be set to NULL.
  
  \sa Curve
  
*/
 
//
// documentation of inlined methods
//
 
/*!
  \fn ostream & Biarc<Vector>::operator<< (ostream &out, const Biarc<Vector> &b)

  Overloaded left shift operator. Returns the Biarc \a b as an ostream
  object that can be written to a file or to standart output.

  \sa print()
*/

#include "../include/Biarc.h"

// Templatized code needs to be kept in the same
// file, that's why we include Biarc.cpp after Biarc.h
// and therefore the #define here!!!!!!
#ifndef __BIARC_SRC__
#define __BIARC_SRC__


/*!
  Default constructor. Initializes this instance of a Biarc to <0,0,0> for the
  point and <0,1,0> for the tangent.
*/
template<class Vector>
Biarc<Vector>::Biarc() :
  _BIARC_(0),
  _Point(0,0,0),_Tangent(0,1,0),
  _BiarcInCurve(-1), _Curve(NULL)
{}

/*!
  Constructor sets the point to \a p and the tangent to \a t.
  Normalizes the tangent.
*/
template<class Vector>
Biarc<Vector>::Biarc(const Vector &p,const Vector &t) :
  _BIARC_(0),
  _Point(p),_Tangent(t),
  _BiarcInCurve(-1) ,_Curve(NULL)
{
  _Tangent.normalize();
}

/*!
  Copy Constructor. Copies only the point and the tangent.
  Doesn't care if we have an interpolated biarc or not.
*/
template<class Vector>
Biarc<Vector>::Biarc(const Biarc<Vector>& b) :
  _BIARC_(b._BIARC_),
  _Point(b._Point), _Tangent(b._Tangent),
  _BiarcInCurve(b._BiarcInCurve), _Curve(b._Curve) 
{
  _Tangent.normalize();
}

/*!
  Destructor.
*/
template<class Vector>
Biarc<Vector>::~Biarc() {}

/*!
  Returns the point of the biarc as a Vector.
*/
template<class Vector>
const Vector& Biarc<Vector>::getPoint() const {
  return _Point;
}

/*!
  Returns the tangent of the biarc as a Vector.
*/
template<class Vector>
const Vector& Biarc<Vector>::getTangent() const {
  return _Tangent;
}

/*!
  Returns the matching point of this biarc, if we have
  interpolated biarcs. Posts a warning message and returns a
  zero vector if not.
*/
template<class Vector>
const Vector &Biarc<Vector>::getMidPoint() const {
  if (!_BIARC_) {
    cerr << "Biarc::getMidPoint() : Not a valid biarc.\n";
    // Return data point
    return _Point;
  }
  return _MidPoint;
}

/*!
  Returns the matching tangent of this biarc, if we have
  interpolated biarcs. Posts a warning message and returns a
  zero vector if this is not a valid biarc.
*/
template<class Vector>
const Vector &Biarc<Vector>::getMidTangent() const {
  if (!_BIARC_) {
    cerr << "Biarc::getMidTangent() : Not a valid biarc.\n";
    // Return data tangent
    return _Tangent;
  }
  return _MidTangent;
}

/*!
  Set point to \a p.
*/
template<class Vector>
void Biarc<Vector>::setPoint(const Vector &p) {
  _Point = p;
}

/*
  Set point to \a <p0,p1,p2>.
*/
/* We do it more general than with simply 3 components ...
template<class Vector>
void Biarc<Vector>::setPoint(float p0,float p1,float p2) {
  _Point=Vector3(p0,p1,p2);
}
*/

/*!
  Set midpoint to \a p.
*/
template<class Vector>
void Biarc<Vector>::setMidPoint(const Vector &p) {
  _MidPoint = p;
}

/*!
  Set midtangent to \a p.
*/
template<class Vector>
void Biarc<Vector>::setMidTangent(const Vector &p) {
  _MidTangent = p;
  // this should go somewhere else. where we init the Bezier points
  // as well.
  _BIARC_ = 1;
  Vector q0 = _Point, q1 = getNext().getPoint();
  Vector t0 = _Tangent, t1 = getNext().getTangent();
  _BezierPoint0 = q0+(_MidPoint-q0).norm2()/2./(_MidPoint-q0).dot(t0)*t0;
  _BezierPoint1 = _MidPoint+(q1-_MidPoint).norm2()/2./(q1-_MidPoint).dot(_MidTangent)*_MidTangent;
  cache();
}

/*!
  Set tangent of the biarc. No normalization of the tangent is done.
*/
template<class Vector>
void Biarc<Vector>::setTangentUnnormalized(const Vector &t) {
  _Tangent = t;
}

/*!
  Set tangent of the biarc to \a t. Tangent automatically normalized.
*/
template<class Vector>
void Biarc<Vector>::setTangent(const Vector &t) {
  _Tangent = t;
  _Tangent.normalize();
}

/*
  Set tangent of the biarc to \a <t0,t1,t2>.
*/
/* no longer exists
template<class Vector>
void Biarc<Vector>::setTangent(float t0,float t1,float t2) {
  _Tangent = Vector3(t0,t1,t2);
  _Tangent.normalize();
}
*/

/*!
  Recover point/tangent data from this biarc and put it into \a p and \a t.
*/
template<class Vector>
void Biarc<Vector>::get(Vector& p, Vector& t) const {
  p = _Point;
  t = _Tangent;
}

/*!
  Set point/tangent data for this biarc to \a p and \a t.
*/
template<class Vector>
void Biarc<Vector>::set(const Vector &p, const Vector &t) {
  _Point = p;
  _Tangent = t;
}

/*!
  Sets the biarc data to : point <0,0,0> and tangent <0,1,0>.
*/
template<class Vector>
void Biarc<Vector>::clear() {
  _Tangent = Vector();
  _Point = Vector();
}

/*!
  Sets the biarc point P to -P.
*/
template<class Vector>
void Biarc<Vector>::reverse() {
  _Point *= -1.0;
}

/*!
  Checks if the Biarc is proper. Returns 1 if this is true.
  Posts a warning message and returns 0 if the biarc is not proper.

  A biarc is proper, if the point-tangent data pair \f$((p_0,t_0),(p_1,t_1))\f$
  satifies

  \f$(p_1-p_0)\cdot t_0 > 0,\f$ and \f$(p_1-p_0)\cdot t_1 > 0.\f$

  This implies that for a Biarc \a b with point \a p and tangent \a t
  and its following biarc (obtained with Biarc::getNext()), with point
  \a p' and \a tangent \a t', the previous condition must hold.

  \sa getNext(), getPrevious().
*/
template<class Vector>
int Biarc<Vector>::isProper() const {
  // check if we got one more on the curve
  return (((getNext().getPoint()-_Point).dot(_Tangent)>0) &&
        ((getNext().getPoint()-_Point).dot(getNext().getTangent())>0));
}

/*!
  Returns true (=1) if the current biarc is interpolated, and 0 if not.
  Here interpolated means, that a valid matching point has been computed
  using this biarc and its following neighbour.
*/
template<class Vector>
int Biarc<Vector>::isBiarc() const {
  return _BIARC_;
}

/*!
  Sets the "interpolated" flag to be true.
*/
template<class Vector>
void Biarc<Vector>::setBiarc() {
  _BIARC_ = 1;
}

/*!
  This function computes the matching point of
  this biarc. It uses the point/tangent data of current biarc
  as starting point and the point/tangent data of the next
  biarc as the end point.

  Giving a value for \a Gamma, it is possible to compute a
  point (matching point), that links the two data points by
  two circular arcs of circles. For each arc the control points
  of a Bezier curve are computed and can be recovered by getBezier(). 

  The appropriate rule that computes the matching point is
  still an open question (since the matching point of a biarc
  is not unique but located on a particular arc of circle defined
  by the data points and tangents).

  For now, this function takes as input \a Gamma, where \a Gamma
  is a parameter related to the ratio of the Bezier triangles
  over the two arcs of circles.

  More details can be found in Jana Smutny's PhD Thesis at

  http://lcvmwww.epfl.ch/~lcvm/articles/thesis.html

  \sa getBezier(),getBezierArc0(),getBezierArc1(),Curve::make()
*/
template<class Vector>
void Biarc<Vector>::make(float Gamma) {

  // is the biarc proper?
  if(!isProper()) {
    cerr << "Biarc::make(float Gamma) : Biarc " << (*this) << " is not proper\n";
    return;
  }

  // we have to check if MidpointRule betw. 0 and 1
  if(Gamma < 0.0 || Gamma > 1.0) {
    cerr << "Biarc::make(float Gamma) : Wrong Gamma value :"
         << Gamma << endl;
    return;
  }

  // declare temporary variables
  Vector q0 = _Point,   q1 = getNext().getPoint();
  Vector t0 = _Tangent, t1 = getNext().getTangent();
  Vector d  = q1 - q0;
  Vector b0, b2, t, q;

  Vector linecheck = q0-q1;
  linecheck.normalize();

  t0.normalize(); t1.normalize();

  if (fabsf(t0.dot(t1)>(1.0-StraightSegTol)) &&
      fabsf(t0.dot(linecheck))>(1.0-StraightSegTol)) {

    // cerr << "Straight Segment! : " << *this << endl;

    float GammaBar = 1 - Gamma;

    _MidPoint = (_Point*GammaBar + getNext().getPoint()*Gamma);
    _MidPoint /= (Gamma+GammaBar);

    _MidTangent = (_Tangent+getNext().getTangent())/2;
    _MidTangent.normalize();

    float d = (getNext().getPoint() - _Point).norm();
    _BezierPoint0 = _Point + _Tangent*Gamma*d/2.0;
    _BezierPoint1 = getNext().getPoint() - getNext().getTangent()*GammaBar*d/2.0;

    // set radius to infinity since straight segment!!
    _Radius0 = -1.0;
    _Radius1 = -1.0;
    _ArcLength0 = (_MidPoint-_Point).norm();
    _ArcLength1 = (getNext().getPoint()-_MidPoint).norm();
    _Length = _ArcLength0+_ArcLength1;

    _BIARC_ = 1;
    return;
  }

  // The biarc parameters are not independent
  float val = Gamma*(1-t0.dot(t1))*d.norm2()+2*t0.dot(d)*t1.dot(d);
  float GammaBar = 2*(1-Gamma)*t0.dot(d)*t1.dot(d)/val;

  // Compute Midpoint with given Gamma/GammaBar
  _MidPoint = (q0*t0.dot(d)*GammaBar + q1*t1.dot(d)*Gamma)/(t1.dot(d)*Gamma + t0.dot(d)*GammaBar);
  _MidPoint += ( (t0-t1)*d.norm2()*Gamma*GammaBar/ 2.0 / (t1.dot(d)*Gamma + t0.dot(d)*GammaBar) );

  // Compute matching tangent
  Vector m = _MidPoint;
  Vector e_m = (m-q0)/(m-q0).norm();
  _MidTangent = e_m*2*e_m.dot(t0) - t0;

  // Bezier midpoint b1 for arc 0 and 1
  // i.e. set _BezierPoint0 and _BezierPoint1
  for (int i=0;i<2;i++)
    (i?_BezierPoint1:_BezierPoint0) = 
      (i?q1:q0) + (i?t1:t0)*(d.norm2()/(2*(i?t1:t0).dot(d)))*(i?-GammaBar:Gamma);

  // compute radius,arclength of arc0 and arc1
  cache();

  // Now this biarc is valid
  _BIARC_ = 1;

}

/*!
  Private function that computes the radius of the arc of circle given
  by two points and a tangent. Returns -1 if the segment is a straight
  line (up to the StraightSegTol tolerance)
*/
template<class Vector>
float Biarc<Vector>::_radius(const Vector &q0, const Vector &q1, const Vector &t0) const {
  // should already be normalized
  // t0.normalize();
  Vector d = q1-q0; Vector e = q1-q0;
  e.normalize();

  float R2 = d.norm2()/4.0;

  if ((1.-e.dot(t0)*e.dot(t0))< StraightSegTol)
    return -1.;
  R2/=(1.-e.dot(t0)*e.dot(t0));

  return sqrt(R2);
}

/*!
  Compute and return the radius of the first arc.
  Internal function that is used by the cache() funtion.
*/
template<class Vector>
float Biarc<Vector>::_radius0() const {
  return _radius(_Point,_MidPoint,_Tangent);
}

/*!
  Compute and return radius of the second arc.
  Internal function that is used by the cache() funtion.
*/
template<class Vector>
float Biarc<Vector>::_radius1() const {
  return _radius(_MidPoint,getNext().getPoint(),_MidTangent);
}

/*!
  Compute and return the radius of the first arc.
*/
template<class Vector>
float Biarc<Vector>::radius0() const {
  return _Radius0;
}

/*!
  Compute and return the radius of the second arc.
*/
template<class Vector>
float Biarc<Vector>::radius1() const {
  return _Radius1;
}

/*!
  Compute and return the angle swept out by an arc of circle.
  The circle is given by start and endpoint and a tangent.
*/
template<class Vector>
float Biarc<Vector>::_angle(const Vector &q0, const Vector &q1, const Vector &t0) const {
  Vector d = q1-q0;
  d.normalize();
  // should already be normalized
  // t0.normalize();
  float tmp = d.dot(t0);

  return 2*acos(tmp);
}

/*!
  Computes and returns the angle swept out by arc of circle one.
*/
template<class Vector>
float Biarc<Vector>::_angle0() const {
  return _angle(_Point,_MidPoint,_Tangent);
}

/*!
  Computes and returns the angle swept out by arc of circle two.
*/
template<class Vector>
float Biarc<Vector>::_angle1() const {
  return _angle(_MidPoint,getNext().getPoint(),_MidTangent);
}

/*!
  Computes and returns the arclength of the first arc of circle
  of the current biarc.
*/
template<class Vector>
float Biarc<Vector>::_arclength0() const {
  if (_radius0()<0)
    return (_MidPoint - _Point).norm();
  else
    return (_radius0()*_angle0());
}

/*!
  Computes and returns the arclength of the second arc of circle
  of the current biarc.
*/
template<class Vector>
float Biarc<Vector>::_arclength1() const {
  if (_radius1()<0)
    return (getNext().getPoint() - _MidPoint).norm();
  else 
    return (_radius1()*_angle1());
}

/*!
  Precomputes for both arcs of circles the radius and
  the arclength.

  \sa arclength0(), arclength1(), biarclength().
*/
template<class Vector>
void Biarc<Vector>::cache() {
  _Radius0 = _radius0();
  _Radius1 = _radius1();
  _ArcLength0 = _arclength0();
  _ArcLength1 = _arclength1();
  _Length = _ArcLength0+_ArcLength1;
  /*
  cout << "Cache : rad " << _Radius0 << " " <<_Radius1 <<endl
       << "len " << _ArcLength0 << " " << _ArcLength1 << " " << _Length << endl;
  */
}

/*!
  Returns the arclength of the first arc of circle.

  \sa arclength1(), biarclength().
*/
template<class Vector>
float Biarc<Vector>::arclength0() const {
  return _ArcLength0;
}

/*!
  Returns the arclength of the second arc of circle.

  \sa arclength0(), biarclength().
*/
template<class Vector>
float Biarc<Vector>::arclength1() const {
  return _ArcLength1;
}

/*!
  Returns the arclength of the current biarc. This is
  the sum of the arcs one and two.

  \sa arclength0(), arclength1().
*/
template<class Vector>
float Biarc<Vector>::biarclength() const {
  return _Length;
}

/*!
  This function is used to extract the Bezier curve control points
  of the first arc of circle of the biarc and put them into
  b0,b1,b2.

  \sa getBezier(),getBezierArc1()
*/
template<class Vector>
void Biarc<Vector>::getBezierArc0(Vector& b0, Vector& b1, Vector& b2) const {
  if (!isBiarc()) {
    cerr << "Biarc::getBezierArc0(...) : Biarc not valid!\n";
    return;
  }
  b0 = _Point;
  b1 = _BezierPoint0;
  b2 = _MidPoint;
}

/*!
  This function is used to extract the Bezier curve control points
  of the second arc of circle of the biarc and put them into
  b0,b1,b2.

  \sa getBezier(),getBezierArc0()
*/
template<class Vector>
void Biarc<Vector>::getBezierArc1(Vector& b0, Vector& b1, Vector& b2) const {
  if (!isBiarc()) {
    cerr << "Biarc::getBezierArc0(...) : Biarc not valid!\n";
    return;
  }
  b0 = _MidPoint;
  b1 = _BezierPoint1;
  b2 = getNext().getPoint();
}

/*!
  This method is used to get the Bezier representation of
  the biarc. The corners of an isosceles triangle over a
  circular arc are known as the Bezier points.

  From a biarc interpolated curve it is easy to compute the
  Bezier curve control points for each particular arc.

  The Bezier points of arc one comes into the Vector objects
  \a b0_0,b1_0,b2_0 and arc two comes in \a b0_1,b1_1,b2_1.

  \sa getBezierArc0(),getBezierArc1()
*/
template<class Vector>
void Biarc<Vector>::getBezier(Vector& b0_0, Vector& b1_0, Vector& b2_0,
		      Vector& b0_1, Vector& b1_1, Vector& b2_1) const {
  getBezierArc0(b0_0,b1_0,b2_0);
  getBezierArc1(b0_1,b1_1,b2_1);
}

/*!
  Returns the point on a circular arc (given by the Bezier
  triangle b0,b1,b2) for a given \a tau, which is in [0,1].
  Caution : \a tau is not an arclength parametrization.
  The public functions pointOnArc0() and pointOnArc1() are
  used for that purpose.
*/
template<class Vector>
Vector Biarc<Vector>::a(const Vector &b0, const Vector &b1,
	  	        const Vector &b2, float tau) const {

  float omega = (b1-b0).dot(b2-b0)/(b1-b0).norm()/(b2-b0).norm();

  // Return point on Bezier arc a(tau)
  Vector tmp = b0*(1.0-tau)*(1.0-tau)+b1*2.0*omega*tau*(1.0-tau)+b2*tau*tau;
  float tmp2 = (1-tau)*(1.0-tau)+2.0*omega*tau*(1.0-tau)+tau*tau;
  return tmp/tmp2;

}

/*!
  Given a value \a tau between 0 and 1, this function returns
  the point \f$a(\tau)\f$ on the first arc of circle of the biarc.
  Where \f$a(\tau)\f$ is a non-arclength parametrization of an
  arc of circle.

  For arc-length parametrized arcs use one of the pointOn*
  functions.

  \sa pointOnArc0(),pointOnArc1(),pointOnBiarc(),a1().
*/
template<class Vector>
Vector Biarc<Vector>::a0(float tau) const {
  if (_BIARC_ && tau<=1.0 && tau>=0.0)
    return a(_Point,_BezierPoint0,_MidPoint,tau);
  else {
    cerr << "Biarc::a0(float) : "
	 << (_BIARC_?"tau value problem\n":"not a valid biarc\n");
    return Vector();
  }
}

/*!
  Given a value \a tau between 0 and 1, this function returns
  the point \f$a(\tau)\f$ on the second arc of circle of the biarc.
  Where \f$a(\tau)\f$ is a non-arclength parametrization of an
  arc of circle.
  
  For arc-length parametrized arcs use one of the pointOn*
  functions.

  \sa pointOnArc0(),pointOnArc1(),pointOnBiarc(),a0().
*/
template<class Vector>
Vector Biarc<Vector>::a1(float tau) const {
  if (_BIARC_ && tau<=1.0 && tau>=0.0) {
    return a(_MidPoint,_BezierPoint1,getNext().getPoint(),tau);
  }  else {
    cerr << "Biarc::a1(float) : "
	 << (_BIARC_?"tau value problem\n":"not a valid biarc\n");
    return Vector();
  }
}

/*!
  Returns the position if this Biarcs is in a Curve object.
  -1 otherwise.
*/
template<class Vector>
inline int Biarc<Vector>::id() const {
  return _BiarcInCurve;
}

/*!
  Private routine. Sets _BiarcInCurve value.
*/
template<class Vector>
inline void Biarc<Vector>::setId(const int i) {
  _BiarcInCurve = i;
}

/*!
  Private routine. Get curve pointer.
*/
template<class Vector>
inline const Curve<Vector>* Biarc<Vector>::getCurve() {
  return _Curve;
}

/*!
  Private routine. Sets _Curve value.
*/
template<class Vector>
inline void Biarc<Vector>::setCurve(Curve<Vector>* c) {
  _Curve = c;
}


/*!
  Private routine. Sets _BiarcInCurve and _Curve values.
*/
template<class Vector>
inline void Biarc<Vector>::setIdAndCurve(const int i, Curve<Vector>* c) {
  _BiarcInCurve = i;
  _Curve = c;
}

/*!
  Returns point \f$a(s)\f$ on arc one of the biarc, here
  \a s is arc-length parametrization.

  We have \f$s\in [0,l_0]\f$, where \f$l_0\f$ is the
  arc-length of the current biarc's first arc.

  No check is done if we have a valid biarc interpolation!

  \sa pointOnBiarc(),pointOnArc1().
*/
template<class Vector>
Vector Biarc<Vector>::pointOnArc0(float s) const {
  if (s==0) return _Point;
  if (s==_ArcLength0) return _MidPoint;

  // Here we map the non-arclength parametrization
  // tau to arclength param s.
  // The idea is to project point at s on the
  // arc onto the arc's cord (normalized by the
  // cord length to get the tau in [0,1]!!

  Vector3 P_s(-_Radius0*cos(s/_Radius0),
	      _Radius0*sin(s/_Radius0),
	      0);
  Vector3 P_l(-_Radius0*cos(_ArcLength0/_Radius0),
	      _Radius0*sin(_ArcLength0/_Radius0),
	      0);

  Vector3 offset(_Radius0,0,0);

  P_s+=offset; P_l+=offset;

  float scale_factor = P_l.norm();
  P_l.normalize();

  float tau = 0;
  if (P_s.dot(P_l)>10e-8)
    tau = P_s.dot(P_l)/scale_factor;

  return a0(tau);

/*
  This version is buggy
////
  Vector b0 = _Point;
  Vector b1 = _BezierPoint0;
  Vector b2 = _MidPoint;

  Vector t0 = b1-b0, t2 = b2-b1;
  t0.normalize(); t2.normalize();
  Vector n = t0.cross(t2); // n.normalize();
  Vector bin = n.cross(t0); // bin.normalize();

  Vector Center = b0 + bin*_Radius0;

  return ((b0-Center).rotPtAroundAxis(s/_Radius0,n)+Center);
*/
}

/*!
  Returns point \f$a(s)\f$ on arc one of the biarc, here
  \a s is arc-length parametrization.

  We have \f$s\in [0,l_1]\f$, where \f$l_1\f$ is the
  arc-length of the current biarc's second arc.

  No check is done if we have a valid biarc interpolation!

  \sa pointOnBiarc(),pointOnArc0().
*/
template<class Vector>
Vector Biarc<Vector>::pointOnArc1(float s) const {
  if (s==0) return _MidPoint;
  if (s==_ArcLength1) return getNext().getPoint();

  // Mapping to [0,1] for tau is explained in pointOnArc0()

  Vector3 P_s(-_Radius1*cos(s/_Radius1),
	      _Radius1*sin(s/_Radius1),
	      0);
  Vector3 P_l(-_Radius1*cos(_ArcLength1/_Radius1),
	      _Radius1*sin(_ArcLength1/_Radius1),
	      0);

  Vector3 offset(_Radius1,0,0);

  P_s+=offset; P_l+=offset;

  float scale_factor = P_l.norm();
  P_l.normalize();

  float tau = 0;
  if (P_s.dot(P_l)>10e-8)
    tau = P_s.dot(P_l)/scale_factor;

  return a1(tau);

/*
  This Version is buggy
////
  Vector b0 = _MidPoint;
  Vector b1 = _BezierPoint1;
  Vector b2 = getNext().getPoint();
  Vector t0 = b1-b0, t2 = b2-b1;
  t0.normalize(); t2.normalize();
  Vector n = t0.cross(t2); n.normalize();
  Vector bin = n.cross(t0); bin.normalize();

  Vector Center = b0 + bin*_Radius1;

  return ((b0-Center).rotPtAroundAxis(s/_Radius1,n)+Center);
*/
}

/*!
  Returns the point \f$a(s)\f$ on the biarc. Here
  \a s is arc-length parametrization.

  We have \f$s\in [0,l]\f$, where \f$l\f$ is the
  total arc-length of the current biarc.

  \sa pointOnArc0(),pointOnArc1().
*/
template<class Vector>
Vector Biarc<Vector>::pointOnBiarc(float arclength) const {

  if (arclength>_Length) {
    cerr << "Biarc::pointOnBiarc(float) : value out of bounds\n";
    return Vector();
  }

  if (!_BIARC_) {
    cerr << "Biarc::pointOnBiarc(float) : Not a valid biarc\n";
    return Vector();
  }

  if (_Radius0 <0 || _Radius1 <0) {
    // cerr << "STRAIGHT LINE\n";
    return (_Point + (getNext().getPoint()-_Point)*arclength/_Length);
  }

  if (arclength<=_ArcLength0)
    return pointOnArc0(arclength);
  else {
    return pointOnArc1(arclength-_ArcLength0);
  }
}

/*!
  Returns tangent vector at a(s) on the biarc
*/
template<class Vector>
Vector Biarc<Vector>::tangentOnBiarc(float arclength) const {
  if (arclength>_Length) {
    cerr << "Biarc::tangentOnBiarc(float) : val out of bounds\n";
    return Vector();
  }
  if (!_BIARC_) {
    cerr << "Biarc::tangentOnBiarc(float) : Not a valid biarc\n";
    return Vector();
  }

  if (_Radius0 <0 || _Radius1 <0) {
    // cerr << "STRAIGHT LINE\n";
    Vector tan = _Tangent + (getNext().getTangent()-_Tangent)*(arclength/_Length);
    tan.normalize();
    return tan;
  }

  Vector p = pointOnBiarc(arclength);
  Vector t;
  if (arclength==0.0) return _Tangent;
  else if (arclength==_ArcLength0) return _MidTangent;
  else if (arclength==_Length) return getNext().getTangent();
  else if (arclength > this->arclength0()) {
    if (arclength > this->arclength0()+this->arclength1()/2.0)
      t = this->getMidTangent().reflect(p-this->getMidPoint());
    else
      t = this->getNext().getTangent().reflect(this->getNext().getPoint()-p);
  }
  else {
    if (arclength > this->arclength0()/2.0)
      t = this->getTangent().reflect(p-this->getPoint());
    else
      t = this->getMidTangent().reflect(this->getMidPoint()-p);
  }
  t.normalize();
  return t;
}

/*!
  Returns normal vector at a(s) on the biarc
  0,0,0 if not defined (straight segment).
*/
template<class Vector>
Vector Biarc<Vector>::normalOnBiarc(float arclength) const {
  if (arclength>_Length || arclength < 0.0) {
    cerr << "Biarc::tangentOnBiarc(float) : val out of bounds\n";
    return Vector();
  }
  if (!_BIARC_) {
    cerr << "Biarc::tangentOnBiarc(float) : Not a valid biarc\n";
    return Vector();
  }

  if (_Radius0 <=1e-12 || _Radius1 <=1e-12) {
    // cerr << "STRAIGHT LINE\n";
    return Vector(0,0,0);
  }

  Vector t, c;

  //Calculate center c of arc 
  if (arclength <= this->arclength0()) {
     Vector x=this->getMidPoint() - this->getPoint();
     x.normalize();
     Vector y=x-x.dot(_Tangent)*_Tangent;
     y.normalize();
     c = this->getPoint() + _Radius0*y;
  } else {
     Vector x=this->getMidPoint()-this->getNext().getPoint();
     x.normalize();
     Vector y=x-x.dot(this->getNext().getTangent())*this->getNext().getTangent();
     y.normalize();
     c = this->getNext().getPoint() + _Radius1*y;
  }
  //__HG
  Vector n = c-pointOnBiarc(arclength);
  n.normalize();
  return n;
}



/*!
  Returns a pointer to the next biarc. We get
  NULL if the current biarc is the last point
  on the curve (i.e. the end of the linked-list).

  \sa getPrevious(),setNext(),setPrevious(),setNextNULL(),setPreviousNULL().
*/
template<class Vector>
const Biarc<Vector>& Biarc<Vector>::getNext() const { 
  if (_Curve==NULL) cout << "C problem\n";
  return _Curve->getNext(_BiarcInCurve);
}

/*!
  Returns a pointer to the previous biarc. We get
  NULL if the current biarc is the first point
  of the curve (i.e. the first element of the linked-list).

  \sa getNext(),setNext(),setPrevious(),setNextNULL(),setPreviousNULL().
*/
template<class Vector>
const Biarc<Vector>& Biarc<Vector>::getPrevious() const {
 return _Curve->getPrevious(_BiarcInCurve);
}

/*!
  Sets the next neighbour of this biarc to \a b.

  \sa getNext(),getPrevious(),setPrevious(),setNextNULL(),setPreviousNULL().
*/
template<class Vector>
void Biarc<Vector>::setNext(const Biarc &b) {
  _Curve->setNext(_BiarcInCurve,b);
}

/*!
  Sets the previous neighbour of this biarc to \a b.

  \sa getNext(),getPrevious(),setPrevious(),setNextNULL(),setPreviousNULL().
*/
template<class Vector>
void Biarc<Vector>::setPrevious(const Biarc &b) {
  _Curve->setPrevious(_BiarcInCurve,b);
}

/*!
  Deletes the link to the next neighbour by setting
  the pointer to NULL.

  \sa getNext(),getPrevious(),setNext(),setPrevious(),setPreviousNULL().
*/
// Reimplement with stl vector
template<class Vector>
void Biarc<Vector>::setNextNULL() { 
  // Next = NULL;
}

/*!
  Deletes the link to the previous neighbour by setting
  the pointer to NULL.

  \sa getNext(),getPrevious(),setNext(),setPrevious(),setNextNULL().
*/
// reimplement stl vec
template<class Vector>
void Biarc<Vector>::setPreviousNULL() {
  // _Previous = NULL;
}

/*!
  Returns a biarc whose point value is multiplied by \a s.
*/
template<class Vector>
Biarc<Vector> Biarc<Vector>::operator*(const float s) const {
  return Biarc(_Point*s,_Tangent);
}

// not used yet
/* Friend
Biarc Biarc::operator *(Biarc & b, float d) {
  return Biarc(b.GetPoint()*d,b.GetTangent());
}

Biarc Biarc::operator *(float d, Biarc & b) {
  return Biarc(b.GetPoint()*d,b.GetTangent());
}

Biarc Biarc::operator /(Biarc & b, float d) {
  return Biarc(b.GetPoint()/d,b.GetTangent());
}
*/

/*!
  Returns a by \a v translated biarc. Tangent remains unchanged.
*/
template<class Vector>
Biarc<Vector> Biarc<Vector>::operator+(const Vector &v) const{
  return Biarc(_Point + v,_Tangent);
}

/*!
  Returns a by \a -v translated biarc. Tangent remains unchanged.
*/
template<class Vector>
Biarc<Vector> Biarc<Vector>::operator-(const Vector &v) const{
  return Biarc(_Point - v,_Tangent);
}

/*!
  Assign operator. Copies the values of the point and
  the tangent and sets the biarc flag _BIARC_ to zero.
*/
template<class Vector>
Biarc<Vector>& Biarc<Vector>::operator=(const Biarc &b) {
  _BIARC_=b._BIARC_;
  _Point=b._Point;
  _Tangent=b._Tangent;
  _Curve=b._Curve;
  return (*this);
}

/*!
  Adds \a v to the point value of this biarc and returns
  an instance to itself.
*/
template<class Vector>
Biarc<Vector>& Biarc<Vector>::operator+=(const Vector &v) {
  _Point += v;
  return *this;
}

/*!
  Subtracts \a v from the point value of this biarc and returns
  an instance to itself.
*/
template<class Vector>
Biarc<Vector>& Biarc<Vector>::operator-=(const Vector &v) {
  _Point -= v;
  return *this;
}

/*!
  Divides the point value of this biarc by \a d and returns
  an instance to itself.
*/
template<class Vector>
Biarc<Vector>& Biarc<Vector>::operator/=(const float d) {
  _Point /= d;
  return *this;
}

/*!
  Multiplies the point value of this biarc by \a d and returns
  an instance to itself.
*/
template<class Vector>
Biarc<Vector>& Biarc<Vector>::operator*=(const float d) {
  _Point *= d;
  return *this;
}

/*!
  Compares this instance to a biarc \a b. Returns 1 if both
  biarcs agree in point and tangent, 0 otherwise.
*/
template<class Vector>
int Biarc<Vector>::operator==(const Biarc &b) const {
  return ( _Point==b._Point && _Tangent==b._Tangent);
};

/*!
  Compares this instance to a biarc \a b. Returns 1 if the
  biarcs disagree in point or tangent or both, 0 otherwise.
*/
template<class Vector>
int Biarc<Vector>::operator!=(const Biarc &b) const {
  return !(*this==b);
}

#define CLAMP_DELTA(v) (fabsf(v)<1e-12?0:v)
/*!
  Prints starting point, tangent, matching point and matching tangent
  onto the stream \a out. If the biarc is not valid, it only prints
  the point/tangent data of this biarc and if this biarc data point
  is the last of the curve, then the tag --NULL-- is printed at the
  end.
*/
template<class Vector>
void Biarc<Vector>::print(ostream &out) const {
  if (_BIARC_) {
    out << "q0 = "  << _Point  << " t0 = "
	<< _Tangent << " m = " << _MidPoint
	<< " m_t = " << _MidTangent;
  }
  else {
    out <<"q0 = ";
    out << (CLAMP_DELTA(_Point[0]))
        << " " << (CLAMP_DELTA(_Point[1]))
	<< " " << (CLAMP_DELTA(_Point[2]));
    out << " t0 = ";
    out << (CLAMP_DELTA(_Tangent[0]))
        << " " << (CLAMP_DELTA(_Tangent[1]))
	<< " " << (CLAMP_DELTA(_Tangent[2]));

  }
}

#endif
