#include "fourier_4_1.h"

// Observation (for k4.1)
//
// cosx cosy cosz sinx siny sinz
//  A 0 B -B 0  A
//  0 C 0  0 D  0    // maybe C = 2*D
//  E 0 F  F 0 -E
//  0 G 0  0 H  0    // G and H very small

// private class to convert normal coeff files to 4_1 format
static void convert_coeffs(const char* file, K41FourierKnot *fk);

K41FourierKnot::K41FourierKnot() {}

K41FourierKnot::K41FourierKnot(const char* file, int Normal) {
  if (Normal) convert_coeffs(file,this);
  else {
    ifstream in(file);
    if (in.good()) in >> *this;
    else {
      cerr << "K41FourierKnot : Could not read " << file << endl;
      exit(2);
    }
  }
}

K41FourierKnot::K41FourierKnot(const K41FourierKnot &fk) {
  clear();
  csin = fk.csin;
  ccos = fk.ccos;
}

K41FourierKnot& K41FourierKnot::operator=(const K41FourierKnot &fk) {
  clear();
  csin = fk.csin;
  ccos = fk.ccos;
  return *this;
}

K41FourierKnot K41FourierKnot::operator+(const K41FourierKnot &fk) {
  K41FourierKnot sum;

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

K41FourierKnot& K41FourierKnot::operator*=(const float d) {
  for (uint i=0;i<csin.size();++i) {
    csin[i]*=d; ccos[i]*=d;
  }
  return *this;
}

K41FourierKnot& K41FourierKnot::operator/=(const float d) {
  return ((*this)*=(1./d));
}

FourierKnot K41FourierKnot::toFourierKnot() {
//  A 0 B -B 0  A
//  0 C 0  0 D  0   
//  E 0 F  F 0 -E
//  0 0 0  0 0  0    
// Let's try : A B C D E F  as coeff file

  Coeff c;
  Coeffs lsin, lcos;
  for (uint i=0;i<csin.size();++i) {
    c[0]= ccos[i][0]; c[1]=0;           c[2]=ccos[i][1];
    lcos.push_back(c);
    c[0]= -ccos[i][1]; c[1]=0 ; c[2]=ccos[i][0];
    lsin.push_back(c);
    c[0]= 0; c[1]=ccos[i][2];           c[2]=0;
    lcos.push_back(c);
    c[0]=  0;         c[1]= csin[i][0]; c[2]=0;
    lsin.push_back(c);
    c[0]= csin[i][1];          c[1]=0;           c[2]=csin[i][2];
    lcos.push_back(c);
    c[0]= csin[i][2];          c[1]=0;           c[2]=-csin[i][1];
    lsin.push_back(c);
    c[0]= 0;          c[1]=0;           c[2]=0;
    lcos.push_back(c);
    lsin.push_back(c);
  }
  c[0]= 0;          c[1]=0;           c[2]=0;
  return FourierKnot(c,lsin,lcos);
}


// point at curve(s), s in (0,1)
Vector3 K41FourierKnot::operator()(float t) {
  float f1,f2,f3;
  Vector3 r(0,0,0);
  // XXX optimise/cache this (precompute cos(f1*t) ... and swap values 1<-2<-3, precomp 3 iterate
  for (uint i=0;i<csin.size();++i) {
    f1 = (float)(4*i+1)*(2.*M_PI);
    f2 = (float)(4*i+2)*(2.*M_PI);
    f3 = (float)(4*i+3)*(2.*M_PI);

    r += Vector3( ccos[i][0]*cos(f1*t) + csin[i][1]*cos(f3*t)
                 -ccos[i][1]*sin(f1*t) + csin[i][2]*sin(f3*t),
                  ccos[i][2]*cos(f2*t) + csin[i][0]*sin(f2*t),
                  ccos[i][1]*cos(f1*t) + csin[i][2]*cos(f3*t)
                 +ccos[i][0]*sin(f1*t) - csin[i][1]*sin(f3*t)); 
  }
  return r;
}

// tangent at curve(t)
Vector3 K41FourierKnot::prime(float t) {
  Vector3 r; float f1,f2,f3;
  // XXX optimise/cache this
  for (uint i=0;i<csin.size();++i) {
    f1 = (float)(4*i+1)*(2.*M_PI);
    f2 = (float)(4*i+2)*(2.*M_PI);
    f3 = (float)(4*i+3)*(2.*M_PI);

    r += Vector3(-f1*ccos[i][0]*sin(f1*t) - f3*csin[i][1]*sin(f3*t)
                 -f1*ccos[i][1]*cos(f1*t) + f3*csin[i][2]*cos(f3*t),
                 -f2*ccos[i][2]*sin(f2*t) + f2*csin[i][0]*cos(f2*t),
                 -f1*ccos[i][1]*sin(f1*t) - f3*csin[i][2]*sin(f3*t)
                 +f1*ccos[i][0]*cos(f1*t) - f3*csin[i][1]*cos(f3*t)); 
  }
  return r;
}
 
// cosx cosy cosz sinx siny sinz
//  A 0 B -B 0  A
//  0 C 0  0 D  0    // maybe C = 2*D
//  E 0 F  F 0 -E
//  0 G 0  0 H  0    // G and H very small

// Translate a normal coeff file to k4.1 sparse coeff file
static void convert_coeffs(const char* file, K41FourierKnot *fk) {
  FourierKnot tmp(file); Vector3 vcos, vsin;
  fk->clear();
  for (uint i=0;i<tmp.csin.size();i+=4) {
    vcos.setValues(tmp.ccos[i+0][0],tmp.ccos[i+0][2],tmp.ccos[i+1][1]);
    vsin.setValues(tmp.csin[i+1][1],tmp.ccos[i+2][0],tmp.ccos[i+2][2]);
    // cout << v << endl;
    fk->csin.push_back(vsin);
    fk->ccos.push_back(vcos);
  }
}

// #define TEST
#ifdef TEST
int main(int argc, char** argv) {
  
  // K41 class (convert a normal coeff file to 4.1 format)
  K41FourierKnot fk(argv[1],1);

  // Print coeffs
  cout << fk << endl;

  return 0;
}
#endif
