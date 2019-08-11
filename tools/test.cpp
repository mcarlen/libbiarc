/*!
  \file test.cpp
  \ingroup ToolsGroup
  \brief Program for testing purpose only.

*/

#ifndef DOXYGEN_SHOULD_SKIP_THIS

// Enable this if you only need to extract isolated chords!
// #define ISOLATED_STRUTS

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

bool compare(const contact& a, const contact& b) {
  return a.s < b.s;
}

// Contacts struts
struct CStrut {
  Vector3 p0, p1;
};
typedef CStrut CStrut;


void getContacts(Curve<Vector3>& curve, vector<contact>& stcontacts, vector<CStrut> *contacts) {
  vector<contact>::iterator it;
  CStrut strut;
  for (it=stcontacts.begin();it!=stcontacts.end();++it) {
		// cout << "c : " << it->s << " " << it->t << endl;
    strut.p0 = curve.pointAt(it->s);
    if (it->t >= 1)
      strut.p1 = curve.pointAt(it->t-1);
    else
      strut.p1 = curve.pointAt(it->t);
    contacts->push_back(strut);
  }
}

void dump_obj_surface(ofstream& out, vector<CStrut>& contacts, unsigned int num, int close = 0) {

  assert(num<contacts.size());
  const int Segments = 30;

  float step_size_1, step_size_2;
  Vector3 N_direc_vect_1, N_direc_vect_2;
  Vector3 save1, save2, V1, V2, V3, V4;

  static int FaceIndex1 = 1, FaceIndex2 = 2, FaceIndex3 = 3;

  // foreach contact line pair

  //
  // Main surface (Triangulation)
  //

  out << "g csurface" << endl;
  float s,t,s1,t1;
  for (unsigned int l=0;l< num + close;++l) {

    // set 4 corners of the "rectangle"
    V1 = contacts[l].p0;
    V2 = contacts[l].p1;

    V3 = contacts[(l+1)%contacts.size()].p0;
    V4 = contacts[(l+1)%contacts.size()].p1;

    // compute step size
    step_size_1 = (V2-V1).norm()/(float)Segments; //NO_OF_SUB_TRIS;
    step_size_2 = (V4-V3).norm()/(float)Segments; //NO_OF_SUB_TRIS;

    // get the direction vector for each contact segment
    N_direc_vect_1 = (V2-V1);
    N_direc_vect_2 = (V4-V3);

    N_direc_vect_1.normalize();
    N_direc_vect_2.normalize();

    for (int i=0;i<Segments;i++) {

      s = (float)(l)/(float)(contacts.size()-1);
      t = (float)i/(float)(Segments-1);
      s1 =(float)(l+1)/(float)(contacts.size()-1);
      t1 = (float)(i+1)/(float)(Segments-1);

      // for glm lib make it wrap
      if(s==1) s=0;if(t==1)t=0;if(s1==1)s1=0;if(t1==1)t1=0;

      // triangle 1
			Vector3 vv =  (V1 + (N_direc_vect_1*(i*step_size_1)));
      out << "v " << vv[0] << " " << vv[2] << " " << -vv[1] << endl;
			vv = (V3 + (N_direc_vect_2*(i*step_size_2)));
      out << "v " << vv[0] << " " << vv[2] << " " << -vv[1] << endl;
			vv = (V1 + (N_direc_vect_1*((i+1)*step_size_1)));
      out << "v " << vv[0] << " " << vv[2] << " " << -vv[1] << endl;

      // texture coords
      out << "vt " << s << " " << t << endl;
      out << "vt " << s1 << " " << t << endl;
      out << "vt " << s << " " << t1 << endl;

      //out << "f -3 -2 -1" << endl;
      out << "f "<< FaceIndex1 << "/" << FaceIndex1 << " "
          << FaceIndex2 << "/" << FaceIndex2 << " "
  	      << FaceIndex3 << "/" << FaceIndex3 << endl;

      FaceIndex1+=3; FaceIndex2+=3; FaceIndex3+=3;

      // triangle 2
			vv = (V1 + N_direc_vect_1*((i+1)*step_size_1));
      out << "v " << vv[0] << " " << vv[2] << " " << -vv[1] << endl;
			vv = (V3 + N_direc_vect_2*(i*step_size_2));
      out << "v " << vv[0] << " " << vv[2] << " " << -vv[1] << endl;
			vv = (V3 + N_direc_vect_2*((i+1)*step_size_2));
      out << "v " << vv[0] << " " << vv[2] << " " << -vv[1] << endl;

      // texture coords
      out << "vt " << s << " " << t1 << endl;
      out << "vt " << s1 << " " << t << endl;
      out << "vt " << s1 << " " << t1 << endl;

      // out << "f -3 -2 -1" << endl;
      out << "f "<< FaceIndex1 << "/" << FaceIndex1 << " "
          << FaceIndex2 << "/" << FaceIndex2 << " "
	        << FaceIndex3 << "/" << FaceIndex3 << endl;

      FaceIndex1+=3; FaceIndex2+=3; FaceIndex3+=3;

    }
  }
}

void dump_iv_surface(ofstream& out, vector<CStrut>& contacts, unsigned int num, int close = 0) {

  assert(num<contacts.size());

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
  for (unsigned int i=0;i<contacts.size();++i) {
    map_color_rainbow_cycle(&col,
        (float)i/(float)(contacts.size()-1),
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
  for (unsigned int j=0;j<num + close;++j) { // it=contacts.begin();it!=contacts.end();++it) {

    v1 = contacts[j].p0;   v2 = contacts[j].p1;
    v3 = contacts[(j+1)%contacts.size()].p0; v4 = contacts[(j+1)%contacts.size()].p1;

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

#ifndef ISOLATED_STRUTS
int main(int argc, char **argv) {

  if (argc!=3) {
    cout << "Usage : " << argv[0] << " <pkf> <contacts>\n";
    return 0;
  }

  vector<contact_double> raw;
  contact_double c2;
  ifstream in(argv[2],ios::in);
  while (in >> c2.s >> c2.sigma) // >> c2.tau)
    raw.push_back(c2);
  in.close();

  // double the stuff
  contact c;
  vector<contact> final, final2;
  for (unsigned int i=0;i<raw.size();i++) {
    c.s = raw[i].s;
    c.t = raw[i].sigma;
		if (c.s > c.t) c.t += 1.0;
    final.push_back(c);
/*
    c.s = raw[i].tau;
    c.t = raw[i].s;
		if (c.s > c.t) c.t += 1.0;
    final.push_back(c);
		*/
  }

  // sort final by s
  sort(final.begin(),final.end(),compare);

  float ot = final[0].t, os = final[0].s;
  float s, t, val;
  c.s = os; c.t = ot;
  final2.push_back(c);
  // cerr << os << " " << ot << endl;

  // cout << "Ot : " << final[1].t - ot << endl;

  for (unsigned int i=1;i<final.size();i++) {

    s = final[i].s; t = final[i].t;
    if (s>1. || s<0.) continue;
//    if (s>t) t += 1;

    val = (os-s)*(os-s) + (ot-t)*(ot-t);
    if (val < 0.00001) continue;

/*
    cout << "s=" << s <<",t="<<t <<endl;
    if (fabsf(ot-t) > ttol) {
			cout << "Filter " << ot << " " << t << " ( " << ot-t << " / " << ttol << ")\n";
			continue;
		}
*/
    // We want a monotone function
    if (ot>t) {
      cout << "Not monotonic s=" << s << ",t=" << t << ",ot=" << ot << '\n';
      continue;
    }


    cerr << s << " " << t << endl;

    c.s = final[i].s; c.t = final[i].t;
    final2.push_back(c);

    os = s;
    ot = t;
  }

  // Dump iv surface
  Curve<Vector3>* k = new Curve<Vector3>(argv[1]);
	k->link();
  k->make_default();
  k->normalize();
  k->make_default();

  // Write s sigma contacts to Vector3 contacts
  vector<CStrut> contacts;
  getContacts(*k, final2, &contacts);

  char name[100];
  /*
  for (int nn=1;nn<final2.size()-1;nn++) {
    sprintf(name,"surf_%04d.iv", nn);
    ofstream out(name,ios::out);
    dump_iv_surface(out, contacts, nn);
    out.close();
  }

  */

  sprintf(name,"surf_%04lu.iv", contacts.size()-1);
  ofstream out(name,ios::out);
  dump_iv_surface(out, contacts, contacts.size()-1, 1);
  out.close();

  sprintf(name,"surf.obj");
  out.open(name,ios::out);
  dump_obj_surface(out, contacts, contacts.size()-1, 1);
  out.close();

  // write contacts.txt file
  // format : p0x p0y p0z midpx midpy midpz p1x p1y p1z
  out.open("contacts.txt",ios::out);
  for (unsigned int i=0;i<contacts.size();++i) {
    out << contacts[i].p0 << " "
        << (contacts[i].p0+contacts[i].p1)*.5 << " "
        << contacts[i].p1 << endl;
  }

  delete k;
  contacts.clear();
  return 0;
}
#else

int main(int argc, char **argv) {

  if (argc!=3) {
    cout << "Usage : " << argv[0] << " <pkf> <isolated_contacts>\n";
    return 0;
  }

  Curve<Vector3>* k = new Curve<Vector3>(argv[1]);
	k->link();
  k->make_default();
  k->normalize();
  k->make_default();

  vector<contact> cvec;
  vector<CStrut> contacts;
  contact c; float dummy;
  ifstream in(argv[2],ios::in);
  while (in >> c.s >> c.t >> dummy)
    cvec.push_back(c);
  in.close();

  getContacts(*k, cvec, &contacts);
	for (int i=0;i<contacts.size();++i) {
    cout << contacts[i].p0 << " " << .5*(contacts[i].p0+contacts[i].p1) << " " << contacts[i].p1 << endl;
	}
}

#endif // ISOLATED_STRUTS

#endif // DOXYGEN_SHOULD_SKIP_THIS
