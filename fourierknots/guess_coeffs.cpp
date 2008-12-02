#include "fourier_syn.h"
#include "../include/Curve.h"

int main(int argc, char** argv) {

  if (argc!=5) {
    cout << "Usage : " << argv[0] << " Ncoeffs Nsamples in.pkf out.coeff\n";
    exit(0);
  }

  FourierKnot fk;
  Curve<Vector3> curve(argv[3]);
  int Samples=atoi(argv[2]);
  int N=atoi(argv[1]);
  Vector3 vsin, vcos;

  curve.link();
  curve.make_default();
  curve.resample(Samples);
  curve.normalize();
  curve.make_default();

  vector<Biarc<Vector3> >::iterator it;
  float dx = 2*M_PI/(float)Samples, x;
  for (int n=1;n<=N;++n) {
    vsin.zero(); vcos.zero(); x = 0.0;
    for (it=curve.begin();it!=curve.end();++it) {
      vsin += it->getPoint()*sin(n*x);
      vcos += it->getPoint()*cos(n*x);
      x += dx;
    }
    fk.csin.push_back(vsin);
    fk.ccos.push_back(vcos);
  }

  // Constant term
  for (it=curve.begin();it!=curve.end();++it) {
    fk.c0 += it->getPoint();
  }

  Curve<Vector3> curve2;
  fk.toCurve(200,&curve2);
  curve2.make_default();
  float iL = 1./curve2.length();
  for (unsigned int i=0;i<fk.csin.size();++i) {
    fk.csin[i] *= iL;
    fk.ccos[i] *= iL;
  }
  fk.c0 *= iL;

  ofstream of(argv[4]);
  of << fk;
  of.close();

  return 0;

}
