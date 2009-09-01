/*!
  \file sigma2contact.cpp
  \ingroup ToolsGroup
  \brief From a sigma(s) function, generate the contact struts.

  Takes the function sigma(s) and a pkf knot and writes the corresponding
	struts (start,mid,endpoint) to stdout.
*/

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include "../include/Curve.h"
#include "../experimental/pngmanip/colors.h"

#define b3 vector<Biarc<Vector3> >::iterator

struct contact {
  float s, sigma, tau;
};

typedef contact contact;

class CContact {
  public:
    Vector3 p[3];
      CContact(Vector3 p0,Vector3 p1,Vector3 p2) {
            p[0] = p0; p[1] = p1; p[2] = p2;
              }
};

vector<CContact> contacts;

int main(int argc, char **argv) {

  if (argc!=3) {
    cout << "Usage : " << argv[0] << " <pkf in> <sigma>\n";
    return 1;
  }

  Curve<Vector3> c(argv[1]);
  c.link();
  c.make_default();
  c.normalize();
  c.make_default();

  // read in contact file
  Vector3 pt1, pt2;
  contact ct;
  ifstream in(argv[2],ios::in);
  while (in >> ct.s >> ct.sigma >> ct.tau) {
    pt1 = c.pointAt(ct.s);
    pt2 = c.pointAt(ct.sigma);
    contacts.push_back(CContact(pt1,(pt1+pt2)*.5,pt2));
    // pt2 = c.pointAt(ct.tau);
		// contacts.push_back(CContact(pt1,(pt1+pt2)*.5,pt2));
  }
  in.close();

  if (contacts.size()<1) {
    cerr << "No contacts found!\n";
    return 1;
  }

  // Write contacts in specified output to standart out
  for (vector<CContact>::iterator it=contacts.begin();it!=contacts.end();++it)
    cout << it->p[0] << " " << it->p[1] << " " << it->p[2] << endl;

 contacts.clear();

  return 0;
}

#endif // DOXYGEN_SHOULD_SKIP_THIS
