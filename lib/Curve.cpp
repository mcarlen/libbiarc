/*!
  \class Curve Curve.h include/Curve.h
  \ingroup BiarcLibGroup
  \brief The Curve class for storing and manipulating a single biarc curve
  in \f$\mathcal{R}^3\f$.

  This class is used to store and manipulate a set of
  point/tangent data. This data can be interpolated to
  a biarc curve. The class is for open and closed curves, but
  this must be specified (how to do that is explained
  later in this text). 

  The space-curve is a linked-list of Biarc elements. Every
  biarc element in the "curve list" has two neighbours. If
  the point is the start or endpoint of an open curve, then
  it will only have a single neighbour.

  In order to change data in a curve, the functions push(),
  pop(), append(), insert() or remove() can be used.

  Here comes an example that reads in a PKF file describing a closed
  curve, interpolates the curve and resamples it :

  \code
  #include "../include/Curve.h"
  
  int main(int argc,char** argv) {

    // Read the data
    Curve c("data.pkf");

    // Close the curve
    c.link();

    // Interpolate the curve by biarcs
    c.make_default();

    // Resample the curve with 100 points.
    c.resample(100);

    // Write new curve to a file
    c.writePKF("resampled.pkf");

    return 0;
  }
  
  \endcode

  The next example illustrates how to build a circle with radius
  one and 10 data points :

  \code

  #include "../include/Curve.h"

  int main(int argc,char** argv) {

    // Init empty curve
    Curve circle;

    // Point and Tangent
    Vector3 p,t;

    // Construct PKF header
    circle.header("Circle","Author","","");

    // Build the circle with 10 nodes
    for (float i=0;i<10;i+=1.0) {

      // Compute point and tangent of the circle
      p = Vector3(sin(i*M_PI/5.0), cos(i*M_PI/5.0),0);
      t = Vector3(cos(i*M_PI/5.0),-sin(i*M_PI/5.0),0);

      // Normalize the tangent
      t.normalize();
      
      // Add point to the curve
      circle.append(p,t);
    }

    ...

    return 0;
  
  }

  \endcode
  
  \sa Biarc
  
*/
 
//
// documentation of inlined methods
//
 
/*!
  \fn ostream & Curve::operator<<(ostream &out, const Curve &c)

  Overloaded left shift operator. Writes the current Curve object \a c
  to the ostream object \a out. If there is an interpolated curve,
  this function prints point/tangent,matching point/tangent of all
  the biarcs of the curve. For non valid biarcs only the point/tangent
  data is written to the stream.
*/

/*!
  \var int Curve::_Closed

  Flag is set to 1 if the curve is closed, 0 otherwise.

  \sa isClosed(),link(),unlink().
*/

#include "../include/Curve.h"

// Templates need to have all in same file!!!
#ifndef __CURVE_SRC__
#define __CURVE_SRC__

/*!
  Returns biarc number \a c of the curve. Index starts
  off with zero (C style).
*/
template<class Vector>
inline biarc_constref Curve<Vector>::operator[](int c) const {
  return _Biarcs[c];
}

/*!
  Returns biarc number \a c of the curve. Index starts
  off with zero (C style).
*/
template<class Vector>
inline biarc_ref Curve<Vector>::operator[](int c) {
  return _Biarcs[c];
}


/*!
  Internal function to initialize a Curve object. Sets the list
  to NULL, the number of nodes and the open/closed flag to zero.
*/
// init to Null
template<class Vector>
void Curve<Vector>::init() {
  _Closed = 0;
}

/*!
  Private access function to the biarcs in the curve.
  Returns an STL iterator to a biarc
*/
template<class Vector>
biarc_it Curve<Vector>::accessBiarc(int i) {
  return (_Biarcs.begin()+i);
}

/*!
  Constructs an empty curve and sets the header to
  "No name","","",""
*/
template<class Vector>
Curve<Vector>::Curve() { init(); }

/*!
  Copy constructor.

  \sa operator=
*/
template<class Vector>
Curve<Vector>::Curve(const Curve<Vector> &curve) {
  init();
  *this = curve;
}

/*!
  Constructs a curve by reading it from the istream \a in.
*/
template<class Vector>
Curve<Vector>::Curve(const char* filename) {
  init();
  if (!readPKF(filename)) {
    cerr << "Curve(istream) : Could not read file "
	 << filename << endl;
    exit(2);
  }
}

/*!
  Read the curve data from a stream \a in. Does
  not read a PKF header. Call the readHeader() method
  for that purpose.

  \sa PKFmanip
*/
template<class Vector>
Curve<Vector>::Curve(istream &in) {
  init();
  if (!readSinglePKF(in))
    cerr << "Curve(istream) : Could not read from stream!\n";
}

/*!
  Assign operator. Copies all the point/tangent data.
  Closes the curve if \a c is closed.
*/
template<class Vector>
Curve<Vector>& Curve<Vector>::operator= (const Curve<Vector> &c) {
  if (this->nodes()!=0) this->flush_all();
  _Biarcs.insert(_Biarcs.begin(),c._Biarcs.begin(),c._Biarcs.end());
  if (c.isClosed())
    link();

  // Fix the biarcs' curve pointers!!
  for (biarc_it it=begin();it!=end();it++)
    it->setCurve(this);
  return *this;
}

/*!
  Destructor. Flushes all the elements of the curve object.
*/
template<class Vector>
Curve<Vector>::~Curve() {
  this->flush_all();
}

/*!
  Returns the number of nodes on the curve. A node
  is a point/tangent data.

  \sa Tube::radius(),Tube::segments()
*/
template<class Vector>
inline int Curve<Vector>::nodes() const {return _Biarcs.size();}

/*!
  Returns the arc-length of the whole curve, making the correct
  difference between an open or a closed curve.
*/
template<class Vector>
inline float Curve<Vector>::length() {
  assert(_Biarcs[0].isBiarc());

  float L = 0.0;

  int N = this->nodes();
  if (!_Closed) N--;

  for (biarc_it it=_Biarcs.begin();it!=_Biarcs.end();it++)
    L += (it->biarclength());

  return L;
}

/*!
  Return Point at Curve position Curve(s), where
  s is in [0,curvelength]
*/
// FIXME : STL adapt
template<class Vector>
Vector Curve<Vector>::pointAt(float s) {
  biarc_it current;
  Vector p;
  if (s==0.0) {
    return _Biarcs.front().getPoint();
  }
  else if (s==this->length()) {
    if (this->isClosed()) current = _Biarcs.begin();
    else current = (_Biarcs.end()-1);
    return current->getPoint();
  }
  else {
    current = _Biarcs.begin();

    // check if we have valid biarcs!
    assert(current->isBiarc());

    while (s > current->biarclength()) {
      s -= current->biarclength();
      current++;
    }
    return current->pointOnBiarc(s);
  }
}

/*!
  Return Tangent at Curve position Curve(s), where
  s is in [0,curvelength]
*/
// FIXME :: STL add
template<class Vector>
Vector Curve<Vector>::tangentAt(float s) {
  biarc_it current;
  Vector t;
  if (s==0.0) {
    return _Biarcs[0].getTangent();
  }
  else if (s==this->length()) {
    if (this->isClosed()) current = _Biarcs.begin();
    else current = (_Biarcs.end()-1);
    return current->getTangent();
  }
  else {
    current = _Biarcs.begin();

    // check if we have valid biarcs!
    assert(current->isBiarc());

    while (s > current->biarclength()) {
      s -= current->biarclength();
      current++;
    }
    return current->tangentOnBiarc(s);
  }
}

/*!
  Returns an STL iterator to the biarc on which the
  point Curve(s) is. s is in [0,curvelength]
*/
// FIXME:STL adapt
template<class Vector>
inline biarc_it Curve<Vector>::biarcAt(float s) {
  biarc_it current;
  Vector p;
  if (s==0.0) {
    return _Biarcs.begin();
  }
  else if (s==this->length()) {
    if (this->isClosed()) current = _Biarcs.begin();
    else current = (_Biarcs.end()-1);
    return current;
  }
  else {
    current = _Biarcs.begin();

    // check if we have valid biarcs!
    assert(current->isBiarc());

    while (s > current->biarclength()) {
      s -= current->biarclength();
      ++current;
    }
    return current;
  }
}

/*!
  Returns the position number of the biarc at
  Curve(s). s is in [0,curvelength]
*/
template<class Vector>
inline int Curve<Vector>::biarcPos(float s) {
  biarc_it it = biarcAt(s);
  return it->id();
}


/*!
  Adds a point/tangent data stored in biarc \a b at the
  front of the curve.
  
  \sa append(),insert(),pop(),remove()
*/
template<class Vector>
inline void Curve<Vector>::push(const Biarc<Vector> &b) {
  _Biarcs.insert(_Biarcs.begin(),b);
}

/*!
  Adds a point \a p, tangent \a t data at the
  front of the curve.
  
  \sa append(),insert(),pop(),remove()
*/
template<class Vector>
inline void Curve<Vector>::push(const Vector &p, const Vector &t) {
  push(Biarc<Vector>(p,t));
}

/*!
  Adds a point/tangent data stored in biarc \a b at the
  end of the curve.
  
  \sa push(),insert(),pop(),remove()
*/
template<class Vector>
inline void Curve<Vector>::append(const Biarc<Vector> &b) {
  _Biarcs.push_back(b);
  _Biarcs.back().setIdAndCurve(_Biarcs.size()-1,this);
}

/*!
  Adds a point \a p, tangent \a t data at the
  end of the curve.
  
  \sa push(),insert(),pop(),remove()
*/
template<class Vector>
inline void Curve<Vector>::append(const Vector &p,const Vector &t) {
  append(Biarc<Vector>(p,t));
}

/*!
  Insert a biarc \a b at position \a loc on the curve
  and push all the elements after \a loc one position
  to the right.
  The index starts with zero. If \a loc = nodes on the current
  curve, then the new element is added at the end of the curve.

  \sa push(),append(),pop(),remove()
*/
template<class Vector>
inline void Curve<Vector>::insert(int loc, const Biarc<Vector> &b) {
  _Biarcs.insert(_Biarcs.begin()+loc,b);
  (_Biarcs.begin()+loc)->setIdAndCurve(_Biarcs.size()-1,this);
}

/*!
  Insert a point \a p, tangent \a t data at position \a loc on the curve
  and push all the elements after \a loc one position
  to the right.
  The index starts with zero. If \a loc = nodes on the current
  curve, then the new element is added at the end of the curve.

  \sa push(),append(),pop(),remove()
*/
template<class Vector>
inline void Curve<Vector>::insert(int loc, const Vector &p, const Vector &t) {
  insert(loc,Biarc<Vector>(p,t));
}


/*!
  Remove biarc at position \a loc
*/
template<class Vector>
inline void Curve<Vector>::remove(biarc_it b) {
  _Biarcs.erase(b);
}

/*!
  Remove biarc at position \a loc
*/
template<class Vector>
inline void Curve<Vector>::remove(int loc) {
  remove(_Biarcs.begin()+loc);
}

/*!
  Remove all the nodes on the curve.
*/
template<class Vector>
inline void Curve<Vector>::flush_all() {
  _Biarcs.clear();
}

/*!
  Get a reference to the Biarc at position i+1
  No check is made wheter i+1 exists!
  However it wraps around for closed curves!
*/
template<class Vector>
biarc_constref Curve<Vector>::getNext(int i) const {
  if (_Closed)
    return _Biarcs[(i+1)%nodes()];
  else
    return _Biarcs[i+1];
}

/*!
  Get a reference to the Biarc at position i-1
  No check is made wheter i-1 exists for open curves!
  However it wraps around for closed curves!
*/
template<class Vector>
biarc_constref Curve<Vector>::getPrevious(int i) {
  if (_Closed && i==0)
    return _Biarcs.back();
  return _Biarcs[i-1];
}

/*!
  Set the Biarc at position i+1 to be Biarc b
*/
template<class Vector>
void Curve<Vector>::setNext(int i, const Biarc<Vector>& b) {
  if (_Closed)
    _Biarcs[(i+1)%nodes()] = b;
  else
    _Biarcs[i+1] = b;
}

/*!
  Set the Biarc at position i-1 to be Biarc b
*/
template<class Vector>
void Curve<Vector>::setPrevious(int i, const Biarc<Vector>& b) {
  if (_Closed && i==0)
    _Biarcs[nodes()-1] = b;
  else
    _Biarcs[i-1] = b;
}

/*!
  This is the way to close a curve. It links the
  last element in the list to the first.
  If the curve is already linked, nothing happens.
*/
template<class Vector>
void Curve<Vector>::link() {
  if (!_Closed)
    _Closed=1;
}

/*!
  Opens a closed curve. This cancels the link between the
  last and the first element in the curve.
  If the curve is already open, nothing happens.
*/
template<class Vector>
void Curve<Vector>::unlink() {
  if (_Closed)
    _Closed=0;
}

/*!
  Returns 1 if the curve is closed (i.e. the link() function
  had been used), and 0 otherwise.
*/
template<class Vector>
int Curve<Vector>::isClosed() const {
  return _Closed;
}

/*!
  Interpolate the point/tangent data actually stored in this
  Curve object. This computes all the matching points along
  the curve for a given value of \a f in [0,1].

  A "good" matching rule is still a unknown, usually f=0.5
  seems not to be a good guess. Some people use the matching
  point that is equidistant between the two data points to
  be interpolated.

  \sa Biarc::make(),CurveBundle::make()
*/
template<class Vector>
void Curve<Vector>::make(float f) {
  biarc_it it = _Biarcs.begin();
  biarc_it stop = _Closed?_Biarcs.end():(_Biarcs.end()-1);
  for (it=_Biarcs.begin();it!=stop;it++)
    it->make(f);
}

/*!
  Interpolate the point/tangent data actually stored in this
  Curve object. The computed matching points will be equidistant
  from their neighboring data points. The correct formulas are
  kindly provided by Antonio Trovato.

  \sa make(), CurveBundle::makeMidpointRule()
*/
// #define TROVATO
template<class Vector>
void Curve<Vector>::makeMidpointRule() {
#ifdef TROVATO
  biarc_it current = _Biarcs.begin();
  // FIXME: here we assume that _Biarcs has at least 2 data points!
  biarc_it next    = current+1;
  Vector d, t0, t1;
  float Gamma, d2, t0d, t1d, tt, tts, val_tmp;
  for (;next!=_Biarcs.end();next++,current++) {
    d = next->getPoint() - current->getPoint();
    d2 = d.norm2();
    t0 = current->getTangent();
    t1 = next->getTangent();
    t0d = d.dot(t0);
    t1d = d.dot(t1);
    tt = t0.dot(t1);
    tts = d2*(1.0-tt)+2.0*t1d*t0d;
    val_tmp = 2.0*fabsf(t1d)*(sqrt(0.5*tts)-t1d)/(tts-2.0*t1d*t1d);

    Gamma = val_tmp*t0d/(t1d+val_tmp*(t0d-t1d)) ;
    current->make(Gamma);
  }
  if (_Closed) {
    current = _Biarcs.end()-1;
    next    = _Biarcs.begin();

    d = next->getPoint() - current->getPoint();
    d2 = d.norm2();
    t0 = current->getTangent();
    t1 = next->getTangent();
    t0d = d.dot(t0);
    t1d = d.dot(t1);
    tt = t0.dot(t1);
    tts = d2*(1.0-tt)+2.0*t1d*t0d;
    val_tmp = 2.0*fabsf(t1d)*(sqrt(0.5*tts)-t1d)/(tts-2.0*t1d*t1d);

    Gamma = val_tmp*t0d/(t1d+val_tmp*(t0d-t1d)) ;
    current->make(Gamma);

  }
#else
/* Ben/Jana Style */
  biarc_it p0 = _Biarcs.begin(),p1 = _Biarcs.begin()+1;
  Vector q0,q1,t0,t1,q1m0,qhalf;
  float l,y,d,b,c;
  int N = nodes() - (_Closed?0:1);
  for (int i=0;i<N;i++) {
    if (p0==_Biarcs.end()) p0 = _Biarcs.begin();
    if (p1==_Biarcs.end()) p1 = _Biarcs.begin();
    q0 = p0->getPoint();
    q1 = p1->getPoint();
    t0 = p0->getTangent();
    t1 = p1->getTangent();
    q1m0=q1-q0;
    l=q1m0.norm();
    q1m0/=l;
    y=sqrt(2.-2.*t0.dot(t1)+4.*q1m0.dot(t0)*q1m0.dot(t1));
    d=y+q1m0.dot(t0+t1);
    b=(q1m0.dot(t1)+y/2.)/d;
    c=l/d/2.;
    qhalf=(1.-b)*q0+b*q1+c*(t0-t1);
    p0->setMidPoint(qhalf);

    // XXX : fix this, computing the tangent this way is unstable.
    //       c.f. pointOnBiarc
    Vector mmp0=qhalf-q0;
    p0->setMidTangent(2.*mmp0.dot(t0)/mmp0.norm2()*mmp0-t0);

    p0++;p1++;
  }
#endif
}

/*! TODO DOC */
template<class Vector>
void Curve<Vector>::make(int from_N, int to_N, float f) {
// #error "make not finished"
/*
  Biarc<Vector>* from = accessBiarc(from_N);
  Biarc<Vector>* to   = accessBiarc(to_N);

  this->make(from,to,f);
*/
};

/*! TODO DOC */
template<class Vector>
void Curve<Vector>::make(Biarc<Vector>* from, Biarc<Vector>* to, float f) {
// #error "make not finished"
/*
  if (!this->isBiarc()) {
    cerr << "Warning:make(Biarc*,Biarc*,float):You try to interpolate a part of a"
	 << " non interpolated curve!\n";
  }
    
  Biarc<Vector>* current = from;

  while (current!=to) {
    if (current==NULL || current->getNext()==NULL) {
      cerr << "Error:make(Biarc*,Biarc*,float):current or next biarc NULL!"
	   << "Forgot to close the curve?\n";
      return;
    }
    current->make(f);
    current = current->getNext();
  }
*/
}

/*! TODO DOC */
template<class Vector>
void Curve<Vector>::makeMidpointRule(int from_N, int to_N) {
// #error "make not finished"
/*
  Biarc<Vector>* from = accessBiarc(from_N);
  Biarc<Vector>* to   = accessBiarc(to_N);

  this->makeMidpointRule(from,to);
*/
}

/*! TODO DOC */
template<class Vector>
void Curve<Vector>::makeMidpointRule(Biarc<Vector>* from, Biarc<Vector>* to) {
// #error "makeMidpointRule not finished"
/*
  Vector d, t0, t1;
  float Gamma, d2, t0d, t1d, tt, tts, val_tmp;

  if (!this->isBiarc()) {
    cerr << "Warning:make(Biarc*,Biarc*,float):You try to interpolate a part of a"
	 << " non interpolated curve!\n";
  }
    
  Biarc<Vector>* current = from;

  while (current!=to) {
    if (current==NULL || current->getNext()==NULL) {
      cerr << "Error:make(Biarc*,Biarc*,float):current or next biarc NULL!"
	   << "Forgot to close the curve?\n";
      return;
    }

    d = current->getNext()->getPoint() - current->getPoint();
    d2 = d.norm2();
    t0 = current->getTangent();
    t1 = current->getNext()->getTangent();
    t0d = d.dot(t0);
    t1d = d.dot(t1);
    tt = t0.dot(t1);
    tts = d2*(1.0-tt)+2.0*t1d*t0d;
    val_tmp = 2.0*fabsf(t1d)*(sqrt(0.5*tts)-t1d)/(tts-2.0*t1d*t1d);

    Gamma = val_tmp*t0d/(t1d+val_tmp*(t0d-t1d)) ;
    current->make(Gamma);
    current = current->getNext();
  }
*/
}

/*!
  This function returns the radius of a circle that
  goes through point \a from and is tangent to the
  curve at point \a to. This function is commonly
  called \f$\rho_{pt}(s,t)\f$. The arguments are the
  positions of the datapoints.

  \sa thickness(), thickness_fast()
*/
template<class Vector>
float Curve<Vector>::radius_pt(int from, int to) {

  biarc_it A = _Biarcs.begin()+from;
  biarc_it B = _Biarcs.begin()+to;

  return radius_pt(A,B);
}

/*!
  This function returns the radius of a circle that
  goes through point \a from and is tangent to the
  curve at point \a to. This function is commonly
  called \f$\rho_{pt}(s,t)\f$. The arguments are two
  pointers to biarcs.

  \sa thickness(), thickness_fast()
*/
template<class Vector>
float Curve<Vector>::radius_pt(biarc_it from, biarc_it to) {
  return radius_pt(from->getPoint(),from->getTangent(),to->getPoint());
}

/*!
  This function returns the radius of a circle that
  goes through point \a from and is tangent to the
  curve at point \a to. This function is commonly
  called \f$\rho_{pt}(s,t)\f$. The arguments are two
  pointers to biarcs.

  \sa thickness(), thickness_fast()
*/
template<class Vector>
float Curve<Vector>::radius_pt(const Biarc<Vector> &from, const Biarc<Vector> &to) {
  return radius_pt(from.getPoint(),from.getTangent(),to.getPoint());
}


/*!
  This function returns the radius of a circle that
  goes through point \a p0 and its tangent t0 to the
  curve at point \a p1. This function is commonly
  called \f$\rho_{pt}(s,t)\f$.
  The arguments are thre Vectors p0,t0,p1.

  \sa thickness(), thickness_fast()
*/
template<class Vector>
float Curve<Vector>::radius_pt(const Vector &p0, const Vector &t0,
                               const Vector &p1) {

  Vector D = p1 - p0;
  float Dlen = D.norm();

  float cosa = t0.dot(D)/Dlen;
  float sina = sqrt(1.0-cosa*cosa);

  if (sina==0.0)
    return -1.0;
  else
    return 0.5*Dlen/sina;
}

/*!
  This function returns the radius of a circle that
  goes through the 3 points x,y,z. This function is commonly
  called \f$\rho_{ppp}(s,t)\f$.

  \sa thickness(), thickness_fast()
*/
template<class Vector>
float Curve<Vector>::radius_global(Biarc<Vector> &at) {
  biarc_it current = this->begin();
  float global = 1e8, r;
  for (;current!=_Biarcs.end();current++) {
    if ((*current)!=at) {
      r = radius_pt(at,(*current));
      if (r<global) {
        global = r;
      }
    }
  }
  return global;
}

/*!
  Returns the diameter of the thickest possible tube
  around the biarc curve without self intersection or
  crossing. This function implements the subdivision
  scheme as proposed in the thesis of Jana Smutny.

  \sa radius_pt(), thickness_fast()
*/
template<class Vector>
float Curve<Vector>::thickness() {
  return compute_thickness(this);
}


/*!
  Returns the diameter of the fattest possible tube around
  the biarc curve. This is the inaccurate/fast version where
  we only look for the smallest local radius of curvature or
  the smallest rho_pt.

  \sa radius_pt()
*/
template<class Vector>
float Curve<Vector>::thickness_fast() {

  float minrad = 1e16, radpt;
  
//  Biarc<Vector> current, sweep;
  
  biarc_it current = this->begin(), it;
  for (;current!=this->end();current++) {
    if (current->radius0()<minrad && current->radius0()>0.0)
      minrad = current->radius0();
    if (current->radius1()<minrad && current->radius1()>0.0)
      minrad = current->radius1();
    if (current!=(this->end()-2)) {
      for (it=current+1;it!=this->end();it++) {
	radpt = radius_pt(*current,*it);
	if (radpt<minrad && radpt>0.0)
	  minrad = radpt;
      }
    }
  }
  return 2*minrad;
}  

/*!
  Returns the length of the longest biarc in the
  current curve.

  \sa minSegDistance()
*/
template<class Vector>
float Curve<Vector>::maxSegDistance() {
  assert(_Biarcs[0].isBiarc());

  float M = -1.0;

  biarc_it it, stop = (_Closed?end():end()-1);
  for (it=begin();it!=stop;it++)
    M= max(it->biarclength(),M);

  return M;
}

/*!
  Returns the length of the smallest biarc in the
  current curve.

  \sa maxSegDistance()
*/
template<class Vector>
float Curve<Vector>::minSegDistance() {
  assert(_Biarcs[0].isBiarc());

  float M = 1e12;

  biarc_it it, stop = (_Closed?end():end()-1);
  for (it=begin();it!=stop;it++)
    M= min(it->biarclength(),M);

  return M;
}

/*!
  This function resamples the current Curve object with
  \a NewNoNodes nodes on the curve. The whole curve
  is given as a sequence of biarcs and resampling by biarcs
  is therefore straightforward.

  The resampling can only be done if the curve is biarc
  interpolated (meaning a call to one of the make() methods).

  \sa refine(),make(),CurveBundle::resample()
*/
// XXX : resampling doesn't work. problem with closed curves!
//       i.e. we get one point to much
template<class Vector>
void Curve<Vector>::resample(int NewNoNodes) {

  Curve<Vector>* c = new Curve<Vector>;
  biarc_it current = _Biarcs.begin();
  biarc_it stop_biarc = _Biarcs.end();
  Vector p, t;

  // check if we have valid biarcs!
  assert(current->isBiarc());

  float SegLen;
  
  // We want exactly \NewNoNodes points on curve, therefore
  if (_Closed)
    SegLen = length()/(float)NewNoNodes;
  else
    SegLen = length()/((float)(NewNoNodes-1));

  float SegDummy = SegLen;
  float Total = 0.0;

  // Set first point
  c->append(*current);

  int STOP = 0;
  while (!STOP) { 

    // is the next sample point still on the current biarc or on the next
    if (SegDummy < current->biarclength()) {
      // FIXME:straight segment check : can i do that better?!!!
      p = current->pointOnBiarc(SegDummy);
      t = current->tangentOnBiarc(SegDummy);

      t.normalize();
      c->append(p,t);

      SegDummy += SegLen;
      Total += SegLen;
    }
    else {

      // substract the length of the current biarc
      SegDummy -= current->biarclength();
      current++;
      
      if (_Closed) {
        if (current==stop_biarc) {
          current = _Biarcs.begin();
          if (NewNoNodes!=c->nodes()) {
  	    float d = ((*c)[0].getPoint()-((*c).end()-1)->getPoint()).norm();
  	    if (d < SegLen/100.0)
	      c->remove(c->nodes()-1);
  	  }
	  STOP = 1;
        }
      }
      else {
	if ((current+1)==stop_biarc) {
	  // Do we need to add last point?
	  if (NewNoNodes!=c->nodes())
	    c->append(_Biarcs.back());
	  
	  STOP = 1;
	}
      }
    }
  }

  if (this->isClosed()) c->link();
  *this = *c;
}

/*!
  This function puts \a NewNoNodes nodes between curve
  node \a from_N and node \a to_N. The count of the number
  of new nodes includes start and endpoint of that part
  of the curve.

  The resampling can only be done if the curve is biarc
  interpolated (meaning a call to one of the make() methods).

  So Far, periodic refinement is not yet implemented. I.e.
  it is not possible to refine the curve between node 'nodes-5' and
  node '10' !!!!

  \sa resample(),make().
*/
// Refine a part of a curve by N points
template<class Vector>
void Curve<Vector>::refine(int from_N, int to_N, int NewNoNodes) {
  biarc_it from = accessBiarc(from_N);
  biarc_it to   = accessBiarc(to_N);

  refine(from,to,NewNoNodes);
}

/*!
  This function puts \a NewNoNodes nodes between pointer
  Biarc* from and Biarc* to. The count of the number
  of new nodes includes start and endpoint of that part
  of the curve.

  The resampling can only be done if the curve is biarc
  interpolated (meaning a call to one of the make() methods).

  So Far, periodic refinement is not yet implemented. I.e.
  it is not possible to refine the curve between node 'nodes-5' and
  node '10' !!!!

  \sa resample(),make().
*/
template<class Vector>
void Curve<Vector>::refine(biarc_it from, biarc_it to, int NewNoNodes) {

  // FIXME : interval check, non-closed, periodic refinement ...

  vector<Biarc<Vector> > c;

  biarc_it begin = from, end = to;
  biarc_it current = begin;
  // check if we have valid biarcs!
  assert(current->isBiarc());

  Vector p, t;

  // Compute arclength between biarc begin to end
  float PartLength = 0.0;

  while (current!=end) {
    PartLength += current->biarclength();
    if (_Closed&& current==(_Biarcs.end()-1))
      current = _Biarcs.begin();
    else
      current++;
  }

  float SegLen = PartLength/((float)NewNoNodes-1.0);

  float SegDummy = SegLen;

  // Init first part of curve till biarc begin
  // begin+1 since the stl range copy is excluding the last elem
  // pointed to by iterator
  c.insert(c.begin(),_Biarcs.begin(),begin+1);
  current = begin;

  while (current!=end) {

    if (SegDummy < current->biarclength()) {
      // FIXME:straight segment check : can i do that better?!!!
      if (fabsf(current->getTangent().dot(current->getNext().getTangent()))>(1.0-StraightSegTol) &&
          fabsf(current->getTangent().dot((current->getNext().getPoint()-current->getPoint()).normalize()))>(1.-StraightSegTol)) {
	p = current->getPoint() + current->getTangent()*SegDummy;
	t = current->getTangent();
      }
      else {
	p = current->pointOnBiarc(SegDummy);
      
	// compute corresponding tangent
	t = current->getTangent().reflect(p-current->getPoint());
      }

      t.normalize();
      
      c.push_back(Biarc<Vector>(p,t));
      
      SegDummy += SegLen;
    }
    else {
      // substract current biarc length
      SegDummy -= current->biarclength();
      
      // go on to the next biarc
      if (_Closed&& current==(_Biarcs.end()-1))
        current = _Biarcs.begin();
      else
        current++;

    }
  }

  /*
   * Append biarc end, if this point isn't already there
   * if the sum of the parts is > than PartLength, then
   * the last point will not be appended, therefore it is neccessary to
   * put it manually. this point is the "end" biarc given as argument
   * of this function
   */
  if ((c.back().getPoint()-current->getPoint()).norm()>(10e-3*SegLen))
    c.push_back(*current);
  current++;

  // append rest of the curve
  while (current!=_Biarcs.end()) {
    c.push_back(*current);
    current++;
  }
 
  // give the newly resampled curve back to the user
  _Biarcs = c;

}

/*
 * Curvature and Torsion methods
 *
 */

/*!
  Computes the curvature at biarc number \a n.

  This curvature function is independant of the interpolation
  it takes only data points/tangents and computes the curvature
  approximation as given in Smutny's Thesis pp.60

  Caution : No inflection points test, so far.

  \sa torsion(),torsion2()
*/
template<class Vector>
float Curve<Vector>::curvature(int n) {
  biarc_it b = accessBiarc(n);
  return curvature(b);
}

/*!
  Computes the curvature at biarc \a b.

  This curvature function is independant of the interpolation
  it takes only data points/tangents and computes the curvature
  approximation as given in Smutny's Thesis pp.60

  Caution : No inflection points test, so far.

  \sa torsion(),torsion2()
*/
template<class Vector>
float Curve<Vector>::curvature(biarc_it b) {

  // FIXME
  // so far no inflection point test!!

  biarc_it current   = b;
  biarc_it current_h = b+1;
  if (current_h == end())
    current_h = begin();

  if (_Closed) {
    current_h = current+1;
    if (current_h==_Biarcs.end())
      current_h = _Biarcs.begin();
  }
  else {
    if (b+1==end())
      current_h = b-1;
  }

  Vector d_h = current_h->getPoint() - current->getPoint();
  Vector t_0 = current->getTangent();
  Vector t_h = current_h->getTangent();

  float radius_0 = d_h.norm2()/2.0/(d_h.cross(t_0)).norm();
  float radius_1 = d_h.norm2()/2.0/(d_h.cross(t_h)).norm();

  // return radius of curvature which is 1/rho
  return 3.0/(2.0*radius_0 + radius_1);
}

/*!
  Returns the normal vector at biarc number \a n.

  XXX : inflection points!
*/
template<class Vector>
Vector Curve<Vector>::normalVector(int n) {
  biarc_it b = accessBiarc(n);
  return normalVector(b);
}

/*!
  Returns the normal vector at biarc \a b.

  XXX : - inflection points!
        - closed curve case!
*/
template<class Vector>
Vector Curve<Vector>::normalVector(biarc_it b) {
  biarc_it prev,next;
  if (b == begin())
    prev = end()-1;
  else
    prev = b-1;
  if (b+1 == end())
    next = begin();
  else
    next = b+1;
  Vector v = (b->getMidPoint()-b->getPoint())-
             (b->getPoint()-prev->getMidPoint());
// XXX not normalize, this way we indirectly recovert the
//     local curvature
//  v.normalize();
  return v;
}

/*!
  Returns the torsion at biarc number \a n.

  The torsion computation as given in Smutny's Thesis p.64
  it is also only based on point/tangent data.

  Caution : No inflection points test, so far.

  \sa torsion2(),curvature()
*/
// FIXME : STL adapt!!
template<class Vector>
float Curve<Vector>::torsion(int n) {

  // doesn't seem to be right
  // not too sure if this is accurate
  // since the h is the arclength between biarc n and biarc n+1

  Biarc<Vector> *current = accessBiarc(n), *current_h;
  float h, sin_phi;
  Vector d_0,t_0,d_h,t_h;

  /*
   * First treat the closed curve case
   */
  if (_Closed) {
    current_h = current->getNext();
    h = current->biarclength();

  }
  else {
    if (n>=(nodes()-2)) {
      // FIXME : end of non-closed curve problem
      return 0.0;
      current = accessBiarc(n-2);
      current_h = accessBiarc(n-1);
      h = current->biarclength();
    }
    else {
      current_h = current->getNext();
      h = current->biarclength();
    }
  }

  d_0 = current_h->getPoint() - current->getPoint();
  t_0 = current->getTangent();
  d_h = current_h->getNext()->getPoint() - current_h->getPoint();
  t_h = current_h->getTangent();

  Vector v_0 = t_0.cross(d_0);
  Vector v_1 = t_h.cross(d_h);

  v_0.normalize(), v_1.normalize();
  sin_phi = fabsf((v_0.cross(v_1)).norm()); 
  return 3.0*sin_phi/h;

}


/*!
  Returns the torsion at biarc number \a n.

  Slightly modified version of the torsion() computation.
  This time we consider a biarc's previous and next
  midpoint to compute the torsion at the current biarc.
  
  Interpolated biarcs are necessary!

  Caution : No inflection points test, so far.
  The torsion at the beginning and the end of an open curve
  is clamped to zero.

  \sa torsion(),curvature()
*/
// FIXME : STL adapt
template<class Vector>
float Curve<Vector>::torsion2(int n) {

  assert(_Biarcs[0].isBiarc());

  if ((n==0||n==(nodes()-1))&&!_Closed)
    return 0.0;

  Biarc<Vector> *current = accessBiarc(n);

  float h = current->getPrevious()->arclength1() + current->arclength0();

  Vector d_0,t_0,d_h,t_h;

  d_0 = current->getPoint() - current->getPrevious()->getMidPoint();
  t_0 = current->getPrevious()->getMidTangent();
  d_h = current->getMidPoint() - current->getPoint();
  t_h = current->getMidTangent();

  Vector v_0 = t_0.cross(d_0);
  Vector v_1 = t_h.cross(d_h);

  v_0.normalize(), v_1.normalize();
  float sin_phi = fabsf((v_0.cross(v_1)).norm()); 
  return 3.0*sin_phi/h;

}


/*
// FIXME : maybe this could be usefull one day ...
void Curve::frenet(int n, Vector3& tangent,
		   Vector3& normal, Vector3& binormal) {
  Biarc<Vector>* b = accessBiarc(n);
  tangent = b->getTangent();

  // need a normal!!!

  binormal = tangent.cross(normal);

}
*/

/*!
  Normalize the length of the curve. The resulting curve will
  have arclenth one.

  An interpolated curve is necessary to compute the length of it.

  \sa scale(),CurveBundle::normalize()
*/
template<class Vector>
void Curve<Vector>::normalize() {
  assert (_Biarcs.begin()->isBiarc());
  float invL = 1.0/length();
  scale(invL);
}

/*!
  Scales the length of the curve by \a s.
  The curve needs not to be interpolated, since only the
  data points are changed.

  \sa normalize(),CurveBundle::scale()
*/
template<class Vector>
void Curve<Vector>::scale(float s) {
  for (biarc_it it=_Biarcs.begin();it!=_Biarcs.end();it++)
    (*it)*=s;
}

/*!
  Translates all the curve points by \a v.

  Redo the interpolation after this operation if
  the initial curve was biarc interpolated, since
  the matching points and bezier points are no longer
  correct.

  \sa center().
*/
template<class Vector>
Curve<Vector>& Curve<Vector>::operator+=(const Vector &v) {
  for (biarc_it it=_Biarcs.begin();it!=_Biarcs.end();it++)
    (*it)+=v;
  return *this;
}

/*!
  Translates all the curve points by \a -v.

  Redo the interpolation after this operation if
  the initial curve was biarc interpolated, since
  the matching points and bezier points are no longer
  correct.

  \sa center().
*/
template<class Vector>
Curve<Vector>& Curve<Vector>::operator-=(const Vector &v) {
  for (biarc_it it=_Biarcs.begin();it!=_Biarcs.end();it++)
    (*it)-=v;
  return *this;
}

/*!
  Rotate curve about some given axis. The rotation angle
  is given in radians!!!
*/
template<class Vector>
Curve<Vector>& Curve<Vector>::rotAroundAxis(float angle,Vector axis) {
  float c = cos(angle), s = sin(angle);
  float t = 1. - c;
  float X=axis[0],Y=axis[1],Z=axis[2];
  Matrix3 m(Vector3(t*X*X+c,t*X*Y+s*Z,t*X*Z-s*Y),
            Vector3(t*X*Y-s*Z,t*Y*Y+c,t*Y*Z+s*X),
            Vector3(t*X*Z+s*Y,t*Y*Z-s*X,t*Z*Z+c));

  return this->apply(m);
}

/*!
  Add a curve to another. Checks wheter the two curves have
  the same number of nodes.
*/
template<class Vector>
Curve<Vector> Curve<Vector>::operator+(const Curve<Vector> &c) const {
  assert(this->nodes()==c.nodes());

  biarc_constit c1 = _Biarcs.begin();
  biarc_constit c2 = c._Biarcs.begin();
  Curve<Vector> c_new;
  c_new.header(getName(),getEtic(),getCite(),getHistory());

  for (;c1!=_Biarcs.end();c1++,c2++)
     c_new.append(c1->getPoint()+c2->getPoint(),
                  c1->getTangent()+c2->getTangent()); 

  return c_new;
}

/*!
  Remove a curve from another. Checks wheter the two curves have
  the same number of nodes.
*/
template<class Vector>
Curve<Vector> Curve<Vector>::operator-(const Curve<Vector> &c) const {
  assert(this->nodes()==c.nodes());

  biarc_constit c1 = _Biarcs.begin();
  biarc_constit c2 = c._Biarcs.begin();
  Curve<Vector> c_new;
  c_new.header(getName(),getEtic(),getCite(),getHistory());

  for (;c1!=_Biarcs.end();c1++,c2++)
     c_new.append(c1->getPoint()+c2->getPoint(),
                  c1->getTangent()+c2->getTangent()); 

  return c_new;
}

/*!
  Returns a copy of the curve scaled by a factor of s.
*/
template<class Vector>
Curve<Vector> Curve<Vector>::operator*(const float s) const {

  Curve<Vector> c_new(*this);
  c_new.scale(s);

  return c_new;
}

/*!
  Applies the rotation specified by a rotation matrix \a m
  to the curve. No check is done for \a m, the user must
  know what matrix he wants to apply.

  This is not the standart 4x4 transformation matrix approach
  known from homogeneous coordinates stuff. 
*/
template<class Vector>
Curve<Vector>& Curve<Vector>::apply(Matrix3 &m) {
  Vector p,t,tmp;

  // Rotate each point and tangent by applying Matrix3 m
  for (biarc_it it=_Biarcs.begin();it!=_Biarcs.end();it++) {
    p = it->getPoint();
    t = it->getTangent();

    tmp = m*p;
    it->setPoint(tmp);
    tmp = m*t;
    it->setTangent(tmp);
  }

  return *this;
}

/*!
  Centers the curve's mass center to <0,0,0>.

  \sa getCenter()
*/
template<class Vector>
void Curve<Vector>::center() {

  Vector delta_center = getCenter();

  (*this) -= delta_center;
}

/*!
  Returns the curve's center of mass.

  \sa center()
*/
template<class Vector>
Vector Curve<Vector>::getCenter() {
  Vector sum; sum.zero();
  biarc_it current = _Biarcs.begin();

  for (;current!=_Biarcs.end();current++)
    sum += current->getPoint();
  sum /= ((float)nodes());
  return sum;
}

/*!
  Prints Biarc and Tangent norm, if abs(norm-1)>1e-4
*/
template<class Vector>
void Curve<Vector>::check_tangents() {
  int count = 0;
  for (biarc_it it = begin(); it!= end(); it++) {
    count++;
    if (fabsf(it->getTangent().norm()-1.0)>1e-4)
      cout << "Tan norm of biarc " << count << " : " << (*it) << " = " << it->getTangent().norm()<<endl;
    if (it->getTangent().dot(it->getNext().getTangent())<1-0.1 && it!=end()-1)
      cout << "Tan too far apart of biarc " << count << " : " << (*it) << " = " << it->getTangent().norm()<<endl;
  }
}

/*!
  This function reads the curve data from a file \a filename, known
  as a portable knot format file (PKF). The PKF comes from the
  links-knots library written by Ben Laurie and Myk Soar.

  An initial header gives details about the knot or link, such
  as the number of components (curves), the name of the current
  knot and more. The tags ETIC,CITE and HIST store a string for
  copyright purpose. The same tags ended with *L give the length of
  the string. The header manipulation is done in the PKFmanip class.

  Each curve centerline is given as a list of point/tangent
  data.

  The Curve class can only handle a single curve. If the number
  of components in the file is larger than 1, a warning message
  is posted to the output, saying that a CurveBundle object
  is necessary to process all the curves in the file.

  It follows an example of a PKF file :

  \verbatim
  PKF 0.2
  BIARC_KNOT "Knot Name"
  ETICL 14
  ETIC "Please cite me"
  END
  CITEL 14
  CITE "I have to cite"
  END
  HISTL 7
  HIST "Remarks"
  END 
  NCMP <Number of components> 
  COMP <Number of nodes for component 1> 
  NODE px py pz tx ty tz
  NODE 1.1363 0.2903 0.1548 0.2936 1.2251 0.2837
  NODE 0.7187 0.8510 0.0271 -0.6460 0.7450 0.1103
  ...
  END
  COMP <Number of nodes for component 2>
  ...
  END
  \endverbatim

  Returns 1 if all went well, zero otherwise.

  \sa writePKF(),CurveBundle,PKFmanip
*/
template<class Vector>
int Curve<Vector>::readPKF(istream &in) {

  readHeader(in);

  char tmp[1024];
  // Read NoComp
  in.getline(tmp,sizeof tmp);
  if(strncmp(tmp,"NCMP ",5)) {
    cerr << "Expected NCMP: " << tmp << '\n';
    return 0;
  }
  int NoCurves = atoi(tmp+5);
  if (NoCurves>1) {
    cerr << "Number of curves = " << NoCurves << endl
	 << "The class Curve can not handle more than\n"
	 << "one component, use a CurveBundle object!";
    return 0;
  }

  if (!readSinglePKF(in)) {
    cerr << "Curve::readPKF() : Could not read component!\n";
    return 0;
  }

  return 1;
}

/*!
  Read a PKF curve from the file \a filename.

  \sa writePKF()
*/
template<class Vector>
int Curve<Vector>::readPKF(const char* filename) {

  ifstream ifs(filename,ios::in);
  if (!ifs.good()) {
    cerr << "Curve::readPKF() : Problem reading file " << filename << endl;
    return 0;
  }
  if (!readPKF(ifs)) {
    cerr << "Curve::readPKF() : could not read curve.\n";
    return 0;
  }

  return 1;
}

/*!
  Read in only the curve data, this is from the COMP
  tag to the curve's END tag!

  \sa readPKF()
*/
template<class Vector>
int Curve<Vector>::readSinglePKF(istream &in) {

  char tmp[1024];
  in.getline(tmp,sizeof tmp);
  if(strncmp(tmp,"COMP ",5)) {
    cerr << "Expected COMP: " << tmp << '\n';
    return 0;
  }

  // Read in Nodes
  int NoNodesDummy  = atoi(tmp+5);
  for(int n=0;n<NoNodesDummy;n++) {

    in.getline(tmp,sizeof tmp);

    if(strncmp(tmp,"NODE ",5)) {
      cerr << "Expected NODE: " << tmp << " "<<n << '\n';
      return 0;
    }

    char *val=strtok(tmp+5," ");

    Vector p,t;
    
    // FIXME : THIS IS NOT RIGHT!!!!! for 4 components
    // Read point

    p[0] = atof(val);
    for (unsigned int i=1;i<p.type;i++) {
      val  = strtok(NULL," ");
      p[i] = atof(val);
    }

    // Read tangent
    for (unsigned int i=0;i<t.type;i++) {
      val  = strtok(NULL," ");
      t[i] = atof(val);
    }

    append(p,t);

  }

  in.getline(tmp,sizeof tmp);
  if(strncmp(tmp,"END",3)) {
    cerr << "Expected END: " << tmp << '\n';
    return 0;
  }

  return 1;

}

/*!
  Writes the current curve object to a stream
  \a out. For more than 1 component use the CurveBundle class.

  Returns 1 if all went well, zero otherwise.

  \sa readPKF(),CurveBundle
*/
template<class Vector>
int Curve<Vector>::writePKF(ostream &out, int Header) {
  if (Header) {
    writeHeader(out);
    out << "NCMP 1"<<endl;
  }
  writeSinglePKF(out);
  return 1;
}

/*!
  Writes the current curve object to a portable PKF file
  \a filename. For more than 1 component use the CurveBundle class.

  Returns 1 if all went well, zero otherwise.

  \sa readPKF(),CurveBundle
*/
template<class Vector>
int Curve<Vector>::writePKF(const char* filename, int Header) {
  
  ofstream ofs(filename,ios::out);
  if (!ofs.good()) {
    cerr << "Curve::writePKF() : write to file " << filename << "problem.\n";
    return 0;
  }
  if (!writePKF(ofs, Header)) {
    cerr << "Curve::writePKF() : could not write curve.\n";
    return 0;
  }
  return 1;
}

/*!
  Write the current curve object to a stream \a out.
  Start with sending "COMP #C" to the stream, where
  #C are the number of nodes currently stored in the
  Curve object. Then follow the NODE tags with the
  data point coordinates.
  
  \sa writePKF()
*/
template<class Vector>
ostream& Curve<Vector>::writeSinglePKF(ostream &out) {
  out << "COMP " << nodes() << '\n';
  int OldOsf = out.precision(20);
  for (biarc_it it=_Biarcs.begin();it!=_Biarcs.end();it++) {
    out << "NODE " << it->getPoint()
	<< ' '     << it->getTangent() << endl;
  }
  
  out << "END\n";
  out.precision(OldOsf);

  return out;
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
*/
template<class Vector>
int Curve<Vector>::readData(istream &in, const char* delimiter) {
  readSingleData(in,delimiter);
  return 1;
}

/*!
  Read the curve data (only x,y,z coordinates) from a file \a
  filename, where the delimiter can be any char*. The default
  delimiter is a space.
*/
template<class Vector>
int Curve<Vector>::readData(const char* filename, const char* delimiter) {
  ifstream ifs(filename,ios::in);
  if (!ifs.good()) {
    cerr << "Curve::readData() : Could not read from " << filename << ".\n";
    return 0;
  }
  if (!readData(ifs, delimiter)) {
    cerr << "Curve::readData() : Could not read data from "
	 <<filename<<".\n";
    return 0;
  }
  return 1;
}

/*!
  Read in the curve data for a single curve given
  in VECT format! Only one component is supported. 
  Skips the first two lines. Reads the number of
  vertices from the 2nd column of line three. Skips
  3 more lines and reads in the coordinates.

  \sa readData()
*/
template<class Vector>
int Curve<Vector>::readSingleData(istream &in, const char* delimiter) {
  char tmp[1024];
  
  in.getline(tmp,sizeof tmp);
  in.getline(tmp,sizeof tmp);

  // Read in number of Nodes
  int NoNodesDummy  = atoi(&tmp[1]);

  in.getline(tmp,sizeof tmp);
  in.getline(tmp,sizeof tmp);
  in.getline(tmp,sizeof tmp);

  Vector p;
  char* val;
  for(int n=0;n<NoNodesDummy;n++) {

    in.getline(tmp,sizeof tmp);

    // Read point
    // skip "atom"
    val = strtok(tmp,delimiter);
    // val  = strtok(NULL,delimiter);
    p[0] = atof(val);
    for (unsigned int i=1;i<p.type;i++) {
      val  = strtok(NULL,delimiter);
      p[i] = atof(val);
    }
    append(p,Vector());
  }
  cout << "Warning: readSingleData :"
       << "The tangents are currently all (0,1,0) vectors\n";
  return 1;
}

/*!
  Write the curve to a file. The format is : the number
  of curves at the first line, then the number of data points
  (the format is {#N}) and then the list of x,y,z coordinates
  for each data point.

  \sa writeSingleData(),readData()
*/
template<class Vector>
int Curve<Vector>::writeData(const char* filename, const char* delimiter,
			  int tangents_flag) {
  ofstream out(filename,ios::out);
  if (!out.good()) {
    cerr << "Can not write to " << filename << endl;
    return 0;
  }
  out << 1 << endl;
  writeSingleData(out,delimiter,tangents_flag);
  return 1;
}

/*!
  Writes the curve to a data file. First line is the number
  of points and then follows a list of x,y,z coordinates.
  If the \a tangents_flag is set to 1, the tangents of the
  points are also dropped (Default is 0).

  In fact we call only the readSingleData() function.

  \sa writeSingleData(),readData()
*/
template<class Vector>
int Curve<Vector>::writeData(ostream &out, const char* delimiter,
			  int tangents_flag) {
  out << 1 << endl;
  writeSingleData(out,delimiter,tangents_flag);
  return 1;
}

/*!
  Writes the current curve to a a stream. A custom delimiter
  may be specified and the tangents are also written or not
  depending on the value of the \a tangents_flag. 1 = please
  write the tangents to the file, 0 = please don't.

  \sa writeData()
*/
template<class Vector>
ostream& Curve<Vector>::writeSingleData(ostream &out, const char* delimiter,
				int tangents_flag) {
    // Number of points on curve
  out << '{' << nodes() << '}' << endl;

  int OldOsf = out.precision(40);

  // Drop points / [tangents]
  for (biarc_it it=_Biarcs.begin();it!=_Biarcs.end();it++) {
    // use the requested delimiter to format the ouptut
    out << it->getPoint()[0] << delimiter
	<< it->getPoint()[1] << delimiter
	<< it->getPoint()[2] << endl;
    if (tangents_flag) {
      out << it->getTangent()[0] << delimiter
	  << it->getTangent()[1] << delimiter
	  << it->getTangent()[2] << endl;
    }
  }
  out.precision(OldOsf);

  return out;
}

/*!
  Computes the tangents for a set of points only. The tangent
  \f$t_i\f$ at the point \f$p_i\f$ is set to

  \f$t_i=\frac{p_{i+1}-p_{i-1}}{|p_{i+1}-p_{i-1}|}\f$ 

  If the curve is open the first and the last point get the
  tangent

  \f$t_0=\frac{p_1-p_0}{|p_1-p_0|}\f$ 
  \f$t_{N-1}=\frac{p_{N-1}-p_{N-2}}{|p_{N-1}-p_{N-2}|}\f$ 
  
*/
template<class Vector>
void Curve<Vector>::computeTangents() {
  biarc_it head = _Biarcs.begin(), tail = _Biarcs.end();
  biarc_it current = head+1;
  Vector t;

  // compute tangents except for first, last point
  for (;current!=tail-1;current++) {
    t = (current+1)->getPoint() - (current-1)->getPoint() ;
    current->setTangent(t);
  }

  // open/closed curve case
  if (_Closed) {

    // First curve point
    t = (head+1)->getPoint() - (tail-1)->getPoint() ;
    head->setTangent(t);

    // Last curve point
    t = head->getPoint() - (tail-2)->getPoint() ;
    (tail-1)->setTangent(t);
  }
  else {

    // First curve point
    t =(head+1)->getPoint() - head->getPoint() ;
    head->setTangent(t);

    // Last curve point
    t = tail->getPoint() - (tail-1)->getPoint() ;
    (tail-1)->setTangent(t);
  }
    
}

/*!
  This function converts a polygonal curve into a
  curve made of arcs of circles.

  The current data points (supposed to describe a polygonal
  curve) stored in Curve \a c are transformed in the following
  way

  \f$p_i^{new}=\frac{p_i^{old}+ p_{i+1}^{old}}{2}\f$ 

  the corresponding tangents at each of these points \f$p_i\f$
  if given by

  \f$t_i= \frac{p_i^{old} + p_{i+1}^{old}}{2\,|p_i^{old}-p_{i+1}^{old}| }\f$ 

  \sa arcsTolPolygonal()
*/
// FIXME : works only for closed
template<class Vector>
void Curve<Vector>::polygonalToArcs() {

  Curve<Vector> tmp;

  biarc_it current = _Biarcs.begin();
  Vector p, t;

  for (;current!=(_Biarcs.end()-1);current++) {

    p = ((current+1)->getPoint() + current->getPoint())/2.0;
    t = (current+1)->getPoint() - current->getPoint();

    tmp.append(p,t);
  }

  (*this) = tmp;
}

/*!
  This function converts a biarc curve into a
  polygonal curve.

  The vertices for the polygonal curve are the
  Bezier control points of the biarc interpolated
  curve, by taking only the control point at the
  tip of the triangle.

  The tangent data for the polygonal knot are set
  to <0,1,0>.

  Caution : a curve with N biarcs yields a polygonal
  curve with 2N vertices. This is true for an open
  and for a closed curve, since for an open curve
  the endpoints of the original biarc curve
  are also included.

  \sa arcsToPolygonal()
*/
template<class Vector>
void Curve<Vector>::arcsToPolygonal() {

  // We don't need all the Bezier control points
  Vector b0,b1,b2,b3;

  // we need to interpolate the thing
  if (!_Biarcs[0].isBiarc())
    make_default();

  // Now the bezier control points give the
  // polygonal representation of the curve
  Curve<Vector> tmp;

  biarc_it current = _Biarcs.begin();
  Vector p0,p1;

  int N = _Closed?nodes():nodes()-1;

  // if not closed the first vertex is the first pt
  // in the biarc curve
  if (!_Closed) tmp.append(current->getPoint(),Vector());

  for(int i=0;i<N;i++) {

    current->getBezier(b0,p0,b1,b2,p1,b3);

    tmp.append(p0,Vector(0,1,0));
    tmp.append(p1,Vector(0,1,0));
    
    current++;
  }

  if (!_Closed) tmp.append(current->getNext().getPoint(),Vector());

  *this = tmp;

}

#endif // __CURVE_SRC__
