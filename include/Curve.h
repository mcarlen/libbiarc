#ifndef __CURVE_H__
#define __CURVE_H__

#include "Biarc.h"
#include "PKFmanip.h"
// #include "algo_helpers.h"
#include <fstream>
#include <vector>

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
  int _Closed;

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

  // FIXME : use const and & !!
 
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

  biarc_it begin() {
    return _Biarcs.begin();
  }

  biarc_it end() {
    return _Biarcs.end();
  }

  int isClosed() const ;
  void link() ;
  void unlink() ;

  void make(float f);
  void makeMidpointRule();
  void make(int from_N, int to_N, float f);
  void make(Biarc<Vector>* from, Biarc<Vector>* to, float f);
  void makeMidpointRule(int from_N, int to_N);
  void makeMidpointRule(Biarc<Vector>* from, Biarc<Vector>* to);

  void resample(int NewNoNodes);
  void refine(int from_N, int to_N, int NewNoNodes);
  void refine(biarc_it from, biarc_it to, int NewNoNodes);

  // caution it's actually radius_tp !!!
  float radius_pt(int from, int to);
  float radius_pt(biarc_it from, biarc_it to);
  float radius_pt(const Biarc<Vector> &from, const Biarc<Vector> &to);
  float radius_pt(const Vector &p0, const Vector &t0, const Vector &p1) const;
  float radius_pt(const float s, const float t) const;
 
  float pp(int from, int to) const;
  float pp(float s, float t) const;
  
  float radius_global(Biarc<Vector>& at);

  float thickness_fast();
  float thickness(Vector *from = NULL, Vector *to = NULL);

  void get_hint(int *i, int *j) const;
  void set_hint(const int i, const int j);

  float minSegDistance();
  float maxSegDistance();
  float distEnergy();

  float curvature(int n);
  float curvature(biarc_it b);
  Vector normalVector(int n);
  Vector normalVector(biarc_it b);
  float torsion(int n);
  float torsion2(int n);

  // Return Frenet-Frame
  // void frenet(Vector3& t, Vector3& n, Vector3& b);

  void computeTangents();
  void polygonalToArcs();
  void arcsToPolygonal();

  int nodes() const;
  float length() const;
  Vector pointAt(float s) const;
  Vector tangentAt(float s) const;
  biarc_it biarcAt(float s);
  int biarcPos(float s);

  Curve& rotAroundAxis(float angle,Vector axis);
  Curve& operator+=(const Vector &v);
  Curve& operator-=(const Vector &v);

  Curve operator+(const Curve &c) const;
  Curve operator-(const Curve &c) const;
  Curve operator*(const float s) const;

  // FIXME : Make this for MatrixN
  Curve& apply(Matrix3 &m);
  Vector getCenter() ;
  void center();
  void normalize();
  void scale(float s);

  void check_tangents();

  int readPKF(const char* filename);
  int readPKF(istream &in);
  int writePKF(const char* filename, int Header = 1);
  int writePKF(ostream &out, int Header = 1);
  int readXYZ(const char* filename);
  int readXYZ(istream &in);
  int readData(const char* filename, const char* delimiter);
  int readData(istream &in, const char* delimiter);
  int writeData(const char* filename, const char* delimiter,
		int tangetns_flag);
  int writeData(ostream &out, const char* delimiter,
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
#include "../lib/Curve.cpp"

#endif // __CURVE_H__
