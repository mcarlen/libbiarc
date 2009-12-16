/*!
  \file sigma_composition.cpp
  \ingroup ToolsGroup
  \brief compute $\sigma^k(s)$.
         input file :
         s sigma tau
         ...

*/

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include "../include/Curve.h"
#include <algorithm>

struct container {
  float s,sigma,tau;
};

bool operator<(const container& a, const container& b) {
      return a.s < b.s;
}

vector<container> contacts;

// we need the first and last point t=0, t=1 !!
float interpolate(const vector<container> &contacts, float s) {
  if (s==0 || s==1) return contacts[0].sigma;
  if (s>1) s -= 1;
  float sigma = contacts[0].sigma;
  unsigned int i = 1;
  while (s > contacts[i].s  && i < contacts.size()-1) {
    sigma = contacts[i].sigma;
    ++i;
  }
  if (contacts[i-1].sigma <= contacts[i].sigma) {
    return (sigma + (s - contacts[i-1].s)/(contacts[i].s - contacts[i-1].s)*(contacts[i].sigma - contacts[i-1].sigma));;
  }
  else {
    float ret = (sigma + (s - contacts[i-1].s)/(contacts[i].s - contacts[i-1].s)*(contacts[i].sigma + 1. - contacts[i-1].sigma));;
    if (ret>=1) ret -= 1;
    return ret;
  }
}

#ifndef TEST
int main(int argc, char** argv) {

  if (argc!=3) {
    cout << "Usage : " << argv[0] << " <s sigma tau file> <power>\n";
    exit(0);
  }
  int k = atoi(argv[2]);
  if (k<1) return 0;
  
  struct container vals, vals2;
  ifstream in(argv[1],ios::in);
  while (in >> vals.s >> vals.sigma >> vals.tau) {
    if (vals.s >= 1) vals.s = vals.s - 1;
    if (vals.sigma >= 1) vals.sigma = vals.sigma - 1;
    contacts.push_back(vals);
  }
  in.close();

  float mid = fabsf((contacts[0].sigma+contacts.back().sigma)*.5);
  vals.s = 0; vals.sigma = mid;
  contacts.insert(contacts.begin(),vals);
  vals.s = 1; vals.sigma = mid;
  contacts.push_back(vals);

  vector<container> final = contacts;
  for (int i=2;i<=k;++i)
    for (int j=0;j<final.size();++j) {
      float v = final[j].sigma;
      if (v<0) cout << v << " neg\n";
      if (v>=1) cout << v << " pos\n";
      final[j].sigma = interpolate(contacts, v);
    }

  for (int i=0;i<final.size();++i)
    cout << final[i].s << " " << final[i].sigma << endl;

  return 0;
}
#else
int main() {
  struct container c;
  int n = 5;
  for (int i=0;i<n;++i) {
    c.s = (float)i/(float)(n-1); c.sigma = (float)i/(float)(n-1);
    contacts.push_back(c);
  }

  cerr << interpolate(contacts, 1.) << endl;
  for (int i=0;i<100;++i) {
    cout << (float)i/99. << " " << interpolate(contacts, (float)i/99.) << endl;
  }
}
#endif

#endif // DOXYGEN_SHOULD_SKIP_THIS
