#ifndef __CURVE_H__
#define __CURVE_H__

#include "Biarc.h"
#include "PKFmanip.h"
// #include "algo_helpers.h"
#include <fstream>
#include <vector>
// For changeDirection reverse
#include <algorithm>

#define StraightSegTol 1e-7

/*
 *  Ideas :
 *
 *  Make this for more than a single component.
 *  Make the pkf for other curves, not only BIARC_KNOT
 *
 *  -- more for fun : add a operator+ function that
 *  can add a whole curve to another (check if there
 *  are the same number of nodes in each curve, if not
 *  ask user to resapample or so. Than try to make
 *  a video that morphs from one knot to another by
 *  using some time step and the operator+ with lin
 *  interpolated moves or so.
 *
 */

#define make_default() makeMidpointRule()
// #define make_default() make(.5)

#define biarc_it  typename vector< Biarc<Vector> >::iterator
#define biarc_constit  typename vector< Biarc<Vector> >::const_iterator
#define biarc_ref typename vector< Biarc<Vector> >::reference
#define biarc_constref typename vector< Biarc<Vector> >::const_reference

template<class Vector>
class Curve : public PKFmanip {

  void init();
  int _hint_i, _hint_j;

 protected:

  vector<Biarc<Vector> > _Biarcs;
  biarc_constit accessBiarc(int i) const;
  biarc_it accessBiarc(int i);

//  int _NoNodes;
  bool _Closed;

  int readSinglePKF(istream &in);
  ostream& writeSinglePKF(ostream &out);
  int readSingleData(istream &in, const char* delimiter);
  ostream& writeSingleData(ostream &out, const char* delimiter,
		      int tangents_flag);
  int readSingleXYZ(istream &in);

 public:

  Curve();
  Curve(const char* filename);
  Curve(istream &in);
  Curve(const Curve<Vector> &curve);
  Curve &operator= (const Curve<Vector> &c);
  ~Curve();

  biarc_ref operator[](int c);
  biarc_constref operator[](int c) const;

  void push(const Biarc<Vector> &b);
  void push(const Vector &p, const Vector &t);
  void append(const Biarc<Vector> &b);
  void append(const Vector &p,const Vector &t);
  void insert(int loc, const Biarc<Vector> &b);
  void insert(int loc, const Vector &p, const Vector &t);
  void remove(int loc);
  void remove(biarc_it b);
  void flush_all();

  biarc_constref getNext(int i) const;
  biarc_constref getPrevious(int i);
  void setNext(int i,const Biarc<Vector>& b);
  void setPrevious(int i,const Biarc<Vector>& b);

  biarc_it begin();
  biarc_it end();

  bool isClosed() const;
  void link() ;
  void unlink() ;

  void changeDirection() ;

  void make(FLOAT_TYPE f);
  void makeMidpointRule();
  void make(int from_N, int to_N, FLOAT_TYPE f);
  void make(Biarc<Vector>* from, Biarc<Vector>* to, FLOAT_TYPE f);
  void makeMidpointRule(int from_N, int to_N);
  void makeMidpointRule(Biarc<Vector>* from, Biarc<Vector>* to);

  void resample(int NewNoNodes);
  void refine(int from_N, int to_N, int NewNoNodes);
  void refine(biarc_it from, biarc_it to, int NewNoNodes);

  // caution it's actually radius_tp !!!
  FLOAT_TYPE radius_pt(int from, int to);
  FLOAT_TYPE radius_pt(biarc_it from, biarc_it to);
  FLOAT_TYPE radius_pt(const Biarc<Vector> &from, const Biarc<Vector> &to);
  FLOAT_TYPE radius_pt(const Vector &p0, const Vector &t0, const Vector &p1) const;
  FLOAT_TYPE radius_pt(const FLOAT_TYPE s, const FLOAT_TYPE t) const;

  FLOAT_TYPE pp(int from, int to) const;
  FLOAT_TYPE pp(FLOAT_TYPE s, FLOAT_TYPE t) const;

  FLOAT_TYPE radius_global(Biarc<Vector>& at);

  FLOAT_TYPE thickness_fast();
  FLOAT_TYPE thickness(Vector *from = NULL, Vector *to = NULL);

  void get_hint(int *i, int *j) const;
  void set_hint(const int i, const int j);

  FLOAT_TYPE minSegDistance();
  FLOAT_TYPE maxSegDistance();
  FLOAT_TYPE span() const;
  FLOAT_TYPE distEnergy();

  FLOAT_TYPE curvature(int n);
  FLOAT_TYPE curvature(biarc_it b);
  Vector normalVector(int n);
  Vector normalVector(biarc_it b);
  FLOAT_TYPE torsion(int n, int a);
  FLOAT_TYPE torsion2(int n);
  FLOAT_TYPE signed_torsion(int n, int a);

  // Return Frenet-Frame
  // void frenet(Vector3& t, Vector3& n, Vector3& b);

  void inertiaTensor(Matrix3& mat);
  void principalAxis(Matrix3& mat);

  void computeTangents();
  void polygonalToArcs();
  void arcsToPolygonal();

  int nodes() const;
  FLOAT_TYPE length() const;
  Vector pointAt(FLOAT_TYPE s) const;
  Vector tangentAt(FLOAT_TYPE s) const;
  biarc_it biarcAt(FLOAT_TYPE s);
  int biarcPos(FLOAT_TYPE s);

  Curve& rotAroundAxis(FLOAT_TYPE angle,Vector axis);
  Curve& operator+=(const Vector &v);
  Curve& operator-=(const Vector &v);

  Curve operator+(const Curve &c) const;
  Curve operator-(const Curve &c) const;
  Curve operator*(const FLOAT_TYPE s) const;

  Curve& apply(Matrix3 &m);
  Vector getCenter() ;
  void center();
  void normalize();
  void scale(FLOAT_TYPE s);

  void check_tangents();

  bool readPKF(const char* filename);
  bool readPKF(istream &in);
  bool writePKF(const char* filename, int Header = 1);
  bool writePKF(ostream &out, int Header = 1);
  bool readXYZ(const char* filename);
  bool readXYZ(istream &in);
  bool readData(const char* filename, const char* delimiter);
  bool readData(istream &in, const char* delimiter);
  bool writeData(const char* filename, const char* delimiter,
		 int tangents_flag);
  bool writeData(ostream &out, const char* delimiter,
		 int tangents_flag);

  //  friend ostream & operator<<(ostream &out, const Curve<Vector> &c);
};

template<class Vector>
inline ostream &operator<<(ostream &out, Curve<Vector> &c) {

  if (c.nodes()==0) {
    out << "-- Curve : No points. --";
    return out;
  }

  for (biarc_it it=c.begin();it!=c.end();it++) {
    out << "Biarc "<< it->id() <<" : " << (*it);
    if (it!=(c.end()-1)) out << '\n';
  }
  return out;
}

// Templates need all code in the same file
// that's why we include here the source file!!!
#ifndef DOXYGEN_SHOULD_SKIP_THIS
#include "../lib/Curve.cpp"
#endif

#endif // __CURVE_H__
