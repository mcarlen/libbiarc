#include "../../include/Curve.h"

/*!
  Convolution on 0,1
  u input function
  w ouput function

*/
void local_conv(const float u[], float w[], const int n,
                const float mu = .5, const float sigma = .001) {
  float *v = new float[n], x, isigma = 1./sigma;
  for (int i=0;i<n;++i) {
    x = (float)i/(float)n;
    v[i] = exp(-(x-mu)*(x-mu)*isigma);
  }

/* this works
  float *ww = new float[2*n-1];
  for (int k=0;k<2*n-1;++k) {
    ww[k] = 0;
    for (int j=max(0,k-n);j<min(k,n);++j)
      ww[k] += u[j]*v[k-j];
  }
*/

  /* this is with periodic bc's */
  int lj;
  for (int k=n/2;k<n+n/2;++k) {
    w[k-n/2] = 0;
    for (int j=0;j<n;++j) {
      lj = (k-j)%n; if (lj<0) lj += n;
      w[k-n/2] += u[j]*v[lj];
    }
  }

  delete[] v;
}

#define TEST
#ifdef TEST
int main(int argc, char** argv) {

  if (argc!=4) {
    cout << "Usage : " << argv[0] << " N sigma in.pkf\n";
    exit(0);
  }

  float sigma = atof(argv[2]);
  Curve<Vector3> curve(argv[3]);
  int N = atoi(argv[1]);

  float *t = new float[2*N];
  float *curv = new float[2*N];
  float *conv = new float[2*N];

  t[0] = 0;
  float D, maxcurv = 0, maxconv = 0;

  // XXX open curve case
  curve.link();
  curve.make_default(); 
  curve.normalize();
  curve.make_default();
  curve.resample(N);
  curve.make_default(); 
  curve.normalize();
  D = curve.thickness_fast();

  for (int i=0;i<N;++i) {
    curv[2*i]   = 2.*D/curve[i].radius0();
    curv[2*i+1] = 2.*D/curve[i].radius1();
    if (curv[2*i]>maxcurv) maxcurv = curv[2*i];
    if (curv[2*i+1]>maxcurv) maxcurv = curv[2*i+1];
  }

  local_conv(curv,conv,2*N,.5,sigma);
  local_conv(conv,curv,2*N,.5,sigma);
  local_conv(curv,conv,2*N,.5,sigma);

  // Integrate :)
  for (int i=1;i<2*N;++i) {
    t[i] = t[i-1] + pow(1./conv[i-1],5);
    if (conv[i-1]>maxconv) maxconv = conv[i-1];
  }

  for (int i=0;i<2*N;++i)
    cout << (float)i/(float)(2*N) << " " << curv[i]/maxcurv << " "
         << maxconv/conv[i] << " " << t[i]/t[2*N-1] << endl;
         // << conv[i]/maxconv << " " << t[i]/t[2*N-1] << endl;

  delete[] curv;
  delete[] conv;

  Curve<Vector3> c2; float s;
  for (int i=0;i<N;++i) {
    s = t[2*i]/t[2*N-1];
    c2.append(curve.pointAt(s),curve.tangentAt(s));
  }

  c2.writePKF("out.pkf");

  delete[] t;
  return 0;

}
#endif
