#ifndef _UTILS_H_
#define _UTILS_H_

#include "main.h"

struct CurveInfo;

// General Sutff
void usage(char* prog);
int parse(int argc, char** argv, ViewerInfo *vi, CurveInfo *ci);

// GFX Stuff
void addBezierCurve(SoSeparator *root, Tube<Vector3>* t);
SoSeparator* drawCircle(const Vector3& p0, const Vector3& t0, const Vector3& p1);

/*
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

class CurveInterface {
  
public:
  
  CurveInterface() {}
  
  struct CurveInfo info;
  
  // Viewer and Coin
  SoSeparator **knot_node;
  SoKnot **knot_shape;
  SoMaterialBinding **material_bindings;
  SoMaterial **materials;
  SoTexture2 *knot_texture;
  
  SoSeparator *graph_node;

public slots:
  int load();
  int save();
  int exportIV();
  int exportRIB();
  int exportPOV();

public:
  
  void clear() {
    int Tubes = info.Knot->tubes();
    if (Tubes>0) {
      graph_node->unref();
      if (materials[0]) cerr << "Should no longer exist!\n";
      if (info.Knot) { delete info.Knot; info.Knot = NULL; }
    }
  }

  /*!
     Construct the meshes for all pkf curves currently loaded.
     Close them if requested.
  */
  void makeMesh() {
    cout << "Generate mesh for all curves"<<flush;
    if (info.Closed)
       for (int i=0;i<info.Knot->tubes();++i)
         (*(info.Knot))[i].link();
    info.Knot->makeMesh(info.N,info.S,info.R,info.Tol);
    cout << "\t[OK]\n";
  }

  /*!
    Load a set of pkf curves. We expect the filenames list in
    info to contain the files to be loaded.
  */  
  SoSeparator* load(float transp) {
    clear();
    for (int i = 0; i < info.filenames.size(); ++i) {
      cout << "Read file : " << info.filenames.at(i).toLocal8Bit().constData() << endl;
      info.Knot->readPKF((const char*)(info.filenames.at(i).toLocal8Bit().constData()));
    }
    
    // FIXME
    // We arbitrarily choose for N the number of datapoints of the first curve
    // It would be reasonnable to include the option to show exactly what
    // is in the data file
    if (!info.N)
      info.N = (*(info.Knot))[0].nodes();

    makeMesh();
    return curveSeparator(transp);
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
    }
    cout << "Tolerance : " << info.Tol << endl;
    if ((*(info.Knot))[0].isClosed()) cout << "Closed curves!\n";
    cout << "Files : ";
    for (int i = 0; i < info.filenames.size(); ++i)
       cout << info.filenames.at(i).toLocal8Bit().constData()
            << (info.filenames.size()==i+1?", ":"");
    cout << endl;
  }
  
  SoSeparator* curveSeparator(float transp) {
    cout << "curveSeparator start\n" << flush;
    if (graph_node) {
      cout << "CurveInterface::curveSeparator : [Warn]"
           << " There seems to be an already existing curve graph node! Doing nothing.\n";
      return graph_node;
    }
    
    cout << "Let's see\n" << flush;
    
    this->graph_node = new SoSeparator;
    cout << "nope\n" << flush;
    // Prepare texture for for knot(s)
    #if 0
    if (info.TEXTURES) {
      knot_texture = new SoTexture2;
      knot_texture->filename.setValue((const char*)info.texture_file.constData());
      graph_node->addChild(knot_texture);
      cout << "Texture node added (" << info.texture_file.data() << ")\n";
    }
    #endif
    
    cout << "hmmm" << endl << flush;
    
    cout << "Create " << info.Knot->tubes() << " SoKnot objects";
    knot_shape = new SoKnot*[info.Knot->tubes()];
    knot_node = new SoSeparator*[info.Knot->tubes()];
    materials = new SoMaterial*[info.Knot->tubes()];
    material_bindings = new SoMaterialBinding*[info.Knot->tubes()];

    return NULL;

    // FIXME : we need an own SoSeparator for each SoKnot instance!!!
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

      materials[i]->transparency = transp;

      if (1) {
        // Material bundle
        material_bindings[i] = new SoMaterialBinding;
        // material_bindings[i]->value = SoMaterialBinding::PER_PART;
        knot_node[i]->addChild(material_bindings[i]);
      }
      knot_node[i]->addChild(materials[i]);
      //addBezierCurve(root,&(*Knot)[i]);
      knot_node[i]->addChild(knot_shape[i]);
#if 1 // SPHERE_END
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
    return graph_node;
  }
};

#endif /* _UTILS_H_ */



