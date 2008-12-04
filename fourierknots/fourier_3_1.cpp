#include "fourier_3_1.h"

TrefoilFourierKnot::TrefoilFourierKnot() {}

TrefoilFourierKnot::TrefoilFourierKnot(const char* file) {
  ifstream in(file);
  if (in.good()) in >> *this;
  else {
    cerr << "TrefoilFourierKnot : Could not read " << file << endl;
    exit(2);
  }
}

TrefoilFourierKnot::TrefoilFourierKnot(const TrefoilFourierKnot &tfk) {
  clear();
  csin = tfk.csin;
}

TrefoilFourierKnot& TrefoilFourierKnot::operator=(const TrefoilFourierKnot &tfk) {
  clear();
  csin = tfk.csin;
  return *this;
}

// point at curve(s), s in (0,1)
Vector3 TrefoilFourierKnot::operator()(float t) {
  float f1,f2,f3;
  Vector3 r(0,0,0);
  // XXX optimise/cache this (precompute cos(f1*t) ... and swap values 1<-2<-3, precomp 3 iterate
  for (uint i=0;i<csin.size();++i) {
    f1 = (float)(3*i+1)*(2.*M_PI);
    f2 = (float)(3*i+2)*(2.*M_PI);
    f3 = (float)(3*i+3)*(2.*M_PI);
    // formula for cos(a)-sin(a)?
    r += Vector3(-csin[i][0]*cos(f1*t)+csin[i][1]*cos(f2*t),
                  csin[i][0]*sin(f1*t)+csin[i][1]*sin(f2*t),
                  csin[i][2]*(sin(f3*t)));
  }
  return r;
}

// tangent at curve(t)
Vector3 TrefoilFourierKnot::prime(float t) {
  Vector3 r; float f1,f2,f3;
  // XXX optimise/cache this
  for (uint i=0;i<csin.size();++i) {
    f1 = (float)(3*i+1)*(2.*M_PI);
    f2 = (float)(3*i+2)*(2.*M_PI);
    f3 = (float)(3*i+3)*(2.*M_PI);
    // formula for cos(a)-sin(a)?
    r += Vector3(f1*csin[i][0]*sin(f1*t)-f2*csin[i][1]*sin(f2*t),
                 f1*csin[i][0]*cos(f1*t)+f2*csin[i][1]*cos(f2*t),
                 f3*csin[i][2]*(cos(f3*t)));
  }
  r.normalize();
  return r;
}
 
void TrefoilFourierKnot::scale(float s) {
  c0 *= s;
  for (uint i=0;i<csin.size();++i)
    csin[i] *= s;
}


// Translate a normal coeff file to trefoil sparse coeff file
static void coeffs2fourier(const char* file, FourierKnot *fk) {
  FourierKnot tmp(file); Vector3 v;
  fk->clear();
  for (uint i=0;i<tmp.csin.size();i+=3) {
    v.setValues(tmp.csin[i][1],tmp.csin[i+1][1],tmp.csin[i+2][2]);
    // cout << v << endl;
    fk->csin.push_back(v);
  }
}

// #define TEST
#ifdef TEST
int main(int argc, char** argv) {
  
  TrefoilFourierKnot tfk;
  Curve<Vector3> knot;

  tfk.c0 = Vector3(0,1,0);
  tfk.csin.push_back(Vector3(1,0,0));

  tfk.toCurve(40, &knot);

  knot.header("Trefoil fourier knot test","libbiarc","H. Gerlach","");
  knot.writePKF("test.pkf");

  // Trefoil class
/*
  TrefoilFourierKnot fk;
  coeffs2fourier(argv[1],&fk);
  fk.scale(4);

  // Print the trefoil short coefficient format to stdout
  cout << fk << endl;
*/

//  cout << "D=" << knot.thickness() << endl;

  return 0;
}
#endif
