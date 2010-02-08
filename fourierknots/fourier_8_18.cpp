#include "fourier_8_18.h"


// Structure (for k8.18)
//
// cosx cosy cosz sinx siny sinz
//  0 0 0  0  0  0
//  0 0 0  0  0  0
//  A B 0 -B  A  0
//  0 0 C  0  0  D
//  E F 0  F -E  0
//  0 0 0  0  0  0
//  0 0 0  0  0  0
//  0 0 0  0  0  0

// private class to convert normal coeff files to 8_18 format
static void convert_coeffs(const char* file, K818FourierKnot *fk);

K818FourierKnot::K818FourierKnot() {}

K818FourierKnot::K818FourierKnot(const char* file, int Normal) {
  if (Normal) convert_coeffs(file,this);
  else {
    ifstream in(file);
    if (in.good()) in >> *this;
    else {
      cerr << "K818FourierKnot : Could not read " << file << endl;
      exit(2);
    }
  }
}

K818FourierKnot::K818FourierKnot(const K818FourierKnot &fk) {
  clear();
  csin = fk.csin;
  ccos = fk.ccos;
}

K818FourierKnot& K818FourierKnot::operator=(const K818FourierKnot &fk) {
  clear();
  csin = fk.csin;
  ccos = fk.ccos;
  return *this;
}

K818FourierKnot K818FourierKnot::operator+(const K818FourierKnot &fk) {
  K818FourierKnot sum;

  int LocalSmaller = csin.size()<fk.csin.size();
  int N=min(csin.size(),fk.csin.size());
  int Nmax= max(fk.csin.size(),csin.size());

  for (int i=0;i<N;++i) {
    sum.csin.push_back(csin[i]+fk.csin[i]);
    sum.ccos.push_back(ccos[i]+fk.ccos[i]);
  }

  if (LocalSmaller)
    for (int i=N;i<Nmax;++i) {
      sum.csin.push_back(fk.csin[i]);
      sum.ccos.push_back(fk.ccos[i]);
    }
  else
    for (int i=N;i<Nmax;++i) {
      sum.csin.push_back(csin[i]);
      sum.ccos.push_back(ccos[i]);
    }

  sum.c0.zero();

  return sum;
}

K818FourierKnot& K818FourierKnot::operator*=(const float d) {
  for (uint i=0;i<csin.size();++i) {
    csin[i]*=d; ccos[i]*=d;
  }
  return *this;
}

K818FourierKnot& K818FourierKnot::operator/=(const float d) {
  return ((*this)*=(1./d));
}

FourierKnot K818FourierKnot::toFourierKnot() {
//  0 0 0  0  0  0
//  0 0 0  0  0  0
//  A B 0 -B  A  0
//  0 0 C  0  0  D
//  E F 0  F -E  0
//  0 0 0  0  0  0
//  0 0 0  0  0  0
//  0 0 0  0  0  0
// Let's try : A B C D E F  as coeff file

  Coeff c;
  Coeffs lsin, lcos;
  for (uint i=0;i<csin.size();++i) {
    c.zero();
    // Rows 1 + 2
    lcos.push_back(c); lsin.push_back(c);
    lcos.push_back(c); lsin.push_back(c);
    c[0]=ccos[i][0]; c[1]=ccos[i][1]; c[2]=0;
    lcos.push_back(c);
    c[0]=-ccos[i][1]; c[1]=ccos[i][0]; c[2]=0;
    lsin.push_back(c);
    c.zero(); c[2] = ccos[i][2];
    lcos.push_back(c);
    c[2]=csin[i][0];
    lsin.push_back(c);
    c[0] = csin[i][1]; c[1] = csin[i][2]; c[2] = 0;
    lcos.push_back(c);
    c[0] = csin[i][2]; c[1] = -csin[i][1]; c[2] = 0;
    lsin.push_back(c);
    c.zero();
    for (int zz=0;zz<3;++zz) {
      lcos.push_back(c);
      lsin.push_back(c);
    }
  }
  return FourierKnot(c,lsin,lcos);
}


// point at curve(s), s in (0,1)
Vector3 K818FourierKnot::operator()(float t) const {
  float f3,f4,f5;
  Vector3 r(0,0,0);
  // XXX optimise/cache this (precompute cos(f1*t) ... and swap values 1<-2<-3, precomp 3 iterate
  for (uint i=0;i<csin.size();++i) {
    f3 = (float)(8*i+3)*(2.*M_PI);
    f4 = (float)(8*i+4)*(2.*M_PI);
    f5 = (float)(8*i+5)*(2.*M_PI);

    r += Vector3( ccos[i][0]*cos(f3*t) + csin[i][1]*cos(f5*t)
                 -ccos[i][1]*sin(f3*t) + csin[i][2]*sin(f5*t),
                  ccos[i][1]*cos(f3*t) + csin[i][2]*cos(f5*t)
                 +ccos[i][0]*sin(f3*t) - csin[i][1]*sin(f5*t),
                  ccos[i][2]*cos(f4*t) + csin[i][0]*sin(f4*t));
  }
  return r;
}

// tangent at curve(t)
Vector3 K818FourierKnot::prime(float t) const {
  Vector3 r; float f3,f4,f5;
  // XXX optimise/cache this
  for (uint i=0;i<csin.size();++i) {
    f3 = (float)(8*i+3)*(2.*M_PI);
    f4 = (float)(8*i+4)*(2.*M_PI);
    f5 = (float)(8*i+5)*(2.*M_PI);

    r += Vector3(-f3*ccos[i][0]*sin(f3*t) - f5*csin[i][1]*sin(f5*t)
                 -f3*ccos[i][1]*cos(f3*t) + f5*csin[i][2]*cos(f5*t),
                 -f3*ccos[i][1]*sin(f3*t) - f5*csin[i][2]*sin(f5*t)
                 +f3*ccos[i][0]*cos(f3*t) - f5*csin[i][1]*cos(f5*t),
                 -f4*ccos[i][2]*sin(f4*t) + f4*csin[i][0]*cos(f4*t));

  }
  return r;
}
 
// Translate a normal coeff file to k8.18 sparse coeff file
static void convert_coeffs(const char* file, K818FourierKnot *fk) {
  FourierKnot tmp(file); Vector3 vcos, vsin;
  fk->clear();
  for (uint i=0;i<tmp.csin.size();i+=8) {
    vcos.setValues(tmp.ccos[i+2][0],tmp.ccos[i+2][1],tmp.ccos[i+3][2]);
    vsin.setValues(tmp.csin[i+3][2],tmp.ccos[i+4][0],tmp.ccos[i+4][1]);
    // cout << v << endl;
    fk->csin.push_back(vsin);
    fk->ccos.push_back(vcos);
  }
}

// #define TEST
#ifdef TEST
int main(int argc, char** argv) {
  
  // K818 class (convert a normal coeff file to 8_18 format)
  K818FourierKnot fk(argv[1],1);

  // Print coeffs
  cout << fk << endl;

  return 0;
}
#endif
