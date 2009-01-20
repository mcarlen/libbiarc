//FIXME: (HG) NOMAIN-construction sucks!
#define NOMAIN 1
#include "new_fourier_anneal.cpp"
#undef NOMAIN


class TrefFKAnnealOnS3: public TrefFKAnneal {

public:

  TrefFKAnnealOnS3(const char* knot_filename, const char* params = ""):TrefFKAnneal(knot_filename, params) {
  }

  virtual float energy() {
    Curve<Vector4> curve;
    knot.toCurveOnS3(NODES,&curve);
    curve.link();
    curve.make_default();
    return 1./curve.thickness();
  }


};

int main(int argc, char** argv) {
  TrefFKAnnealOnS3 * t;
  if (argc != 4) {
    cout << "Usage: " << argv[0] << " [n|3|4] filename params " << endl;
    cout << "    only 3 supported " << endl;
    exit(1);
  }
  switch(argv[1][0]) {
  case '3': t= new TrefFKAnnealOnS3(argv[2],argv[3]); 
            t->do_anneal();
            break;
  default:
    cerr << "Wrong Fourier Knot type [n|3|4]\n";
    cerr << "    only 3 supported " << endl;
    exit(1);
  }
  return 0;
}
