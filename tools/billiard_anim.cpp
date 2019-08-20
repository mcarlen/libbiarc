/*!
  \file sigma2inventor.cpp
  \ingroup ToolsGroup
  \brief Convert 2D sigma(s) to iv or obj file.
*/

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include "../include/Curve.h"
#include "../experimental/pngmanip/colors.h"
#include <algorithm>

#define b3 vector<Biarc<Vector3> >::iterator
#define all(a) a.begin(), a.end()

struct contact {
  float s, sigma, tau;
};

typedef contact contact;

bool operator<(const contact &c1, const contact &c2) {
  return c1.s < c2.s;
}

class CContact {
  public:
    Vector3 p[3];
      CContact(Vector3 p0,Vector3 p1,Vector3 p2) {
            p[0] = p0; p[1] = p1; p[2] = p2;
              }
};

float interpolate(const vector<contact> &contacts, float s) {
  if (s==0 || s==1) return contacts[0].sigma;
  if (s>1) s -= 1;
  if (s<0) s += 1;
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

vector<contact> sigmaf;

void dump_frame(vector<CContact> &struts, int flag) {
  cout << "    Separator {\n";
  /*
  cout << "      MaterialBinding { value PER_PART }\n"
       << "      Material { diffuseColor [ 0 1 0,";
  for (int i=0;i<7;++i) cout << "1 1 1,";
  cout << "0 0 1 ] }\n";
  */
  cout << "      Coordinate3 {\npoint [\n";
  for (int i = flag; i < static_cast<int>(struts.size()); ++i) {
    cout << struts[i].p[0] << ",\n";
  }
  cout << struts.back().p[2] << "\n]\n      }\n";
  if (flag==0)
    cout << "      LineSet { numVertices [ 10 ] }\n";
  else
    cout << "      LineSet { numVertices [ 9 ] }\n";
  cout << "    }\n";
}

int main(int argc, char **argv) {

  if (argc!=5) {
    cout << "Usage : " << argv[0] << " <pkf in> <contacts> <start> <steps>\n";
    return 1;
  }

  Curve<Vector3> c(argv[1]);
  c.link();
  c.make_default();
  c.normalize();
  c.make_default();

  float s0 = atof(argv[3]);
  int N = atoi(argv[4]);

  // read in contact file
  contact ct;
  ifstream in(argv[2],ios::in);
  while (in >> ct.s >> ct.sigma) { // >> ct.tau) {
    // if (ct.s >= 1) ct.s = ct.s - 1;
    if (ct.sigma >= 1) ct.sigma = ct.sigma - 1;
    sigmaf.push_back(ct);
  }
  in.close();
  sort(all(sigmaf));

/*
  float mid = fabsf((sigmaf[0].sigma+sigmaf.back().sigma)*.5);
  ct.s = 0; ct.sigma = mid;
  if (sigmaf[0].s!=0.0)
    sigmaf.insert(sigmaf.begin(),ct);
  ct.s = 1; ct.sigma = mid;
  if (sigmaf.back().s!=1.0)
    sigmaf.push_back(ct);
*/

  for (int i=0;i<100;++i) {
    float sss = (float)i/99.;
    cerr << sss << " " << interpolate(sigmaf, sss) << endl;
  }
//  return 0;

  Vector3 p0, p1;
  float s, sig;
  vector<CContact> curr;
  s = s0;
  sig = interpolate(sigmaf, s);
  p0 = c.pointAt(s);
  p1 = c.pointAt(sig);
  curr.push_back(CContact(p0,(p0+p1)/2,p1));
  for (int i=0;i<8;++i) {
    s = sig; p0 = p1;
    sig = interpolate(sigmaf, s);
    p1 = c.pointAt(sig);
    curr.push_back(CContact(p0,(p0+p1)/2,p1));
  }

  cout << "#Inventor V2.1 ascii\nSeparator {\n";
  cout << "  Blinker {\n    speed .1\n";

  for (int i=0;i<N;++i) {
    dump_frame(curr,0);
    dump_frame(curr,1);
    curr.erase(curr.begin());
    s = sig;
    p0 = curr.back().p[2];
    sig = interpolate(sigmaf, s);
    p1 = c.pointAt(sig);
    curr.push_back(CContact(p0,(p0+p1)/2,p1));
  }

  cout << "  }\n}\n";

  return 0;
}

#endif // DOXYGEN_SHOULD_SKIP_THIS
