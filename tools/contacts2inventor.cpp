/*!
  \file contacts2inventor.cpp
  \ingroup ToolsGroup
  \brief Convert contact chords to obj/iv file.
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

  if (argc!=4) {
    cout << "Usage : " << argv[0] << " <pkf in> <contacts> <flag>\n"
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
  c.normalize();
  c.make_default();

  int flag = atoi(argv[3]);

  // read in contact file
  Vector3 pt1, pt2;
  contact ct;
  ifstream in(argv[2],ios::in);
  while (in >> ct.s >> ct.sigma >> ct.tau) {
    pt1 = c.pointAt(ct.s);
    pt2 = c.pointAt(ct.sigma);
    contacts.push_back(CContact(pt1,(pt1+pt2)*.5,pt2));
  }
  in.close();

  if (contacts.size()<1) {
    cerr << "No contacts found!\n";
    return 1;
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
      cerr << "Write " << v1 << ", " << v2 << ", " << v3 << ", " << v4 << endl;
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
