#include "fourier_3_1.h"
#include "fourier_4_1.h"

#warning "!!!!! Not finished, looks wrong !!!!!!!!"

void gen3(const int N, const char* infile) {

  TrefoilFourierKnot fk(infile);

  float isampling = 1./(float)N, t;
  Vector3 r; float f1,f2,f3;
  for (int i=0;i<N;++i) {
    r.zero();
    t = adjust((float)i*isampling);
    // XXX optimise/cache this
    for (uint i=0;i<fk.csin.size();++i) {
      f1 = (float)(3*i+1)*(2.*M_PI);
      f2 = (float)(3*i+2)*(2.*M_PI);
      f3 = (float)(3*i+3)*(2.*M_PI);
      // formula for cos(a)-sin(a)?
      r += Vector3( f1*f1*fk.csin[i][0]*cos(f1*t)-f2*f2*fk.csin[i][1]*cos(f2*t),
                   -f1*f1*fk.csin[i][0]*sin(f1*t)-f2*f2*fk.csin[i][1]*sin(f2*t),
                   -f3*f3*fk.csin[i][2]*(sin(f3*t)));
    }
    cout << t << " " << r.norm() << endl;
  }

//  fk.toCurve(adjust_with_spline,N,&knot);
//  knot.header("fourier trefoil","coeff2pkf","","");
//  knot.writePKF(outfile);
}

int main(int argc, char **argv) {

  if (argc!=4) {
    cout << "Usage : " << argv[0] << " <n/3/4>"
            " <N> <coeff_file>\n";
    exit(0);
  }

  int N = atoi(argv[2]);
  char *infile = argv[3];

  gen3(N,infile);
  return 0;
}
