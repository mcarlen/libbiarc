#ifndef __CURVE_BUNDLE_H__
#define __CURVE_BUNDLE_H__

#include "PKFmanip.h"
#include "Curve.h"
#include <vector>

template<class Vector>
class CurveBundle : public PKFmanip {

  void init();
  vector< Curve<Vector> > bundle;

 public:

  int curves() const ;
  int nodes() const;
  void link();
  void unlink();

  void newCurve(Curve<Vector>& c);
  void newCurve(istream& in);
  void newCurve(Curve<Vector>* c);
  Curve<Vector>& operator[](int c);

  CurveBundle();
  CurveBundle(const char* filename);
  CurveBundle(const CurveBundle &cb);
  CurveBundle &operator= (const CurveBundle &cb);
  ~CurveBundle();

  void make(float f);
  void makeMidpointRule();
  void resample(int NewNoNodes);

  void normalize();
  float length();
  float thickness();
  float thickness_fast();
  CurveBundle& operator+=(const Vector &vec);
  CurveBundle& operator-=(const Vector &vec);
  void center();

  Vector getCenter() ;
  CurveBundle& scale(float s);
  CurveBundle& rotate(Matrix3 &m);
  
  int readPKF(const char* infile);
  int readPKF(istream& in);
  int writePKF(const char *outfile);
  int writePKF(ostream& out);
  int readXYZ(const char* infile);
  int readData(const char* infile, const char* delimiter = " ");
  int writeData(const char* outfile, const char* delimiter = " ",
		int tangents_flag = 0);
  int readVECT(const char* infile);
  int writeVECT(const char* outfile);

  void computeTangents();
  void polygonalToArcs();
  void arcsToPolygonal();

  // friend ostream & operator<<(ostream &out, CurveBundle<Vector> &cb);

};

template<class Vector>
inline ostream &operator<<(ostream &out, CurveBundle<Vector> &c) {

  if (c.curves()==0) {
    out << "No curves!\n";
    return out;
  }
  
  for (int i=0;i<c.curves();i++) {
    out << "Curve "<< i << endl;
    out << c.bundle[i] << endl;
  }
  return out;
}

// Template thing
#include "../lib/CurveBundle.cpp"

#endif // __CURVE_BUNDLE_H__
