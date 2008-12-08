#include "fourier_3_1.h"

TrefoilFourierKnot::TrefoilFourierKnot() {}

TrefoilFourierKnot::TrefoilFourierKnot(const char* file) {
  ifstream in(file);
  if (in.good()) in >> *this;
  else {
    cerr << "TrefoilFourierKnot : Could not read " << file << endl;
    exit(2);
  }
  _shift = 0.; // XXX: save shift in file, too
}

TrefoilFourierKnot::TrefoilFourierKnot(const TrefoilFourierKnot &tfk) {
  clear();
  csin = tfk.csin;
  _shift = tfk._shift;
}

TrefoilFourierKnot& TrefoilFourierKnot::operator=(const TrefoilFourierKnot &tfk) {
  clear();
  csin = tfk.csin;
  _shift = tfk._shift;
  return *this;
}

// point at curve(s), s in (0,1)
Vector3 TrefoilFourierKnot::operator()(float t) {
  float f1,f2,f3;
  Vector3 r(0,0,0);
  t += _shift;
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
  t += _shift;
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

void TrefoilFourierKnot::shift(float sh) {
  _shift += sh;
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


float adjusthelper(float x, float a1, float h1, float a2, float h2) {
  const float C=1./6.;
  float r = C - a1 -a2;
  float hr = C - h1 - h2;
  if (x < r) {
    return x * hr/r;
  }
  if (x < r+a2) {
    return hr+(x-r)*h2/a2;
  }
  if (x < C + a1) {
    return hr+h2+(x-r-a2)*h1/a1;
  }
  if (x < C + a1 + a2) {
    return hr+h2+2*h1+(x-C-a1)*h2/a2;
  }
  return hr+2*h2+2*h1+(x-C-a1-a2)*hr/r;
}

float adjust3(float x) {
  float shift = 0.0;
  while (x<0.) {x+=1./3.; shift-=1./3.; }
  while (x>1./3.) {x-=1./3.; shift+=1./3.; }
  return shift + adjusthelper(x);
}

float adjust2(float x) {
  float s = 0.8;
  return x+s/(3.*2.*M_PI)*sin(3.*2.*M_PI*x) +
         s/(3.*2.*M_PI)*sin(3.*2.*M_PI*(x+s/(3.*2.*M_PI)*sin(3.*2.*M_PI*x)));
}

float adjust(float x) {
  return (x+0.95/(3.*2.*M_PI)*sin(3.*2.*M_PI*x));
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
