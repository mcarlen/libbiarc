/*!
  \file test.cpp
  \ingroup ToolsGroup
  \brief Program for testing purpose only.

*/

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include "../include/Curve.h"
#include "../experimental/pngmanip/colors.h"
#include <algorithm>

struct contact {
  float s, t;
};

struct contact_double {
  float s, sigma, tau;
};

typedef contact_double contact_double;
typedef contact contact;

vector<contact_double> raw;
vector<contact> final, final2, final3;

bool compare(const contact& a, const contact& b) {
  return a.s < b.s;
}

void dump_surface(ofstream& out, vector<contact> &final, Curve<Vector3> *k, int num, int close = 0) {
  
  assert(num<final.size());

  // How many segments
  int Seg = 30;

  out << "#Inventor V2.1 ascii\n\n\nSeparator {\n\n"
    << "  LightModel {\n        model PHONG\n"
    << "  }\n  DirectionalLight {\n"
    <<"        intensity 0.5\n        color 1 1 1\n"
    <<"        direction 1 0 0\n  }\n"
    <<"  DirectionalLight {\n        intensity 0.5\n"
    <<"        color 1 1 1\n"
    <<"        direction -1 0 0\n  }\n"
    <<"  DirectionalLight {\n        intensity 0.5\n"
    <<"        color 1 1 1\n        direction 0 1 0\n"
    <<"  }\n  DEF +0 DirectionalLight {\n"
    <<"        intensity 0.5\n        color 1 1 1\n"
    <<"        direction 0 0 -1\n  }\n"
    <<"  USE +0\n  USE +0\n"
    <<"  Separator {\n\n        ShapeHints {\n"
    <<"          shapeType SOLID\n          creaseAngle 0.5\n"
    <<"        }\n        MaterialBinding {\n"
    <<"          value PER_PART\n        }Material {\n"
    <<"diffuseColor [";

  // Write a color for every slice (i.e. number of contacts)
  RGB col;
  for (unsigned int i=0;i<final.size();++i) {
    map_color_rainbow_cycle(&col,
        (float)i/(float)(final.size()-1),
        0.,1.);
    out << b2f(col.r)/255. << " "
        << b2f(col.g)/255. << " "
        << b2f(col.b)/255. << ",\n";
  }

  out << "]\nspecularColor 0.40000001 0.40000001 0.40000001\n"
      << "shininess 0.6\n#transparency 0.60000000\n}\n";

  out << "Coordinate3 {\npoint [";

  // Write out coordinates
  Vector3 v1, v2, v3, v4;
  for (int j=0;j<num;++j) { // it=contacts.begin();it!=contacts.end();++it) {
    v1 = k->pointAt(final[j].s);   v2 = k->pointAt(final[j].t);
    v3 = k->pointAt(final[j+1].s); v4 = k->pointAt(final[j+1].t);

    for (int i=0;i<Seg;i++) {
      out << v1+(v2-v1)*(float)i/(float)(Seg-1) << ",";
      out << v3+(v4-v3)*(float)i/(float)(Seg-1) << ",";
    }
  }

  if (close) {
    v1 = k->pointAt(final[final.size()-1].s); v2 = k->pointAt(final[final.size()-1].t);
    v3 = k->pointAt(final[0].s);              v4 = k->pointAt(final[0].t);

    for (int i=0;i<Seg;i++) {
      out << v1+(v2-v1)*(float)i/(float)(Seg-1) << ",";
      out << v3+(v4-v3)*(float)i/(float)(Seg-1) << ",";
    }
  }

  out << "]}\n";

  // Write out triangle strip
  out << "TriangleStripSet {\nnumVertices [";
  for (unsigned int i=0;i < num + close;i++)
    out << 2*Seg << ",";
  out << "]\n} }}" << endl;

}

int main(int argc, char **argv) {

  if (argc!=3) {
    cout << "Usage : " << argv[0] << " <pkf> <contacts>\n";
    return 0;
  }

  contact_double c2;
  ifstream in(argv[2],ios::in);
  while (in >> c2.s >> c2.sigma >> c2.tau)
    raw.push_back(c2);
  in.close();

  // double the stuff
  contact c;
  for (int i=0;i<raw.size();i++) { // ++i) {
    c.s = raw[i].s;
    c.t = raw[i].sigma;
    final.push_back(c);
/*
    c.s = raw[i].tau;
    c.t = raw[i].s;
    final.push_back(c);
    */
  }
  
  // sort final by s
  sort(final.begin(),final.end(),compare);

  // remove contacts that do not fit :D
  float tol = 10; // if diff > 1% drop contact
  float tol2 = 0.0;
  float ot = final[0].t, os = final[0].s;
  float s, t, val;
  c.s = os; c.t = ot;
  final2.push_back(c);
  cerr << os << " " << ot << endl;

  for (int i=1;i<final.size();i++) {

    s = final[i].s; t = final[i].t;
    if (s>t) t += 1;
    if (t>1) t -= 1;
    val = (os-s)*(os-s) + (ot-t)*(ot-t);

    // We want a monotone function
    if (ot>t) continue;

//    if (val < 0.00001) continue;
 //   if (fabsf(t - ot) < tol && val > tol2) {

      // cerr << final[i].s << " " << final[i].t << endl;
      cerr << s << " " << t << endl;
      
      c.s = final[i].s; c.t = final[i].t;
      final2.push_back(c);
      
      os = s;
      ot = t;
//    }
  }

  // Dump iv surface
  Curve<Vector3>* k = new Curve<Vector3>(argv[1]);
  k->make_default();
  k->normalize();
  k->make_default();

  char name[100];
  /*
  for (int nn=1;nn<final2.size()-1;nn++) {
    sprintf(name,"surf_%04d.iv", nn);
    ofstream out(name,ios::out);
    dump_surface(out, final2, k, nn);
    out.close();
  }
  */

  sprintf(name,"surf_%04d.iv", final2.size()-1);
  ofstream out(name,ios::out);
  dump_surface(out, final2, k, final2.size()-1, 1);
  out.close();

  delete k;
  return 0;
}

#endif // DOXYGEN_SHOULD_SKIP_THIS
