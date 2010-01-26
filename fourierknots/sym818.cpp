#include "fourier_syn.h"
#include "CurveBundle.h"

#define OFFSET .3

void g1(FourierKnot *fk) {
  fk->mirror(Vector3(0,0,1));
  fk->rotate(Vector3(0,0,1),M_PI/4.); 
  fk->shift(5./8.);
}

void g2(FourierKnot *fk) {
  fk->rotate(Vector3(0,0,1),M_PI/2.);
  fk->shift(.25);
}

void mysym(int J, int I, FourierKnot *fk) {
  for (int j=0;j<J;++j)
    g1(fk);
  for (int i=0;i<I;++i)
    g2(fk);
}

const int SymN = 32;
FourierKnot syms[SymN];

int main(int argc, char ** argv) {

  if (argc!=3+9) {
    cout << "Usage : " << argv[0] << " coeffs out.pkf matrix3x3" << endl;
    exit(0);
  }

  FourierKnot fk(argv[1]), final;

  CurveBundle<Vector3> cb;
  Curve<Vector3> c;

  fk.c0.zero();
  fk.toCurve(100,&c);
  cb.newCurve(c);

  // Write aligned fourier knot
  cout << setprecision(16) << fk << endl;

  // Put the symmetries here :
  //  FourierKnot sym1(fk);

  for (int i=0;i<SymN;++i) {
    syms[i] = fk;
  }

  // final = fk;
  int symid = 0;
  for (int i=0;i<4;++i) {
    for (int j=0;j<8;++j) {
      mysym(j,i,&syms[symid]);

      syms[symid].c0[1] = (j+1)*OFFSET;
      syms[symid].c0[0] = (i+1)*OFFSET;

      c.flush_all();
      syms[symid].toCurve(100,&c);
      cb.newCurve(c);

      symid++;
    }
  }

  final = syms[0];
  for (int i=1;i<SymN;++i)
    final = final + syms[i];
  final = final/(float)SymN;

  c.flush_all();
  final.toCurve(100,&c);
  c.writePKF("final.pkf");

  cb.writePKF(argv[2]);

  cerr << setprecision(16) << final << endl;

  return 0;
  
}
