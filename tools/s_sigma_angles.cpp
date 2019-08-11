/*!
  \file s_sigma_angles.cpp
  \ingroup ToolsGroup
  \brief Program for testing purpose only.

*/

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include "../include/Curve.h"
#include "../experimental/pngmanip/colors.h"
#include "../include/algo_helpers.h"
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
	Vector3 t0, t1;
};
typedef CStrut CStrut;
float thick;

void getContacts(Curve<Vector3>& curve, vector<contact>& stcontacts, vector<CStrut> *contacts) {
  vector<contact>::iterator it;
  CStrut strut;
  for (it=stcontacts.begin();it!=stcontacts.end();++it) {
    strut.p0 = curve.pointAt(it->s);
		strut.t0 = curve.tangentAt(it->s);

    if (it->t > 1) {
      strut.p1 = curve.pointAt(it->t-1);
      strut.t1 = curve.tangentAt(it->t-1);
		}
    else {
      strut.p1 = curve.pointAt(it->t);
      strut.t1 = curve.tangentAt(it->t);
		}

    float d = (strut.p0 - strut.p1).norm();
		cout.precision(12);
		cout << it->s << " " << strut.t0.dot(strut.t1) << " "
		     << strut.t0.dot(strut.p1 - strut.p0) << " "
		     << strut.t1.dot(strut.p0 - strut.p1) << " "
		     << d << " "
		     << std::abs(d-thick) << endl;
  }
}

int main(int argc, char **argv) {

  if (argc!=3) {
    cout << "Usage : " << argv[0] << " <pkf> <contacts>\n";
    return 0;
  }

  vector<contact_double> raw;
  contact_double c2;
  ifstream in(argv[2],ios::in);
  while (in >> c2.s >> c2.sigma)// >> c2.tau)
    raw.push_back(c2);
  in.close();

  // double the stuff
  contact c;
  vector<contact> final, final2;
  for (unsigned int i=0;i<raw.size();i++) {
    c.s = raw[i].s;
    c.t = raw[i].sigma;
//		if (c.s > c.t) c.t += 1.0;
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

  for (unsigned int i=1;i<final.size();i++) {

    s = final[i].s; t = final[i].t;
    if (s>1. || s<0.) continue;

    val = (os-s)*(os-s) + (ot-t)*(ot-t);
 //   if (val < 0.0001) continue;

    // We want a monotone function
		/*
    if (ot>t) {
      cout << "Not monotonic s=" << s << ",t=" << t << ",ot=" << ot << '\n';
      continue;
    }
		*/

    c.s = final[i].s; c.t = final[i].t;
    final2.push_back(c);

    os = s;
    ot = t;
  }

  Curve<Vector3>* k = new Curve<Vector3>(argv[1]);
	k->link();
  k->make_default();
  k->normalize();
  k->make_default();
  thick =	k->thickness();

  vector<CStrut> contacts;
	// Write angles between chords and tangents. and angle between tangents
  getContacts(*k, final2, &contacts);

  delete k;
  contacts.clear();
  return 0;
}

#endif // DOXYGEN_SHOULD_SKIP_THIS
