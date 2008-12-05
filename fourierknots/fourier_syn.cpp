#include "fourier_syn.h"

void FourierKnot::clear() {
  csin.clear(); ccos.clear(); c0.setValues(0,0,0);
}

FourierKnot::FourierKnot() {}
FourierKnot::~FourierKnot() { clear(); }

void FourierKnot::scale(float s) {
  c0 *= s;
  for (uint i=0;i<csin.size();++i) {
    csin[i] *= s;
    ccos[i] *= s;
  }
}

// We expect 2*n + 1 coefficient!
FourierKnot::FourierKnot(const char* file) {
  ifstream in(file);
  if (in.good()) in >> *this;
  else {
    cerr << "FourierKnot : Could not read " << file << endl;
    exit(2);
  }
}

FourierKnot::FourierKnot(Coeffs lsin, Coeffs lcos) {
  assert(lsin.size()==lcos.size());
  csin = lsin; ccos = lcos;
}

FourierKnot::FourierKnot(Coeff constant, Coeffs lsin, Coeffs lcos) {
  FourierKnot(lsin,lcos);
  c0 = constant;
}

void FourierKnot::set(Coeff c, Coeffs lsin, Coeffs lcos) {
  c0 = c; csin = lsin; ccos = lcos;
}

void FourierKnot::setConst(Coeff constant) { c0 = constant; }
void FourierKnot::setSin(Coeffs lsin) { csin = lsin; }
void FourierKnot::setCos(Coeffs lcos) { ccos = lcos; }

// point at curve(s), s in (0,1)
Vector3 FourierKnot::operator()(float t) {
  float f;
  // Fourier constant term
  Vector3 r = this->c0/2.;
  // XXX optimise/cache this
  for (uint i=0;i<csin.size();++i) {
    f = (float)(i+1)*(2.*M_PI);
    r += cos(f*t)*ccos[i];
    r += sin(f*t)*csin[i];
  }
  return r;
}

// tangent at curve(t)
Vector3 FourierKnot::prime(float t) {
  Vector3 r; float f;
  // XXX optimise/cache this
  for (uint i=0;i<csin.size();++i) {
    f = (float)(i+1)*(2.*M_PI);
    r += (-f*sin(f*t))*ccos[i];
    r += ( f*cos(f*t))*csin[i];
  }
  return r;
}

// sample our fourier knot and put that in initialized
// pointer to Curve curve
void FourierKnot::toCurve(const int sampling, Curve<Vector3> *curve) {
  float isampling = 1./(float)sampling, s;
  for (int i=0;i<sampling;++i) {
    s = (float)i*isampling;
    // Tangent gets normalized in biarc constructor
    curve->append((*this)(s),this->prime(s));
  }
}

/*!
  Function pointer pt2func is used to change the speed of the
  parametrization along the curve. This means, sample the curve
  with sampling points using a speed function given by pt2func.
  Write the result in the already initialized and clean curve.
*/
void FourierKnot::toCurve(float(*pt2func)(float), const int sampling,
                          Curve<Vector3> *curve) {
  float isampling = 1./(float)sampling, s;
  for (int i=0;i<sampling;++i) {
    s = pt2func((float)i*isampling);
    // Tangent gets normalized in biarc constructor
    curve->append((*this)(s),this->prime(s));
  }
}

// Rotate coefficients around axis v by angle alpha.
// This is equal to rotating the object in Euclidean space
// XXX HG has this in the neg trigo direction!!!
void FourierKnot::rotate(Vector3 v,float alpha) {
  // This rotation matrix has a trigo pos orientation!
  Matrix3 D; D.rotAround(v,alpha);
  c0 = D*c0;
  for (uint i=0;i<csin.size();++i) {
    csin[i] = D*csin[i];
    ccos[i] = D*ccos[i];
  }
}

// Flips the paramtrization orientation of the curve
void FourierKnot::flip_dir(float sh) {
  if (sh!=0)
    this->shift(sh);
  for (Coeffs::iterator i=csin.begin();i!=csin.end();++i)
    *i = -(*i);
  if (sh!=0)
    this->shift(-sh);
}

// Shift parameter space of the coefficients
void FourierKnot::shift(float sh) {
  Coeffs tmpsin(csin), tmpcos(ccos);
  float f,vcos,vsin;
  for (uint i=0;i<csin.size();++i) {
    f = (float)(i+1)*(2.0*M_PI);
    vcos = cos(f*sh); vsin = sin(f*sh);
    csin[i] = vcos*tmpsin[i] + vsin*tmpcos[i];
    ccos[i] = vcos*tmpcos[i] - vsin*tmpsin[i];
  }
}

TrefoilFourierKnot::TrefoilFourierKnot() {}

TrefoilFourierKnot::TrefoilFourierKnot(const char* file) {
  ifstream in(file);
  if (in.good()) in >> *this;
  else {
    cerr << "TrefoilFourierKnot : Could not read " << file << endl;
    exit(2);
  }
  _shift = 0.0;
}

TrefoilFourierKnot::TrefoilFourierKnot(const TrefoilFourierKnot &tfk) {
  clear();
  _shift = tfk._shift;
  csin = tfk.csin;
}

TrefoilFourierKnot& TrefoilFourierKnot::operator=(const TrefoilFourierKnot &tfk) {
  clear();
  _shift = 0.0;
  csin = tfk.csin;
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
  t += _shift;
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

void TrefoilFourierKnot::shift(float sh) {
  _shift = sh;
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
  
  FourierKnot fk;
  Curve<Vector3> knot;

  fk.c0 = Vector3(0,1,0);
  fk.csin.push_back(Vector3(1,0,0));
  fk.ccos.push_back(Vector3(0,.5,0));

  fk.rotate(Vector3(0,0,-1),M_PI/4.);
  fk.shift(M_PI/2.);
  fk.flip_dir();

  fk.toCurve(40, &knot);

  knot.header("Fourier knot test","libbiarc","H. Gerlach","");

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
