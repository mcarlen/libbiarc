/*!
  \file curvesplit.cpp
  \ingroup ToolsGroup
  \brief Split a "closed" curve into segments for given arclength values.

  Splits a closed curve c into segments given the values s0, s1, etc.
  Initially written to be used with billiard_sigmas.py.
  
  Example : ./curvesplit f3.1.pkf `python billiard_sigmas.py`

  Resulting curves are of the form XXXX.pkf
*/

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include "../include/Curve.h"
#include <stdlib.h>
#include <algorithm>

#define all(a) a.begin(),a.end()

void usage(char* prog) {
  cerr << "Usage : " << prog
       <<" file.pkf s0 s1 ...\n";
  exit(1);
}

int main(int argc, char** argv) {
  // Check number of arguments
  if (argc<2||argc>1000) {
    usage(argv[0]);
  }

  string infile(argv[1]);
  Curve<Vector3> c(infile.c_str());
  c.link();
  c.make_default();
  
  float L = c.length();
  vector<float> s;
  for (int i=2;i<argc;++i) {
    float tmp = atof(argv[i]);
    if (tmp>0. && tmp<L)
      s.push_back(tmp);
    else
      cout << "Ignore " << tmp << endl;
  }

  sort(all(s));
  for (uint i=0;i<s.size();++i)
    cout << s[i] << endl;
  
  uint sid = 0, cid = 0;
  Vector3 pt, tg;
  float len = 0;
  char stri[10];
  Curve<Vector3> seg;

  while (sid<s.size()) {
    float val = s[sid];
    while (len<val) {
      seg.append(c[cid]);
      len += c[cid++].biarclength();
    }
    
    pt = c.pointAt(val);
    tg = c.tangentAt(val);
    if ((seg[seg.nodes()-1].getPoint()-pt).norm()>1e-3) {
      seg.append(pt,tg);
      if (sid+1<s.size())
        if (len<s[sid+1])
          len = val;
    }
    
    sprintf(stri,"%04d.pkf",sid);
    sid++;
    pt = seg[seg.nodes()-1].getPoint();
    tg = seg[seg.nodes()-1].getTangent();
    seg.writePKF(stri);
    seg.flush_all();
      seg.append(pt,tg);

  }
  for (int i=cid;i<c.nodes();++i)
    seg.append(c[i]);
  seg.append(c[0]);

  sprintf(stri,"%04d.pkf",sid);
  seg.writePKF(stri);

  return 0;
}

#endif // DOXYGEN_SHOULD_SKIP_THIS
