/*!
  \file clif_tref_contactset.cpp
  \ingroup ToolsGroup
  \brief Compute the contact set and surface (iv file) for the clifford trefoil in S^3.

  Example for an optimal g-Trefoil
  ./contactset clif_tref_s3_100.pkf 0.0001 2>contacts.txt >surface.iv
*/

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include "../include/Curve.h"
#include "../include/algo_helpers.h"
#include "../experimental/pngmanip/colors.h"
#include <algorithm>

// #define VERBOSE
#define b4 vector<Biarc<Vector4> >::iterator
#define dump3(v) (v)[0] << " " << (v)[1] << " " << (v)[2]

class ssigma {
public:
  float s,sigma;
  ssigma(float _s, float _sigma) : s(_s), sigma(_sigma) {}
  bool operator<(const ssigma &c) const { return (s < c.s); }
};

float arclength(Curve<Vector4> &c, int n) {
  float s = 0;
  for (int i=0;i<n;++i)
    s += c[i].biarclength();
  return s;
}

float find_s(Curve<Vector4> &c, Vector4 &p) {
  // XXX possible optimization : not necessary to check against all the points ...
  // Find closest point to p on curve c
  float d = 1e22, dl; b4 close;
  for (b4 b=c.begin();b!=c.end();++b) {
    dl = (b->getPoint()-p).norm2();
    if (dl < d) {
      d = dl;
      close = b;
    }
  }

  b4 neighbor;
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
  Vector4 ptrial;

  // Make this bisection better!!!
  int NN = 1000;
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


class CContact {
public:
  Vector4 p[3];
  CContact(Vector4 p0,Vector4 p1,Vector4 p2) {
    p[0] = p0; p[1] = p1; p[2] = p2;
  }
};

class contact3 {
public:
  Vector3 p[3];
  contact3(Vector3 p0,Vector3 p1,Vector3 p2) {
    p[0] = p0; p[1] = p1; p[2] = p2;
  }
};

vector<CContact> contacts;

int th_cond(float d,Vector4& p0, Vector4 &p1, float tol) {
  if ( (p0-p1).norm()<=d*(1+tol) ) return 1;
  return 0;
}

int new_rhopt(const Vector4& p,
              const Vector4 &a0, const Vector4 &a1, const Vector4 &a2,
              const float rad,
              Vector4 &vec) {
  // Skip if point p is an arc endpoint
  if ((p-a0).norm()<1e-10 || (p-a2).norm()<1e-10) return 0;

  if ((a1-a0).dot(a0-p)<=0 && (a2-a1).dot(a2-p)>=0) {
    
    // Compute center of the arc of circle
    Vector4 dir = (a1-a0)-(a2-a1); dir.normalize();
    // This is the normal vector at the midpoint of the arc
    // given by the bezier points a0,a1,a2
    // so the center is at
    Vector4 a = a1-a0, b = a2-a0; a.normalize(); b.normalize();
    float omega = a.dot(b);
    Vector4 c = (.5*a0+omega*a1+.5*a2)/(omega+1.) + dir*rad;
    // project p to the plane in which the arc lies
    b = b - b.dot(a)*a;
    b.normalize();
    Vector4 proj = (p-c);
    proj = proj - proj.dot(a)*a;
    proj.normalize(); proj = proj - proj.dot(b)*b;
    proj.normalize();
    Vector4 phat = p - proj.dot(p)*proj;

    Vector4 x = phat-c; x.normalize();
    vec = c + rad*x;
    return 1;
  }
  return 0;
}

void iv_dump(vector<CContact> &newcontacts, int CURVED = 0) {
  // Write inventor contact surface to stdout

  // How many segments
  int Seg = 30;
  vector<contact3>::iterator it3;

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
  for (unsigned int i=0;i<newcontacts.size();++i) {
    map_color_rainbow_cycle(&col,
        (float)i/(float)(newcontacts.size()-1),
        0.,1.);
    cout << b2f(col.r)/255. << " "
      << b2f(col.g)/255. << " "
      << b2f(col.b)/255. << ",\n";
  }

  cout << "]\nspecularColor 0.40000001 0.40000001 0.40000001\n"
    << "shininess 0.6\n#transparency 0.60000000\n}\n";

  cout << "Coordinate3 {\npoint [";

  // Write out coordinates
  Vector3 v1,v2,v3,v4;
  Vector4 from, to, from2, to2, vec4, vec4_1, vec4_2;
  for (uint j=0;j<newcontacts.size();++j) {
    from = newcontacts[j].p[0];
    to   = newcontacts[j].p[2];
    from2 = newcontacts[(j+1)%newcontacts.size()].p[0];
    to2   = newcontacts[(j+1)%newcontacts.size()].p[2];

    if (!CURVED) {
      vec4 = inversion_in_sphere(from);
      v1 = Vector3(vec4[0],vec4[1],vec4[2]);
      vec4 = inversion_in_sphere(to);
      v2 = Vector3(vec4[0],vec4[1],vec4[2]);
      vec4 = inversion_in_sphere(from2);
      v3 = Vector3(vec4[0],vec4[1],vec4[2]);
      vec4 = inversion_in_sphere(to2);
      v4 = Vector3(vec4[0],vec4[1],vec4[2]);
    }
    for (int i=0;i<Seg;i++) {
      float val = (float)i/(float)(Seg-1);
      if (CURVED) {
        vec4 = (1.-val)*from + val*to;
        vec4.normalize();
        vec4_1 = inversion_in_sphere(vec4);
        vec4 = (1.-val)*from2 + val*to2;
        vec4.normalize();
        vec4_2 = inversion_in_sphere(vec4);
        cout << Vector3(vec4_1[0],vec4_1[1],vec4_1[2]) << ",";
        cout << Vector3(vec4_2[0],vec4_2[1],vec4_2[2]) << ",";
      }
      else {
        cout << v1+(v2-v1)*(float)i/(float)(Seg-1) << ",";
        cout << v3+(v4-v3)*(float)i/(float)(Seg-1) << ",";
      }
    }
  }

  cout << "]}\n";

  // Write out triangle strip
  cout << "TriangleStripSet {\nnumVertices [";
  for (unsigned int i=0;i<newcontacts.size();i++)
    cout << 2*Seg << ",";
  cout << "]\n} }}" << endl;
}

void obj_dump(vector<CContact> &newcontacts, int CURVED = 0) {
  int Seg = 30;
  Vector3 v1,v2,v; Vector4 from, to, v4, v4_final;
  for (uint j=0;j<newcontacts.size();++j) {
    from = newcontacts[j].p[0];
    to   = newcontacts[j].p[2];
    if (!CURVED) {
      v4 = inversion_in_sphere(from);
      v1 = Vector3(v4[0],v4[1],v4[2]);
      v4 = inversion_in_sphere(to);
      v2 = Vector3(v4[0],v4[1],v4[2]);
    }
    for (int i=0;i<Seg;i++) {
      float val = (float)i/(float)(Seg-1);
      if (CURVED) {
        v4 = (1.-val)*from + val*to;
        v4.normalize();
        v4_final = inversion_in_sphere(v4);
        v = Vector3(v4_final[0],v4_final[1],v4_final[2]);
      }
      else {
        v = (1.-val)*v1 + val*v2;
      }
      cout << "v " << v << endl;
      cout << "vt " << (float)j/(float)(newcontacts.size()-1) << " " << (float)i/(float)(Seg-1) << endl;
    }
  }
  for (uint i=0;i<newcontacts.size();++i) {
    for (int j=0;j<Seg-1;j++) {
      cout << "f "
           << i*Seg+j+1 << "/" << i*Seg+j+1 << "/ "
           << i*Seg+j+2 << "/" << i*Seg+j+2 << "/ "
           << (((i+1)%newcontacts.size())*Seg)+j+2 << "/" << (((i+1)%newcontacts.size())*Seg)+j+2 << "/ "
           << (((i+1)%newcontacts.size())*Seg)+j+1 << "/" << (((i+1)%newcontacts.size())*Seg)+j+1 << "/ "
           << endl;
    }  
  }
}

// http://en.wikipedia.org/wiki/Circumscribed_circle
void circle3p(Vector3& p1, Vector3& p2, Vector3& p3) {
  Vector3 a = (p1-p2), b = (p2-p3), c = (p3-p1);
  float radius = a.norm()*b.norm()*c.norm()/2./(a.cross(b)).norm();

  float alpha, beta, gamma, denom;
  denom = 2*(a.cross(b)).norm2();
  alpha = b.norm2()*(a.dot(-c))/denom;
  beta  = c.norm2()*((-a).dot(b))/denom;
  gamma = a.norm2()*(c.dot(-b))/denom;

  Vector3 center = alpha*p1 + beta*p2 + gamma*p3;

//  cerr << "Radius " << radius << "\nCenter " << center << endl;

  Vector3 e1 = p1-center, e2 = c-center;
  e1.normalize();
  e2 = e2 - e1.dot(e2)*e1;
  e2.normalize();

  Curve<Vector3> vill;
  for (int i=0;i<100;++i)
    vill.append(center + sin(2.*M_PI*(float)i/100.)*radius*e1 + cos(2.*M_PI*(float)i/100.)*radius*e2, Vector3());
  vill.computeTangents();
  vill.writePKF("vill.pkf");
}


int main(int argc, char **argv) {

  if (argc!=5) {
    cout << "Usage : " << argv[0] << " <pkf in> <tol> <iv=1/obj=0> <curved=1 or 0>" << endl;
    return 1;
  }

  Curve<Vector4> c(argv[1]);
  c.link();
  c.make_default();

  float tol = atof(argv[2]);
  int CURVED = atoi(argv[4]);
  float thick = c.thickness();
#ifdef VERBOSE
  cerr << "Thickness = " << thick << endl;
#endif

  Vector4 b0,b1,b2,p,v;
  float r;
  for (b4 b=c.begin();b!=c.end();++b) {
    p = b->getPoint();
    for (b4 it=c.begin();it!=c.end();++it) {
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
#ifdef VERBOSE
  else
    cerr << contacts.size() << " contacts.\n";
#endif

  vector<CContact>::iterator it;
  vector<ssigma> vssigma;
  float s, sigma;
  for (it=contacts.begin();it!=contacts.end();++it) {
    s     = find_s(c,it->p[0]);
#ifdef VERBOSE
    cerr << "pt diff " << (c.pointAt(s)-it->p[0]).norm() << " ";
#endif
    sigma = find_s(c,it->p[2]);
#ifdef VERBOSE
    cerr << "pt diff " << (c.pointAt(sigma)-it->p[2]).norm() << "\n";
#endif
    vssigma.push_back(ssigma(s,sigma));
  }
  contacts.clear();

  // filter and sort
  sort(vssigma.begin(),vssigma.end());
  // Keep only the sigma(s) and discard tau(s)
  vector<ssigma> tmp;
  for (uint i=0;i<vssigma.size();i+=2) {
    ssigma s1 = vssigma[i], s2 = vssigma[i+1];
    if (s1.sigma > s1.s && s2.sigma > s1.s) {
      if (s1.sigma > s2.sigma)
        tmp.push_back(s1);
      else
        tmp.push_back(s2);
    }
    else if (s1.sigma > s1.s && s2.sigma < s1.s) {
      tmp.push_back(s2);
    }
    else if (s1.sigma < s1.s && s2.sigma > s1.s) {
      tmp.push_back(s1);
    }
    else {
      if (s1.sigma > s2.sigma)
        tmp.push_back(s1);
      else
        tmp.push_back(s2);
    }
  }
  vssigma = tmp;
  tmp.clear();

  contacts.clear();
  for (uint i=0;i<vssigma.size();++i) {
    s = vssigma[i].s; sigma = vssigma[i].sigma;
    Vector4 v0 = c.pointAt(s), v1 = c.pointAt(sigma);
    Vector4 t0 = c.tangentAt(s), t1 = c.tangentAt(sigma);
#ifdef VERBOSE
    cerr << "Dot " << t0.dot(v0-v1) << " " << t1.dot(v0-v1) << endl;
#endif
    contacts.push_back(CContact(v0,(v0+v1)/2,v1));
  }
  vssigma.clear();

  if (atoi(argv[3]))
    iv_dump(contacts,CURVED);
  else
    obj_dump(contacts,CURVED);

  // Write R^3 contacts to stderr
#ifndef VERBOSE
  cerr << "#Inventor V2.1 ascii\n";
  cerr << "Separator { Coordinate3 { point [\n";
  for (uint i=0;i<contacts.size();++i)
    cerr << dump3(inversion_in_sphere(contacts[i].p[0])) << ", "
         << dump3(inversion_in_sphere(contacts[i].p[2])) << ", " << endl;
  cerr << "]}\nLineSet {\n numVertices [\n";
  for (uint i=0;i<contacts.size();++i)
    cerr << "2,";
  cerr << "\n]}}\n";
#endif

  // Villarceau circle
  Vector4 proj = inversion_in_sphere(contacts[0].p[0]);
  Vector3 p0 = Vector3(proj[0],proj[1],proj[2]);
  proj = inversion_in_sphere(contacts[0].p[2]);
  Vector3 p2 = Vector3(proj[0],proj[1],proj[2]);
  Vector4 ptmp = contacts[0].p[1]; ptmp.normalize();
  proj = inversion_in_sphere(ptmp);
  Vector3 p1 = Vector3(proj[0],proj[1],proj[2]);

/*
  p0 = Vector3(cos(2.*M_PI*.1),sin(2.*M_PI*.1),1);
  p1 = Vector3(cos(2.*M_PI*.2),sin(2.*M_PI*.2),1);
  p2 = Vector3(cos(2.*M_PI*.3),sin(2.*M_PI*.3),1);
  */
  circle3p(p0,p1,p2);

  contacts.clear();

  return 0;
}

#endif // DOXYGEN_SHOULD_SKIP_THIS
