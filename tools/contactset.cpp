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

#define b3 vector<Biarc<Vector3> >::iterator

class CContact {
public:
  Vector3 p[3];
  CContact(Vector3 p0,Vector3 p1,Vector3 p2) {
    p[0] = p0; p[1] = p1; p[2] = p2;
  }
};

vector<CContact> contacts;

int th_cond(float d,Vector3& p0, Vector3 &p1, float tol) {
  if ( fabsf((p0-p1).norm()-d)<d*tol ) return 1;
  return 0;
}

int main(int argc, char **argv) {

  if (argc!=4) {
    cout << "Usage : " << argv[0] << " <pkf in> <tol> <flag>\n"
         << "  flag : 1 -> inventor lineset\n"
         << "         2 -> inventor contact surface\n"
         << "         3 -> obj contact surface\n"
         << "         4 -> contact line as pkf output\n"
         << "     other -> contacts (start midpoint end),\n";
    return 1;
  }

  Curve<Vector3> c(argv[1]);
  c.link();
  c.make_default();

  float tol = atof(argv[2]);
  int flag = atoi(argv[3]);
  float thick = c.thickness();

  Vector3 b0,b1,b2,p,v;
  float r;
  for (b3 b=c.begin();b!=c.end();++b) {
    p = b->getPoint();
    for (b3 it=c.begin();it!=c.end();++it) {
      if (it==b) continue;
      it->getBezierArc0(b0,b1,b2);
      r = it->radius0();
      if (rhopt(p,b0,b1,b2,r,v) && th_cond(thick,p,v,tol))
        contacts.push_back(CContact(p,(p+v)*.5,v));
      it->getBezierArc1(b0,b1,b2);
      r = it->radius1();
      if (rhopt(p,b0,b1,b2,r,v) && th_cond(thick,p,v,tol))
        contacts.push_back(CContact(p,(p+v)*.5,v));
    }
  }

  if (contacts.size()<1) {
    cerr << "No contacts found!\n";
    return 1;
  }

  // Sort contact struts if needed
//  if (flag==2 || flag==3 || flag==4) {
  if (1) {
    vector<CContact> tmp_c;
    float closest_f, tmp_f;
    vector<CContact>::iterator cit,cit2,closest_c;
    Vector3 p,tmp_p;

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
      if (closest_f > thick*0.6) {
        contacts.erase(closest_c);
        continue;
      }

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
    for (it=contacts.begin();it!=contacts.end();++it)
      cout << it->p[0] << ", " << it->p[2] << endl;
    cout << "] }\nLineSet {\nnumVertices [ ";
    for (unsigned int i=0;i<contacts.size();++i)
      cout << "2, ";
    cout << "] } }";
  }
  // Contact surface as inventor file
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
    Vector3 v1,v2,v3,v4;
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
    Curve<Vector3> curve;
    for (it=contacts.begin();it!=contacts.end();++it) {
      curve.append(it->p[1],Vector3(1,0,0));
    }
    curve.link();
    curve.computeTangents();
    curve.writePKF(cout);
  }
  // Contacts start midpoint endpoint (one per row)
  else {
    for (it=contacts.begin();it!=contacts.end();++it)
      cout << it->p[0] << " " << it->p[1] << " " << it->p[2] << endl;
  }

  cerr << contacts.size() << " contacts\n";

  contacts.clear();

  return 0;
}

#endif // DOXYGEN_SHOULD_SKIP_THIS
