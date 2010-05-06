/*!
 \file billiard_poly.cpp
 \ingroup ToolsGroup
 \brief Construct billiard polygon.
 
 Given the contacts for the trefoil billiard (from follow_contact tool)
 write the vertices of the actual polygon of the billiard to stdout.
*/
#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include <iostream>
#include <vector>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <map>
using namespace std;
#define dump(a) a.x << " " << a.y << " " << a.z
struct Vec { double x,y,z; };
double diff(Vec& v1, Vec& v2) {
  return sqrt((v1.x-v2.x)*(v1.x-v2.x)+(v1.y-v2.y)*(v1.y-v2.y)+(v1.z-v2.z)*(v1.z-v2.z));
}

int next(vector<pair<Vec,Vec> > &pt, Vec& v) {
  int i = 0;
  while (i<pt.size()) {
    if (diff(pt[i].first,v)<1e-8) break;
    else if (diff(pt[i].second,v)<1e-8) {
      Vec tmp = pt[i].first;
      pt[i].first = pt[i].second; pt[i].second = tmp;
      break;
    }
    ++i;
  }
  cerr << i << " / " << pt.size() << endl;
  return i;
}

int main(int argc, char** argv) {
  if (argc!=2) return -1;
  char* file = argv[1];
  FILE* id = fopen(file,"r");
  Vec v,v2;
  vector<pair<Vec,Vec> > pt, ptmp; double d;
  while (!feof(id)) {
    fscanf(id,"%lg %lg %lg %lg %lg %lg %lg %lg %lg\n",
               &v.x, &v.y, &v.z, &d, &d, &d, &v2.x, &v2.y, &v2.z);
    pt.push_back(pair<Vec,Vec>(v,v2));
  }

  if (pt.size()==1) { cout << dump(pt[0].first) << endl << dump(pt[0].second) << endl; return 1; }

  int i = 0;
  pair<Vec,Vec> pp = pt[i];
  bool ok = true;
  while (ok) {
    cout << dump(pp.first) << endl;
    pt.erase(pt.begin()+i);
    i = next(pt,pp.second);
    if (i<pt.size()) pp = pt[i];
    else ok = false;
  }

  return 1;
}

#endif
