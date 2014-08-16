#ifndef _UTILS_H_
#define _UTILS_H_

#include "main.h"
#include "../fourierknots/fourier_syn.h"
#include <QStringList>

struct CurveInfo;

// General Sutff
void usage(char* prog);
int parse(int argc, char** argv, ViewerInfo *vi, CurveInfo *ci);

// GFX Stuff
void addBezierCurve(SoSeparator *root, Tube<Vector3>* t, bool BLACK = false);
SoSeparator* drawCircle(const Vector3& p0, const Vector3& t0, const Vector3& p1, bool BLACK = false);

// Frame Normal generation helpers
void frenet_frame_normals(Tube<Vector3>* t, Vector3* nor, int FRENET = 0);
void parallel_frame_normals(Tube<Vector3>* t, Vector3* nor, float TwistSpeed = 0.);
void parallel_ode_frame_normals(Tube<Vector3>* t, Vector3* Normals);
void writhe_frame_normals(Tube<Vector3>* t, Vector3* Normals);
void writhe_ode_frame_normals(Tube<Vector3>* t, Vector3* Normals);

/*!
  \struct CurveInfo
  \ingroup InventorGroup
  \brief Curve information.

  Knot class interface for viewer, containing
  info shared among viewer, interaction callbacks,
  gui routines.
*/
struct CurveInfo {
  // Knot information
  TubeBundle<Vector3>* Knot;

  // Nodes, Segments
  int N, S;
  // Radius, Frame Tolerance
  float R, Tol;
  
  int Closed;
  
  QStringList filenames;
  
  // 3D
  uint TEXTURES;
  QString texture_file;
};

/*!
  \class CurveInterface
	\ingroup InventorGroup
	\brief Interface between curve data and viewer.
*/
class CurveInterface {
  
public:
  
  CurveInterface() : graph_node(0), frame_node(0) {}
  
  struct CurveInfo info;
  
  // Viewer and Coin
  SoSeparator **knot_node;
  SoKnot **knot_shape;
  SoMaterialBinding **material_bindings;
  SoMaterial **materials;
  SoTexture2 *knot_texture;

  SoSeparator *graph_node;
  SoSeparator *frame_node;

public slots:
  int save();
  int exportIV();
  int exportRIB();
  int exportPOV();

  void decreaseTransparency() {
    float transp = materials[0]->transparency[0] + 0.05;
    if (transp>1.0) transp = 1.0;
    for (int i=0;i<info.Knot->tubes();i++) {
      materials[i]->transparency = transp;
    }
  }

  void increaseTransparency() {
    float transp = materials[0]->transparency[0] - 0.05;
    if (transp<0.0) transp = 0.0;
    for (int i=0;i<info.Knot->tubes();i++) {
      materials[i]->transparency = transp;
    }
  }

  void increaseRadius() {
    for (int i=0;i<info.Knot->tubes();i++)
      knot_shape[i]->radius = knot_shape[i]->radius.getValue()*1.1;
//    cout << "Radius : " << knot_shape[0]->radius.getValue() << endl;
  }

  void decreaseRadius() {
    for (int i=0;i<info.Knot->tubes();i++)
      knot_shape[i]->radius = knot_shape[i]->radius.getValue()*0.9;
//    cout << "Radius : " << knot_shape[0]->radius.getValue() << endl;
  }

  void increaseSegments() {
    int tS = knot_shape[0]->segments.getValue()+1;
    if (tS>120) {
      cerr << "Warning : more than 120 segments requested. Set to 120!\n";
      tS = 120;
    }
    for (int i=0;i<info.Knot->tubes();i++)
      knot_shape[i]->segments.setValue(tS);
    // cout << "Segments : " << tS << endl;
  }

  void decreaseSegments() {
    int tS = knot_shape[0]->segments.getValue()-1;
    if (tS<3) {
      cerr << "Warning : more than 120 segments requested. Set to 120!\n";
      tS = 3;
    }
    for (int i=0;i<info.Knot->tubes();i++)
      knot_shape[i]->segments.setValue(tS);
    // cout << "Segments : " << tS << endl;
  }

  void setNumberOfNodes(int N) {
    for (int i=0;i<info.Knot->tubes();i++)
      knot_shape[i]->nodes.setValue(N);
  }

  SoSeparator* frame(int FRAME = 0) {
    
    Tube<Vector3>* t = knot_shape[0]->getKnot();
    float rad = knot_shape[0]->radius.getValue();

    // Construct Frenet Frame using fourier representation
    SoSeparator *frenet = new SoSeparator;
    SoSeparator *sep_tangents  = new SoSeparator;
    SoSeparator *sep_normals   = new SoSeparator;
    SoSeparator *sep_binormals = new SoSeparator;
    frenet->addChild(sep_tangents);
    frenet->addChild(sep_normals);
    frenet->addChild(sep_binormals);

    // Materials
    SoMaterial *ma_t = new SoMaterial, *ma_n = new SoMaterial, *ma_b = new SoMaterial;

    ma_t->diffuseColor.setValue(1,0,0); sep_tangents->addChild(ma_t);
    ma_n->diffuseColor.setValue(0,1,0); sep_normals->addChild(ma_n);
    ma_b->diffuseColor.setValue(0,0,1); sep_binormals->addChild(ma_b);

    SoCoordinate3 *co_tangents  = new SoCoordinate3;
    SoCoordinate3 *co_normals   = new SoCoordinate3; 
    SoCoordinate3 *co_binormals = new SoCoordinate3;

    SoLineSet *ls_tangents  = new SoLineSet;
    SoLineSet *ls_normals   = new SoLineSet;
    SoLineSet *ls_binormals = new SoLineSet;

    sep_tangents->addChild(co_tangents);   sep_tangents->addChild(ls_tangents);
    sep_normals->addChild(co_normals);     sep_normals->addChild(ls_normals);
    sep_binormals->addChild(co_binormals); sep_binormals->addChild(ls_binormals);

    Vector3* Normals = new Vector3[t->nodes()];
    switch(FRAME) {
      case 0:
        frenet_frame_normals(t,Normals,0);
        break;
      case 1:
        frenet_frame_normals(t,Normals,1);
        break;
      case 2:
        parallel_frame_normals(t,Normals);
        break;
      case 3:
        parallel_ode_frame_normals(t,Normals);
        break;
      case 4:
        writhe_frame_normals(t,Normals);
        break;
      case 5:
        writhe_ode_frame_normals(t,Normals);
        break;
    }

    float scale = 2.*rad;
    Vector3 pt, tan, nor, bin, vec;
    for (int i=0;i<t->nodes();++i) {
      pt  = (*t)[i].getPoint();
      tan = (*t)[i].getTangent();
      nor = Normals[i];
      bin = Normals[i].cross(tan);   bin.normalize();

      co_tangents->point.set1Value(2*i,SbVec3f(pt[0],pt[1],pt[2]));
      vec = pt+scale*tan;
      co_tangents->point.set1Value(2*i+1,SbVec3f(vec[0],vec[1],vec[2]));

      co_normals->point.set1Value(2*i,SbVec3f(pt[0],pt[1],pt[2]));
      vec = pt+scale*nor;
      co_normals->point.set1Value(2*i+1,SbVec3f(vec[0],vec[1],vec[2]));

      co_binormals->point.set1Value(2*i,SbVec3f(pt[0],pt[1],pt[2]));
      vec = pt+scale*bin;
      co_binormals->point.set1Value(2*i+1,SbVec3f(vec[0],vec[1],vec[2]));

      ls_tangents->numVertices.set1Value(i,2);
      ls_normals->numVertices.set1Value(i,2);
      ls_binormals->numVertices.set1Value(i,2);
    }
    return frenet;
  }



public:
  
  void clear() { info.Knot = NULL; }

  /*!
     Construct the meshes for all pkf curves currently loaded.
     Close them if requested.
  */
  void makeMesh() {
    // cout << "Generate mesh for all curves"<<flush;
    if (info.Closed)
       for (int i=0;i<info.Knot->tubes();++i)
         (*(info.Knot))[i].link();
    info.Knot->makeMesh(info.N,info.S,info.R,info.Tol);
    // cout << "\t[OK]\n";
  }

  /*!
    Load a set of pkf curves. We expect the filenames list in
    info to contain the files to be loaded.
  */  
  SoSeparator* load() {
    clear();
    if (info.Knot==NULL) info.Knot = new TubeBundle<Vector3>;

    int count = 0;
    for (int i = 0; i < info.filenames.size(); ++i) {
      cout << "Read file : " << info.filenames.at(i).toLocal8Bit().constData() << endl;
      // Guess PKF or VECT
      if (info.filenames.at(i).endsWith(".pkf") ||
          info.filenames.at(i).endsWith(".PKF")) {
        if (!info.Knot->readPKF((const char*)(info.filenames.at(i).toLocal8Bit().constData())))
          cout << "! Could not load " << (const char*)(info.filenames.at(i).toLocal8Bit()) << endl;
        else count++;
      }
      else if (info.filenames.at(i).endsWith(".xyz") ||
               info.filenames.at(i).endsWith(".XYZ")) {
        if (!info.Knot->readXYZ((const char*)(info.filenames.at(i).toLocal8Bit().constData())))
          cout << "! Could not load " << (const char*)(info.filenames.at(i).toLocal8Bit()) << endl;
        else count++;
      }
      else if (info.filenames.at(i).endsWith(".vect") ||
               info.filenames.at(i).endsWith(".VECT")) {
        CurveBundle<Vector3> cb;
        if (!cb.readVECT(info.filenames.at(i).toLocal8Bit().constData()))
          cout << "! Could not load " << (const char*)(info.filenames.at(i).toLocal8Bit()) << endl;

        else {
          for (int k=0;k<cb.curves();++k) {
            cb[k].computeTangents();
            info.Knot->newTube(Tube<Vector3>(cb[k]));
            count++;
          }
        }
      }
      else {
        cerr << "[Warn] Unknown filetype. Skip!\n";
        continue;
      }
    }
    
    if (count==0) return NULL;
       
    // FIXME
    // We arbitrarily choose for N the number of datapoints of the first curve
    // It would be reasonnable to include the option to show exactly what
    // is in the data file
//    if (!info.N)
      info.N = (*(info.Knot))[0].nodes();
//      info.R = (*(info.Knot))[0].thickness_fast()/20.;

    makeMesh();
    return curveSeparator();
  }
    
  void dumpInfo() {
    if (info.Knot->tubes()<=0) {
      cout << "No knot initialized.\n";
      return;
    }
    cout << "Curves    : "<< info.Knot->tubes() << endl;
    for (int i=0;i<info.Knot->tubes();i++) {
      cout << "Curve " << i+1 << endl;
      cout << "Nodes     : "<< (*(info.Knot))[i].nodes() << endl;
      cout << "Segments  : "<< (*(info.Knot))[i].segments() << endl;
      cout << "Radius    : "<< (*(info.Knot))[i].radius() << endl;
      cout << "Length    : "<< (*(info.Knot))[i].length() << endl;
    }
    cout << "Tolerance : " << info.Tol << endl;
    if ((*(info.Knot))[0].isClosed()) cout << "Closed curves!\n";
    cout << "Files : ";
    for (int i = 0; i < info.filenames.size(); ++i)
       cout << info.filenames.at(i).toLocal8Bit().constData()
            << (info.filenames.size()==i+1?"":", ");
    cout << endl;
  }
  
  SoSeparator* curveSeparator() {
    if (graph_node) {
      cout << "CurveInterface::curveSeparator : [Warn]"
           << " There seems to be an already existing "
           << "curve graph node! Doing nothing.\n";
      return graph_node;
    }
    
    this->graph_node = new SoSeparator;

    // Prepare texture for for knot(s)
    if (info.TEXTURES) {
      knot_texture = new SoTexture2;
      knot_texture->filename.setValue((const char*)info.texture_file.toLocal8Bit().constData());
      graph_node->addChild(knot_texture);
      cout << "Texture node added (" << info.texture_file.toLocal8Bit().constData() << ")\n";
    }
    
    // cout << "Create " << info.Knot->tubes() << " SoKnot objects";
    knot_shape = new SoKnot*[info.Knot->tubes()];
    knot_node = new SoSeparator*[info.Knot->tubes()];
    materials = new SoMaterial*[info.Knot->tubes()];
    material_bindings = new SoMaterialBinding*[info.Knot->tubes()];

    for (int i=0;i<info.Knot->tubes();i++) {
      (*(info.Knot))[i].make_default();
      knot_shape[i] = new SoKnot;
      knot_node[i] = new SoSeparator;

      knot_shape[i]->setKnot( &((*(info.Knot))[i]), (*(info.Knot))[i].isClosed()?info.Tol:-1 );
      knot_shape[i]->radius.setValue(info.R);
      knot_shape[i]->segments.setValue(info.S);
      knot_shape[i]->nodes.setValue(info.N);

      materials[i] = new SoMaterial;
      for (int z=0;z<(*(info.Knot))[i].nodes()-((*(info.Knot))[i].isClosed()?0:1);z++) {
        materials[i]->diffuseColor.set1Value(z,ColorTable[i%ColorNum]);
  /*
        materials[i]->diffuseColor.set1Value(z,ColorTable[ColorIdx]);
          ColorIdx=((ColorIdx+1)%ColorNum);
    */
      }
        // Mark beginning of curve!
    //    materials[i]->diffuseColor.set1Value(0,ColorTable[1]);

//      materials[i]->transparency = transp;

      if (1) {
        // Material bundle
        material_bindings[i] = new SoMaterialBinding;
        // material_bindings[i]->value = SoMaterialBinding::PER_PART;
        knot_node[i]->addChild(material_bindings[i]);
      }
      knot_node[i]->addChild(materials[i]);
      //addBezierCurve(root,&(*Knot)[i]);
      knot_node[i]->addChild(knot_shape[i]);
#if 0 // SPHERE_END
      if (!(*(info.Knot))[i].isClosed()) {
        SoTranslation *tr = new SoTranslation;
        SoTranslation *tr2 = new SoTranslation;
        tr->translation.setValue((SbVec3f&)((*(info.Knot))[i][0].getPoint()[0]));
        tr2->translation.setValue((SbVec3f&)((*(info.Knot))[i][(*(info.Knot))[i].nodes()-1].getPoint()[0]));
        SoSphere *sp = new SoSphere;
        sp->radius = info.R;
        SoSeparator *sep1 = new SoSeparator;
        knot_node[i]->addChild(sep1);
        SoSeparator *sep2 = new SoSeparator;
        knot_node[i]->addChild(sep2);
        sep1->addChild(tr);
        sep1->addChild(sp);
        sep2->addChild(tr2);
        sep2->addChild(sp);
      }
#endif
      graph_node->addChild(knot_node[i]);
    }
    cout << "graph_node : " << graph_node << endl;
    return graph_node;
  }
};

#endif /* _UTILS_H_ */



