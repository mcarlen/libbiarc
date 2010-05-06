/*!
  \file follow_contact.cpp
  \ingroup ToolsGroup
  \brief Starting at some point on the curve, follow the contacts to depth max_level.
*/

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include "../include/Curve.h"
#include "../include/algo_helpers.h"
#include "../experimental/pngmanip/colors.h"

// We set this in the Makefile
// #define Dim4
#ifdef Dim4
#define VecType Vector4
#else
#define VecType Vector3
#endif

#define b3 vector<Biarc<VecType> >::iterator

float arclength(Curve<VecType> &c, int n) {
  float s = 0;
  for (int i=0;i<n;++i)
    s += c[i].biarclength();
  return s;
}

float find_s(Curve<VecType> &c, VecType &p) {
  // XXX possible optimization : not necessary to check against all the points ...
  // Find closest point to p on curve c
  float d = 1e22, dl; b3 close;
  for (b3 b=c.begin();b!=c.end();++b) {
    dl = (b->getPoint()-p).norm2();
    if (dl < d) {
      d = dl;
      close = b;
    }
  }

  b3 neighbor;
  if ( (close->getPoint()-close->getNext().getPoint()).norm2() < (close->getPoint()-close->getPrevious().getPoint()).norm2() ) {
    if (close == c.begin())
      neighbor = c.end()-1;
    else
      neighbor = close-1;
  }
  else {
    if (close == c.end()-1)
      neighbor = c.begin();
    else
      neighbor = close+1;
  }

  // recover s for our point p
  float s0 = arclength(c,close->id());
  float s1 = arclength(c,neighbor->id());

  if (s0>s1) {
    float tmp = s0;
    s0 = s1; s1 = tmp;
  }

  d = 1e22;
  float c_s0 = s0, c_s1 = s1, s_best = s0;
  VecType ptrial;

  // Make this bisection better!!!
  int NN = 10000.;
  for (int i=0;i<NN;++i) {
    ptrial = c.pointAt(c_s0 + (c_s1-c_s0)*(float)i/(float)(NN-1));
    dl = (ptrial - p).norm2();
    if (dl<d) {
      d = dl;
      s_best = c_s0 + (c_s1-c_s0)*(float)i/(float)(NN-1);
    }
  }

  return s_best;

}


class CContact {
public:
  VecType p[3];
  CContact(VecType p0,VecType p1,VecType p2) {
    p[0] = p0; p[1] = p1; p[2] = p2;
  }
};
vector<CContact> contacts;

int th_cond(float d,const VecType& p0, const VecType &p1, float tol) {
  if ( (p0-p1).norm()<=d*(1+tol) ) return 1;
  return 0;
}

#ifdef Dim4
Vector4 inversion_in_sphere(Vector4 &p, Vector4 center = Vector4(0,0,0,1),
                         float radius = 2.0) {
  Vector4 pnew,t,tnew,v; 
  float factor, vnorm2;

  v = (p-center);
  vnorm2 = v.norm2();
 
  factor = radius*radius/vnorm2;
  pnew = center + v*factor;
  return pnew;
}
#endif

int new_rhopt(const VecType& p,
              const VecType &a0, const VecType &a1, const VecType &a2,
              const float rad,
              VecType &vec) {
  // Skip if point p is an arc endpoint
  if ((p-a0).norm()<1e-10 || (p-a2).norm()<1e-10) return 0;

  if ((a1-a0).dot(a0-p)<=0 && (a2-a1).dot(a2-p)>=0) {
#ifndef Dim4
    VecType b = (a0-a1).cross(a2-a1); b.normalize();
    VecType t = (a1-a0); t.normalize();
    VecType c = a0 + t.cross(b)*rad;
    VecType phat = p - b.dot(p-c)*b;
#else
    // Compute center of the arc of circle
    VecType dir = (a2-a1) - (a1-a0); dir.normalize();
    // This is the normal vector at the midpoint of the arc
    // given by the bezier points a0,a1,a2
    // so the center is at
    
    VecType a = a1-a0, b = a2-a0; a.normalize(); b.normalize();
    float omega = a.dot(b);
    VecType c = (.5*a0+omega*a1+.5*a2)/(omega+1.) + dir*rad;

    // project p to the plane in which the arc lies
    b = b - b.dot(a)*a;
    b.normalize();
    VecType phat = (p-c).dot(a)*a + (p-c).dot(b)*b + c;
#endif
    VecType x = phat-c; x.normalize();
    vec = c + rad*x;
    return 1;
  }
  return 0;
}

// FIXME this tolerance depends on the size of the arcs, precision in the contact extraction and other
void follow_contact(float thick, float tol, const VecType &p, const VecType &oldp, Curve<VecType>& curve, int max_level, int level = 0) {
  // cerr << "Level " << level << " p=" << p << " oldp=" << oldp << endl;
  if (level>=max_level) return;

  float TOL = thick/10.;
  VecType b0,b1,b2,v1,v2;
  float r;
  for (b3 it=curve.begin();it!=curve.end();++it) {
    it->getBezierArc0(b0,b1,b2);
    r = it->radius0();
    if (new_rhopt(p,b0,b1,b2,r,v1) && th_cond(thick,p,v1,tol)) {
      // We check if v1 and p are the same because we can not tell if p is on biarc *it
      if ((v1-oldp).norm() > TOL && (v1-p).norm() > TOL) {
        contacts.push_back(CContact(p,(p+v1)*.5,v1));
        follow_contact(thick,tol,v1,p,curve,max_level,level+1);
      }
    }
    it->getBezierArc1(b0,b1,b2);
    r = it->radius1();
    if (new_rhopt(p,b0,b1,b2,r,v2) && th_cond(thick,p,v2,tol)) {
      if ((v2-oldp).norm() > TOL && (v2-p).norm() > TOL) {
        contacts.push_back(CContact(p,(p+v2)*.5,v2));
        follow_contact(thick,tol,v2,p,curve,max_level,level+1);
      }
    }
  } 
}

int main(int argc, char **argv) {

#ifdef Dim4
  if (argc!=7) {
    cout << "Usage : " << argv[0] << " <pkf in> <tol> <depth> <startpos[0,1]> <lineset=1/0> <curved=1/0>\n";
#else
  if (argc!=6) {
    cout << "Usage : " << argv[0] << " <pkf in> <tol> <depth> <startpos[0,1]> <lineset=1/0\n";
#endif
    return 1;
  }

  Curve<VecType> c(argv[1]);
  c.link();
  c.make_default();

  int LineSet = atoi(argv[5]);
  float tol = atof(argv[2]);
  int Depth = atoi(argv[3]);
  float starts = atof(argv[4]);
#ifdef Dim4
  int CURVED = atoi(argv[6]);
#endif
  float thick = c.thickness();
  float len = c.length();

  VecType vstart = c.pointAt(starts*len);
  follow_contact(thick,tol,vstart,vstart,c,Depth);

  if (contacts.size()<1) {
    cerr << "No contacts found!\n";
    return 1;
  }
  else
    cerr << contacts.size() << " contacts.\n";

  vector<CContact>::iterator it;

  if (LineSet) {
    // Inventor Line Set Version
    cout << "#Inventor V2.1 ascii\nSeparator {\nCoordinate3 {\npoint [";
    int Seg = 30;
    for (it=contacts.begin();it!=contacts.end();++it) {
#ifdef Dim4
      Vector4 projected;
      if (!CURVED) {
        projected = inversion_in_sphere(it->p[0]);
        cout << projected[0] << " " << projected[1] << " " << projected [2];
        cout << ", ";
        projected = inversion_in_sphere(it->p[2]);
        cout << projected[0] << " " << projected[1] << " " << projected [2];
        cout << ",\n";
      }
      else {
        Vector4 from = it->p[0], to = it->p[2], vcurr; float val;
        for (int j=0;j<Seg;++j) {
          val = (float)j/(float)(Seg-1);
          vcurr = (1.-val)*from + val*to;
          vcurr.normalize();
          projected = inversion_in_sphere(vcurr);
          cout << projected[0] << " " << projected[1] << " " << projected [2];
          cout << ", ";
        }
        cout << endl;
      }
#else
      cout << it->p[0] << ", " << it->p[2] << endl;
      // cerr << find_s(c,it->p[0])/c.length() << " " << find_s(c,it->p[2])/c.length() << endl;
#endif
    }
    cout << "] }\nLineSet {\nnumVertices [ ";
    for (unsigned int i=0;i<contacts.size();++i) {
#ifdef Dim4
      if (CURVED)
        cout << Seg << ", ";
      else
#endif
        cout << "2, ";
    }
    cout << "] } }";
  }
  else {
    // Use this in blender struts.py plugin
    for (it=contacts.begin();it!=contacts.end();++it) {
#ifdef Dim4
      Vector4 projected;
      if (!CURVED) {
        projected = inversion_in_sphere(it->p[0]);
        cout << projected[0] << " " << projected[1] << " " << projected [2] << " ";
        projected = inversion_in_sphere(it->p[1]);
        cout << projected[0] << " " << projected[1] << " " << projected [2] << " ";
        projected = inversion_in_sphere(it->p[2]);
        cout << projected[0] << " " << projected[1] << " " << projected [2] << endl;
      }
      else {
        int Seg = 30;
        Vector4 from = it->p[0], to = it->p[2], vcurr; float val;
        projected = inversion_in_sphere(from);
        cout << projected[0] << " " << projected[1] << " " << projected[2];
        for (int j=1;j<Seg-1;++j) {
          val = (float)j/(float)(Seg-1);
          vcurr = (1.-val)*from + val*to;
          vcurr.normalize();
          projected = inversion_in_sphere(vcurr);
          cout << " 0 0 0 "; // Dummy
          cout << projected[0] << " " << projected[1] << " " << projected[2] << endl;
          cout << projected[0] << " " << projected[1] << " " << projected[2];
        }
        cout << " 0 0 0 ";
        projected = inversion_in_sphere(to);
        cout << projected[0] << " " << projected[1] << " " << projected[2];

        cout << endl;
      }
#else
      cout << it->p[0] << " " << it->p[1] << " " << it->p[2] << endl;
#endif
    }
  }
  contacts.clear();

  return 0;
}

#endif // DOXYGEN_SHOULD_SKIP_THIS
