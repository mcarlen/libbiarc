/*!
  \file contactset.cpp
  \ingroup ToolsGroup
  \brief Compute the contact chords for a given knot.

  Jana Smutny's rhopt contact computation algorithm.
  Given a pkf curve and a tolerance, write an inventor
  file with the contacts as a lineset to stdout.

  Example : Get Jana's best trefoil contactset with
    ./contactset j3.1.pkf 0.00002
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
  int NN = 1000.;
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

int th_cond(float d,VecType& p0, VecType &p1, float tol) {
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

int main(int argc, char **argv) {

#ifdef Dim4
  if (argc!=5) {
    cout << "Usage : " << argv[0] << " <pkf in> <tol> <flag> <curved=1/0>\n";
#else
  if (argc<4 || argc>5) {
    cout << "Usage : " << argv[0] << " -open <pkf in> <tol> <flag>\n";
#endif
    cout << "  flag : 1 -> inventor lineset\n"
         << "         2 -> inventor contact surface\n"
         << "         3 -> obj contact surface\n"
         << "         4 -> contact line as pkf output\n"
         << "         5 -> sigma(s) contacts\n"
         << "     other -> contacts (start midpoint end),\n";
    return 1;
  }

  bool OPEN = false;
  int first_arg = 1;
  if (argc==5) if (!strcmp(argv[1],"-open")) {
    OPEN = true;
    first_arg = 2;
  }
  Curve<VecType> c(argv[first_arg]);
  if (!OPEN)
    c.link();
  c.make_default();

  float tol = atof(argv[first_arg+1]);
  int flag = atoi(argv[first_arg+2]);
#ifdef Dim4
  int CURVED = atoi(argv[4]);
#endif
  float thick = c.thickness();

  VecType b0,b1,b2,p,v;
  float r;
  for (b3 b=c.begin();b!=c.end();++b) {
    p = b->getPoint();
    for (b3 it=c.begin();it!=c.end();++it) {
      it->getBezierArc0(b0,b1,b2);
      r = it->radius0();
      if (new_rhopt(p,b0,b1,b2,r,v) && th_cond(thick,p,v,tol))
        contacts.push_back(CContact(p,(p+v)*.5,v));
      it->getBezierArc1(b0,b1,b2);
      r = it->radius1();
      if (new_rhopt(p,b0,b1,b2,r,v) && th_cond(thick,p,v,tol))
        contacts.push_back(CContact(p,(p+v)*.5,v));
    }
  }

  if (contacts.size()<1) {
    cerr << "No contacts found!\n";
    return 1;
  }
  else
    cerr << contacts.size() << " contacts.\n";

  // Sort contact struts if needed
//  if (flag==2 || flag==3 || flag==4) {
  if (0) {
    vector<CContact> tmp_c;
    float closest_f, tmp_f;
    vector<CContact>::iterator cit,cit2,closest_c;
    VecType p,tmp_p;

    tmp_c.push_back(contacts.front());
    contacts.erase(contacts.begin());

    while(contacts.size()>0) {
      p = tmp_c.back().p[1];
      closest_c = contacts.begin();
      closest_f = (p-closest_c->p[1]).norm();
      for (cit=contacts.begin()+1;cit!=contacts.end();++cit) {
        tmp_f = (cit->p[1]-p).norm();
        if (tmp_f < closest_f) {
          closest_f = tmp_f; closest_c = cit;
        }
      }

      // Remove isolated struts (we assume they are
      // far away form most other struts)
      /*
      if (closest_f > thick*0.6) {
        contacts.erase(closest_c);
        continue;
      }
      */

      // Swap points if not aligned
      if ((closest_c->p[0]-tmp_c.back().p[0]).norm() >
          (closest_c->p[2]-tmp_c.back().p[0]).norm()) {
        tmp_p = closest_c->p[0];
        closest_c->p[0] = closest_c->p[2];
        closest_c->p[2] = tmp_p;
      }
      tmp_c.push_back(*closest_c);
      contacts.erase(closest_c);
    }
    contacts.clear();
    contacts = tmp_c;
    tmp_c.clear();
  }

  // Write contacts in specified output to standart out

  vector<CContact>::iterator it;
  // Inventor Line Set Version
  if (flag==1) {
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
  // Contact surface as inventor file (this usually doesn't work since isolated contacts mess things up)
  else if (flag==2) {

    // How many segments
    int Seg = 30;

    cout << "#Inventor V2.1 ascii\n\n\nSeparator {\n\n"
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
      cout << b2f(col.r)/255. << " "
           << b2f(col.g)/255. << " "
           << b2f(col.b)/255. << ",\n";
    }

    cout << "]\nspecularColor 0.40000001 0.40000001 0.40000001\n"
         << "shininess 0.6\n#transparency 0.60000000\n}\n";

    cout << "Coordinate3 {\npoint [";

    // Write out coordinates
    VecType v1,v2,v3,v4;
    for (it=contacts.begin();it!=contacts.end();++it) {
      v1 = it->p[0]; v2 = it->p[2];
      if ((it+1)==contacts.end()) {
        v3 = contacts.begin()->p[0]; v4 = contacts.begin()->p[2];
      }
      else {
        v3 = (it+1)->p[0]; v4 = (it+1)->p[2];
      }
      for (int i=0;i<Seg;i++) {
        cout << v1+(v2-v1)*(float)i/(float)(Seg-1) << ",";
        cout << v3+(v4-v3)*(float)i/(float)(Seg-1) << ",";
      }
    }
    cout << "]}\n";

    // Write out triangle strip
    cout << "TriangleStripSet {\nnumVertices [";
    for (unsigned int i=0;i<contacts.size();i++)
      cout << 2*Seg << ",";
    cout << "]\n} }}" << endl;
  }
  // Contact surface as OBJ file
  else if (flag==3) {
  }
  else if (flag==4) {
  // Contact line as pkf output
    Curve<VecType> curve;
    for (it=contacts.begin();it!=contacts.end();++it) {
      curve.append(it->p[1],VecType());
    }
    curve.link();
    curve.computeTangents();
    curve.writePKF(cout);
  }
  // sigma(s) values
  else if (flag==5) {
    float L = c.length(), s, sigma;
    for (it=contacts.begin();it!=contacts.end();++it) {
      s     = find_s(c,it->p[0]);
      sigma = find_s(c,it->p[2]);
      cout << s/L << " " << sigma/L << endl;
      // VecType v0 = c.pointAt(s), v1 = c.pointAt(sigma);
      // cout << v0 << " " << (v0+v1)/2 << " " << v1 << endl;
    }
  }
  // Contacts start midpoint endpoint (one per row)
  else {
    cerr << contacts[0].p[0].norm2() << " " << contacts[0].p[2].norm2() <<endl;
    for (it=contacts.begin();it!=contacts.end();++it)
      cout << it->p[0] << " " << it->p[1] << " " << it->p[2] << endl;
  }

  cerr << contacts.size() << " contacts\n";

  contacts.clear();

  return 0;
}

#endif // DOXYGEN_SHOULD_SKIP_THIS
