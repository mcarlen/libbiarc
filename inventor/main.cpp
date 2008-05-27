/*!
  \file main.cpp
  \ingroup InventorGroup
  \brief Supposed to become the unique viewer combining
         the now separated multiview, skeleton open/closed etc

  This is the most complete viewer. It includes a biarc view.
  Basic editing. pt/pp/tt plot viewer (not finished). Multi
  components are partially supported.
*/

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include "main.h"

#include <Inventor/details/SoPointDetail.h>
#include <Inventor/details/SoLineDetail.h>

SbBool myAppEventHandler(void *userData, QEvent *anyevent);

static void motionfunc(void *data, SoEventCallback *eventCB);
static void mousefunc(void *data, SoEventCallback *eventCB);
bool PRESSED = 0;
vector<Biarc<Vector3> >::iterator picked_biarc;
SbPlaneProjector spp;
SbVec3f UpVector, LeftVector, delta;
float AspectratioX, AspectratioY;
int EditTangent;

// background
unsigned int BackGroundFlag;

// Partial Resample of the curve globals
unsigned int ResamplePartFlag;
unsigned int FirstPoint;
vector<Biarc<Vector3> >::iterator FirstBiarc;

void addBezierCurve(SoSeparator *root, Tube<Vector3>* t);

#define _SCREEN_W_ 800
#define _SCREEN_H_ 600

QWidget *myWindow;
//SoQtExaminerViewer *myViewer;
SoSeparator *root, *interaction, *scene;
SoSeparator *circles;

void usage(char* prog);
int parse(int argc, char** argv);

// Number of nodes, number of circular segments
int N, S;
// Tube radius, mesh correction tolerance
float R, Tol;
char filename[50];

SoSeparator **knot_node;
SoKnot **knot_shape;
TubeBundle<Vector3> *Knot;

SoMaterialBinding **material_bindings;
SoMaterial **materials;
// Color index used for material assignment
static uint ColorIdx = 0;
// Number of colors in ColorTable
const uint ColorNum = 7;
SbColor ColorTable[ColorNum];
static float transp = 0.0;

SoTexture2 *knot_texture;
static uint TEXTURES = 0;
char texture_file[100];

static uint IV_SCENE = 0;
static uint PT_PLOT = 0;
char iv_file[100];
QString ptplot_file;

enum VIEW_MODE { SOLID_VIEW = 1, WIRE_VIEW, BIARC_VIEW };
static VIEW_MODE view_mode = SOLID_VIEW;

void swap_view() {
  switch(view_mode) {
  case SOLID_VIEW: view_mode = WIRE_VIEW; break;
  case WIRE_VIEW: view_mode = BIARC_VIEW; break;
  case BIARC_VIEW: view_mode = SOLID_VIEW; break;
  default: view_mode = SOLID_VIEW; break;
  }
}

/* new
class VVV : public SoQtExaminerViewer {
  //Q_OBJECT
  //SOQT_OBJECT_HEADER(VVV, SoQtExaminerViewer);
public:
  VVV(QWidget *parent=NULL, const char *name=NULL, SbBool embed=TRUE, SoQtFullViewer::BuildFlag flag=BUILD_ALL, SoQtViewer::Type type=BROWSER) :
SoQtExaminerViewer(parent,name,embed,flag,type) { oldx = oldy = -1; }
  ~VVV() {}
*/

SOQT_OBJECT_SOURCE(VVV);

VVV::VVV(QWidget *parent, const char *name, SbBool embed, SoQtFullViewer::BuildFlag flag, SoQtViewer::Type type) :
SoQtExaminerViewer(parent,name,embed,flag,type) { oldx2 = oldy2 = oldx = oldy = -1; }
VVV::~VVV() {}

// u,v in [0,1]
void VVV::update_picked(float u, float v, float u2, float v2) {
  // FIXME : one component only supported for now
  int x,y,x2,y2,i;
  float a,b;

  cout << "u,v  = " << u << " ," << v << endl;
  cout << "u2,v2 = " << u2 << " ," << v2 << endl;

  // reset highlight to zero
  if (oldx2>=0 && oldy2>=0) {
    for (i=oldx;i<=oldx2;i++)
      materials[0]->diffuseColor.set1Value(i,ColorTable[0]);
    for (i=oldy;i<=oldy2;i++)
      materials[0]->diffuseColor.set1Value(i,ColorTable[0]);
  }
  else {
    if (oldx >= 0)
      materials[0]->diffuseColor.set1Value(oldx,ColorTable[0]);
    if (oldy >= 0)
      materials[0]->diffuseColor.set1Value(oldy,ColorTable[0]);
  }

  if (u2>=0 && v2>=0) {
    x = (*Knot)[0].biarcPos(u); //*(*Knot)[0].length());
    y = (*Knot)[0].biarcPos(v); //*(*Knot)[0].length());
    x2 = (*Knot)[0].biarcPos(u2); //*(*Knot)[0].length());
    y2 = (*Knot)[0].biarcPos(v2); //*(*Knot)[0].length());

    cout << "x,y  = " << x << " , " << y << endl;
    cout << "2x,y = " << x2 << " , " << y2 << endl;

    for (i=x;i<=x2;i++)
      materials[0]->diffuseColor.set1Value(i,ColorTable[1]);
    for (i=y;i<=y2;i++)
      materials[0]->diffuseColor.set1Value(i,ColorTable[1]);


    // sort the values s.t u<u2 and v<v2
    circles->removeAllChildren();
    if (x>x2) { int xtmp = x; x = x2; x2 = xtmp; }
    if (y>y2) { int ytmp = y; y = y2; y2 = ytmp; }
    if (u>u2) { float utmp = u; u = u2; u2 = utmp; }
    if (v>v2) { float vtmp = v; v = v2; v2 = vtmp; }

 //   float NUMX = ((float)(*Knot)[0].nodes())*(u2-u)/(*Knot)[0].length()/5.;
 //   float NUMY = ((float)(*Knot)[0].nodes())*(v2-v)/(*Knot)[0].length()/5.;

    float Nx = (u2-u)/(float)(x2-x);
    // float Ny = (v2-v)/(float)(y2-y);
    for (i=0;i<=(x2-x);i++) {
      a = u+(float)i*Nx;
      //for (j=0;j<=(y2-y);j++) {
      b = v; //+(float)j*Ny;
        circles->addChild(drawCircle((*Knot)[0].pointAt(a),
                                  (*Knot)[0].tangentAt(a),
                                  (*Knot)[0].pointAt(b)));
/*
      b = v+(float)(y2-y)*Ny;
        circles->addChild(drawCircle((*Knot)[0].pointAt(a),
                                  (*Knot)[0].tangentAt(a),
                                  (*Knot)[0].pointAt(b)));
*/
      //}
   }

/*
    for (j=0;j<=(y2-y);j++) {
      b = v+(float)j*Ny;
      a = u;
        circles->addChild(drawCircle((*Knot)[0].pointAt(a),
                                  (*Knot)[0].tangentAt(a),
                                  (*Knot)[0].pointAt(b)));
      a = u+(float)(x2-x)*Ny;
        circles->addChild(drawCircle((*Knot)[0].pointAt(a),
                                  (*Knot)[0].tangentAt(a),
                                  (*Knot)[0].pointAt(b)));
    }
*/

    // reset old values
    if (x<x2 && x>=0) { oldx = x; oldx2 = x2; }
    else { oldx = x2; oldx2 = x; }
    if (y<y2 && y>0) { oldy = y; oldy2 = y2; }
    else { oldy = y2; oldy2 = y; }
  }
  else {
    x = (*Knot)[0].biarcPos(u);//*(*Knot)[0].length());
    y = (*Knot)[0].biarcPos(v);//*(*Knot)[0].length());
    materials[0]->diffuseColor.set1Value(x,ColorTable[1]);
    materials[0]->diffuseColor.set1Value(y,ColorTable[1]);
    oldx = x; oldx2 = -1;
    oldy = y; oldy2 = -1;

    SoSeparator *sep = drawCircle((*Knot)[0].pointAt(u),//*(*Knot)[0].length()),
                                  (*Knot)[0].tangentAt(u),//*(*Knot)[0].length()),
                                  (*Knot)[0].pointAt(v));//*(*Knot)[0].length()));
    //cout << "Add child\n";
    if (circles->getNumChildren()==1)
      circles->replaceChild(0,sep);
    else {
      circles->removeAllChildren();
      circles->addChild(sep);
    }
  }
}

SoSeparator* drawCircle(Vector3 p0, Vector3 t0, Vector3 p1) {
  Vector3 d = (p1-p0)-t0*(p1-p0).dot(t0);
  d.normalize();
  SoSeparator *c_sep = new SoSeparator;
  SoCoordinate4 *coords = new SoCoordinate4;
  SoNurbsCurve *nurbs = new SoNurbsCurve;
  SoDrawStyle *ds = new SoDrawStyle;
  SoPointSet *ps = new SoPointSet;

  nurbs->numControlPoints.setValue(9);
  nurbs->knotVector.set1Value(0,0);
  nurbs->knotVector.set1Value(1,0);
  nurbs->knotVector.set1Value(2,0);
  nurbs->knotVector.set1Value(3,1);
  nurbs->knotVector.set1Value(4,1);
  nurbs->knotVector.set1Value(5,2);
  nurbs->knotVector.set1Value(6,2);
  nurbs->knotVector.set1Value(7,3);
  nurbs->knotVector.set1Value(8,3);
  nurbs->knotVector.set1Value(9,4);
  nurbs->knotVector.set1Value(10,4);
  nurbs->knotVector.set1Value(11,4);

  ds->pointSize.setValue(3);
  ps->numPoints.setValue(9);

  float R;
  Vector3 D = p1 - p0;
  float Dlen = D.norm();
  float cosa = (t0.dot(D))/Dlen;
  float sina = sqrt(1.0-cosa*cosa);

  if (sina==0.0) R=1e6;
  else R = 0.5*Dlen/sina;

  float w = cos(M_PI/180.*45.);
  SbVec4f v;

  v = SbVec4f(p0[0],p0[1],p0[2],1.);
  coords->point.set1Value(0,v);

  v = SbVec4f(p0[0]+R*t0[0],p0[1]+R*t0[1],p0[2]+R*t0[2],1);
  v*=w;
  coords->point.set1Value(1,v);

  v = SbVec4f(p0[0]+R*t0[0]+R*d[0],p0[1]+R*t0[1]+R*d[1],p0[2]+R*t0[2]+R*d[2],1);
  coords->point.set1Value(2,v);

  v = SbVec4f(p0[0]+R*t0[0]+2.*R*d[0],p0[1]+R*t0[1]+2.*R*d[1],p0[2]+R*t0[2]+2.*R*d[2],1);
  v*=w;
  coords->point.set1Value(3,v);

  v = SbVec4f(p0[0]+2.*R*d[0],p0[1]+2.*R*d[1],p0[2]+2.*R*d[2],1);
  coords->point.set1Value(4,v);

  v = SbVec4f(p0[0]-R*t0[0]+2.*R*d[0],p0[1]-R*t0[1]+2.*R*d[1],p0[2]-R*t0[2]+2.*R*d[2],1);
  v*=w;
  coords->point.set1Value(5,v);

  v = SbVec4f(p0[0]-R*t0[0]+R*d[0],p0[1]-R*t0[1]+R*d[1],p0[2]-R*t0[2]+R*d[2],1);
  coords->point.set1Value(6,v);

  v = SbVec4f(p0[0]-R*t0[0],p0[1]-R*t0[1],p0[2]-R*t0[2],1);
  v*=w;
  coords->point.set1Value(7,v);

  v = SbVec4f(p0[0],p0[1],p0[2],1.);
  coords->point.set1Value(8,v);

  c_sep->addChild(coords);
  c_sep->addChild(nurbs);
// FIXME : uncomment if u wanna see the control points of the Nurb
  // c_sep->addChild(ds);
  // c_sep->addChild(ps);
  
  return c_sep;
}

VVV *myViewer;

ImageIconProvider* iip = NULL;
Aux2DPlotWindow *pl_win = NULL;

int main(int argc, char **argv) {

  // Set parameters according to command line args

  if(!parse(argc,argv)) usage(argv[0]);

  // FIXME Center somhow the whole knot bundle ...
  //  Knot->center();
  // Knot->make_default();
  //Knot->normalize();

  // TODO : Put this in an init func!!!
  // Generate ColorTable
  ColorTable[0] = SbColor(1,0,0);
  ColorTable[1] = SbColor(0,1,0);
  ColorTable[2] = SbColor(0,0,1);
  ColorTable[3] = SbColor(1,1,0);
  ColorTable[4] = SbColor(1,0,1);
  ColorTable[5] = SbColor(0,1,1);
  ColorTable[6] = SbColor(.4,.4,.4);

  cout << "Generate mesh for all curves"<<flush;
  Knot->makeMesh(N,S,R,Tol);
  cout << "\t[OK]\n";

  cout << "Curves    : "<< Knot->tubes() << endl;
  for (int i=0;i<Knot->tubes();i++) {
    cout << "Curve " << i+1 << endl;
    cout << "Nodes     : "<< (*Knot)[i].nodes() << endl;
    cout << "Segments  : "<< (*Knot)[i].segments() << endl;
    cout << "Radius    : "<< (*Knot)[i].radius() << endl;
  }
  cout << "Tolerance : "<<Tol<<endl;
  if ((*Knot)[0].isClosed()) cout << "Closed curves!\n";

  myWindow = SoQt::init("KnotViewer");
  if (myWindow == NULL) exit(2);

  // init my class
  SoKnot::initClass();

  root = new SoSeparator; root->ref();
  circles = new SoSeparator; circles->ref();
  interaction = new SoSeparator; interaction->ref();
  scene = new SoSeparator; scene->ref();

  if (IV_SCENE) {
    SoInput iv_in;
    if (iv_in.openFile(iv_file)) {
      SoSeparator *external_iv = SoDB::readAll(&iv_in);
      if (external_iv) root->addChild(external_iv);
      iv_in.closeFile();
    }
    else {
      cerr << "[Warning] " << iv_file << " IV file Problem. Skipped\n";
    }
  }

  root->addChild(interaction);
  root->addChild(scene);

  // NEW
  root->addChild(circles);

  if (TEXTURES) {
    knot_texture = new SoTexture2;
    knot_texture->filename.setValue(texture_file);
    scene->addChild(knot_texture);
    cout << "Texture node added (" <<texture_file << ")\n";
  }

  cout << "Create " << Knot->tubes() << " SoKnot objects";
  knot_shape = new SoKnot*[Knot->tubes()];
  knot_node = new SoSeparator*[Knot->tubes()];
  materials = new SoMaterial*[Knot->tubes()];
  material_bindings = new SoMaterialBinding*[Knot->tubes()];

  // FIXME : we need an own SoSeparator for each SoKnot instance!!!
  for (int i=0;i<Knot->tubes();i++) {
    (*Knot)[i].make_default();
    knot_shape[i] = new SoKnot;
    knot_node[i] = new SoSeparator;
    knot_node[i]->ref();
    knot_shape[i]->setKnot( &((*Knot)[i]), (*Knot)[i].isClosed()?Tol:-1 );
    knot_shape[i]->radius.setValue(R);
    knot_shape[i]->segments.setValue(S);
    knot_shape[i]->nodes.setValue(N);
    knot_shape[i]->ref();

    materials[i] = new SoMaterial;
    for (int z=0;z<(*Knot)[i].nodes()-((*Knot)[i].isClosed()?0:1);z++) {
      materials[i]->diffuseColor.set1Value(z,ColorTable[i%ColorNum]);
      /*
      materials[i]->diffuseColor.set1Value(z,ColorTable[ColorIdx]);
      ColorIdx=((ColorIdx+1)%ColorNum);
      */
    }
    // Mark beginning of curve!
    materials[i]->diffuseColor.set1Value(0,ColorTable[1]);

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
    scene->addChild(knot_node[i]);
  }

  cout << "\t[OK]\n";

 // myViewer=new SoQtExaminerViewer(myWindow);
  myViewer = new VVV(myWindow,"viewer",TRUE,SoQtFullViewer::BUILD_ALL,SoQtViewer::BROWSER);

  // Create event handler for mouse
  SoEventCallback *mouseEvent = new SoEventCallback;
  mouseEvent->addEventCallback(SoMouseButtonEvent::getClassTypeId(), mousefunc, myViewer);// &appData);
  interaction->addChild(mouseEvent);
  
  // Create event handler for mouse motion
  SoEventCallback *motionEvent = new SoEventCallback;
  motionEvent->addEventCallback(SoLocation2Event::getClassTypeId(), motionfunc, myViewer);//&appData);
  root->addChild(motionEvent);

  myViewer->setSceneGraph(root);
  myViewer->setSize(SbVec2s(_SCREEN_W_,_SCREEN_H_));

  myViewer->setEventCallback(myAppEventHandler,myViewer);
  myViewer->setTransparencyType(SoGLRenderAction::SORTED_OBJECT_SORTED_TRIANGLE_BLEND);

  if (BackGroundFlag)
    myViewer->setBackgroundColor(SbColor(1,1,1));

  // myViewer->setFeedbackVisibility(TRUE);

  myViewer->setTitle("KnotViewer");
  myViewer->show();
  SoQt::setWidgetSize(myWindow,SbVec2s(_SCREEN_W_,_SCREEN_H_));
  SoQt::show(myWindow);

  if (PT_PLOT) {
    if (!pl_win) {
      pl_win = new Aux2DPlotWindow(NULL,"2dwindow",Qt::WNoAutoErase);
      pl_win->setCaption("2D Window");
      pl_win->setGeometry(_SCREEN_W_+8,0,200,200);
      iip = new ImageIconProvider;
      QFileDialog::setIconProvider(iip);
      QObject::connect(pl_win,SIGNAL(pos_changed(float,float,float,float)),
                       myViewer,SLOT(update_picked(float,float,float,float)));

    }
    if (pl_win->loadImage(ptplot_file)) {
      pl_win->repaint();
      if (pl_win->isShown()) pl_win->hide();
      else pl_win->show();
    }
    else  {
      pl_win->hide();
      cerr << "[Warning] Could not load " << ptplot_file << ". Skipped\n";
    }
  }

  SoQt::mainLoop();

}

void usage(char *prog) {
  cerr << "Usage : " << prog << " [OPTIONS] <PFK file>\n"
       << "Optional arguments\n"
       << "  -N=NODES\t\tnumber of points on the curve\n"
       << "  -S=SEGMENTS\t\tcross sectional segmentation\n" 
       << "  -R=RADIUS\t\ttube radius\n"
       << "  -Tol=TOLERANCE\tmesh adjustment tolerance\n"
       << "  -closed\t\tcloses all the curve\n"
       << "  -iv-scene <file.iv>\tadd a .iv scenegraph to the scene.\n"
       << "  -texture <img_file>\ttexture map the pkf curve\n"
       << "  -whitebg\t\twhite background color\n";
  exit(0);
}

int parse(int argc, char** argv) {

  int pkf_knots = 0;
  int pkf_idx = 0;
  N = 0;

  ResamplePartFlag = 0;
  FirstPoint = 0;

  // default values
  S = 12;
  R = 0.0083; // experimental knot value
  Tol = 1e-02;
  BackGroundFlag = 0;


  if (argc<2) return 0;

  // Parse Options
  // do we have an option as last argument where a pkf filename should go?
//  if (argv[argc-1][0]=='-') return 0;

  for (int i=1;i<argc;i++) {

    if (argv[i][0] == '-') {
      if (pkf_knots>0) return 0;
      if (!strncmp(&argv[i][1],"N=",2)) {
        N = atoi(&argv[i][3]);
      }
      else if (!strncmp(&argv[i][1],"S=",2))
        S = atoi(&argv[i][3]);
      else if (!strncmp(&argv[i][1],"R=",2))
        R = atof(&argv[i][3]);
      else if (!strncmp(&argv[i][1],"Tol=",4))
        Tol = atof(&argv[i][5]);
      else if (!strncmp(&argv[i][1],"closed",6))
        continue;
      else if (!strncmp(&argv[i][1],"texture",7)) {
        TEXTURES = 1;
        strcpy(texture_file,argv[i+1]); i++;
      }
      else if (!strncmp(&argv[i][1],"iv-scene",8)) {
        IV_SCENE = 1;
        strcpy(iv_file,argv[i+1]); i++;
      }
      else if (!strncmp(&argv[i][1],"whitebg",7)) {
        BackGroundFlag = 1;
      }
      else if (!strncmp(&argv[i][1],"ptplot",6)) {
        PT_PLOT = 1;
        ptplot_file = QString(argv[i+1]); i++;
        cout << "We got a pt plot : " << ptplot_file << endl;
      }
      else {
        cerr << "Unknown option : " << argv[i][1] << endl;
        return 0;
      }
    }
    else {
      if (pkf_knots==0) pkf_idx = i;
      pkf_knots++;
    }
  }

/*
  CurveBundle<Vector3> tmp_cb;
  CurveBundle<Vector3> cb;
*/
  if (pkf_knots+pkf_idx!=argc) return 0;

  // create knot from pkf
  // fill our TubeBundle with all the knots
  //  Knot = new TubeBundle<Vector3>(argv[argc-1]);
  Knot = new TubeBundle<Vector3> ;//(cb);
  for (int i=pkf_idx;i<argc;i++) {
    if (!Knot->readPKF(argv[i])) {
      cerr << "Aborted.\n";
      exit(-1);
    }
  }

  // closed curve requested?
  for (int i=1;i<argc;i++)
    if (!strncmp(&argv[i][1],"closed",6)) {
      for (int j=0;j<Knot->tubes();j++)
	(*Knot)[j].link();
    }
  
  // FIXME
  // We arbitrarily choose for N the number of datapoints of the first curve
  // It would be reasonnable to include the option to show exactly what
  // is in the data file
  if (!N)
    N = (*Knot)[0].nodes();

  return 1;
}

SbBool myAppEventHandler(void *userData, QEvent *anyevent) {

  //  SoQtRenderArea *myRenderArea = (SoQtRenderArea *) userData;
  QKeyEvent *myKeyEvent;
  //SbBool handled = TRUE;

  int tS, tN;
  //  SoQtViewer::DrawStyle DStyle;

  SoChildList *children = new SoChildList(scene);
  int child_len;
  Tube<Vector3>* bez_tub;

  if(anyevent->type()==QEvent::KeyPress) {

    children = scene->getChildren();
    child_len = children->getLength();

    myKeyEvent = (QKeyEvent *) anyevent;

    switch(myKeyEvent->key()) {

    // Transparency changing
    case Qt::Key_F:
      transp+=0.05;
      if (transp>1.0) transp = 1.0;
      for (int i=0;i<Knot->tubes();i++)
        materials[i]->transparency = transp;
      break;

    case Qt::Key_V:
      transp-=0.05;
      if (transp<0.0) transp = 0.0;
      for (int i=0;i<Knot->tubes();i++)
        materials[i]->transparency = transp;
      break;

    case Qt::Key_A:
      for (int i=0;i<Knot->tubes();i++)
	knot_shape[i]->radius = knot_shape[i]->radius.getValue()*1.1;
      cout << "Radius : " << knot_shape[0]->radius.getValue() << endl;
      break;

    case Qt::Key_Y:
    case Qt::Key_Z:
      for (int i=0;i<Knot->tubes();i++)
	knot_shape[i]->radius = knot_shape[i]->radius.getValue()*0.9;
      cout << "Radius : " << knot_shape[0]->radius.getValue() << endl;
      break;

    case Qt::Key_S:
      tS = knot_shape[0]->segments.getValue()+1;
      if (tS>120) {
	cerr << "Warning : more than 120 segments requested. Set to 120!\n";
	tS = 120;
      }
      for (int i=0;i<Knot->tubes();i++)
	knot_shape[i]->segments.setValue(tS);
      cout << "Segments : " << tS << endl;
      break;

    case Qt::Key_X:
      tS = knot_shape[0]->segments.getValue()-1;
      if (tS<3) {
	cerr << "Warning : less than 3 segments not allowed. Set to 3!\n";
	tS = 3;
      }
      for (int i=0;i<Knot->tubes();i++)
	knot_shape[i]->segments.setValue(tS);
      cout << "Segments : " << tS << endl;
      break;

    case Qt::Key_D:
      tN = knot_shape[0]->nodes.getValue()+10;
      for (int i=0;i<Knot->tubes();i++) {
	knot_shape[i]->nodes.setValue(tN);
	if (view_mode==BIARC_VIEW)
	  children->remove(0);
      }

      if (view_mode==BIARC_VIEW) {
	for (int i=0;i<Knot->tubes();i++) {
	  bez_tub = knot_shape[i]->getKnot();
	  bez_tub->make_default();
	  bez_tub->resample(tN);
	  bez_tub->make_default();
	  addBezierCurve(scene,bez_tub);
	}
      }

      cout << "Nodes : " << tN << endl;
      break;

    case Qt::Key_C:
      tN = knot_shape[0]->nodes.getValue()-10;
      if (tN<3) {
	cerr << "Warning : less than 3 nodes requested. Set to 3!\n";
	tN=3;
      }
      for (int i=0;i<Knot->tubes();i++) {
	knot_shape[i]->nodes.setValue(tN);
	if (view_mode==BIARC_VIEW)
	  children->remove(0);
      }

      if (view_mode==BIARC_VIEW) {
	for (int i=0;i<Knot->tubes();i++) {
	  bez_tub = knot_shape[i]->getKnot();
	  bez_tub->make_default();
	  bez_tub->resample(tN);
	  bez_tub->make_default();
	  addBezierCurve(scene,bez_tub);
	}
      }

      cout << "Nodes : " << tN << endl;
      break;

    case Qt::Key_Space:
      swap_view();
      switch(view_mode) {
      case SOLID_VIEW: 

	for (int i=0;i<child_len;i++) children->remove(0);
	for (int i=0;i<Knot->tubes();i++)
	  scene->addChild(knot_node[i]);

	myViewer->setDrawStyle(SoQtViewer::STILL,
			       SoQtViewer::VIEW_AS_IS);

	break;
      case WIRE_VIEW: myViewer->setDrawStyle(SoQtViewer::STILL,
					     SoQtViewer::VIEW_HIDDEN_LINE);
	break;
      case BIARC_VIEW:
	myViewer->setDrawStyle(SoQtViewer::STILL,
			       SoQtViewer::VIEW_AS_IS);

	for (int i=0;i<child_len;i++) children->remove(0);
	for (int i=0;i<Knot->tubes();i++) {
//cerr << "knot " << i << ":\n"<<flush;
	  bez_tub = knot_shape[i]->getKnot();
//cerr << *bez_tub <<endl;
//cerr << "get ok\n" << flush;
	  bez_tub->make_default();
//cerr << "make_def ok\n" << flush;
	  addBezierCurve(scene,bez_tub);
//cerr << "add ok\n" << flush;
	}

	break;
      default: cerr << "View Mode problem. Should not happen\n"; exit(3);
      }
      break;

    //
    case Qt::Key_W:
      knot_shape[0]->getKnot()->writePKF("curve.pkf");
      cout << "Current curve state written to curve.pkf.\n";
      break;

      // TODO TODO
    case Qt::Key_E:
      //knot_shape[0]->getKnot()->exportRIBFile("knot.rib");
      cout << "[Not Implemented] Current curve is exported to a Renderman RIB file knot.rib.\n";
      break;

    /* Start "resample curve between two points" procedure
       This is only working in BIARC_VIEW
    */
    case Qt::Key_R:
      if (view_mode == BIARC_VIEW) {
        ResamplePartFlag = 1;
        cout << "Resample a part of the curve!\n";
      }
      break;

    case Qt::Key_O:
      //knot_shape[0]->getKnot()->exportPOVFile("knot.pov");
      cout << "[Not Implemented] Current curve is exported to a Povray file knot.pov.\n";
      break;
      
    case Qt::Key_P:
      if (!pl_win) {
        pl_win = new Aux2DPlotWindow(NULL,"2dwindow",Qt::WNoAutoErase);
        pl_win->setCaption("2D Window");
        pl_win->setGeometry(_SCREEN_W_+8,0,200,200);
        iip = new ImageIconProvider;
        QFileDialog::setIconProvider(iip);
        QObject::connect(pl_win,SIGNAL(pos_changed(float,float,float,float)),
                         myViewer,SLOT(update_picked(float,float,float,float)));

      }
      if (pl_win->isShown()) pl_win->hide();
      else pl_win->show();    
      break;

    // Quit program
    case Qt::Key_Q:
      exit(0);

    }

    return TRUE;

  }
  return FALSE;
}

//Mouse motion callback
static void motionfunc(void *data, SoEventCallback *eventCB) {

  if (view_mode==BIARC_VIEW && PRESSED) {
    
    const SoMouseButtonEvent *mbe=(SoMouseButtonEvent* )eventCB->getEvent();
    VVV *viewer = (VVV*)data;
    
    if (EditTangent) {
      SbVec3f loc = spp.project(mbe->getNormalizedPosition(viewer->getViewportRegion())) + delta;
      loc += (LeftVector*(AspectratioX-1.f)*(loc.dot(LeftVector))
               + UpVector*(AspectratioY-1.f)*(loc.dot(UpVector))) ;
      Vector3 editT((float*)&loc[0]);
      picked_biarc->setTangent(editT-picked_biarc->getPoint());
    }
    else {
      SbVec3f loc = spp.project(mbe->getNormalizedPosition(viewer->getViewportRegion())) + delta;
    
      loc += (LeftVector*(AspectratioX-1.f)*(loc.dot(LeftVector))
               + UpVector*(AspectratioY-1.f)*(loc.dot(UpVector))) ;

      picked_biarc->setPoint(Vector3((float*)&loc[0]));
    }

    SoChildList *children = new SoChildList(scene);
    Tube<Vector3>* bez_tub;
    children = scene->getChildren();
    
    for (int i=0;i<Knot->tubes();i++)
      children->remove(0);
    
    for (int i=0;i<Knot->tubes();i++) {
      bez_tub = knot_shape[i]->getKnot();
      bez_tub->make_default();
      addBezierCurve(scene,bez_tub);
    }
  }
}

//Mouse callback
static void mousefunc(void *data, SoEventCallback *eventCB) {

  if (view_mode!=BIARC_VIEW) return;

  VVV *viewer = (VVV*)data;
  Tube<Vector3>* bez_tub;
  const SoMouseButtonEvent *mbe=(SoMouseButtonEvent*)eventCB->getEvent();
  
  //Handle point grabbing
  if(mbe->getButton() == SoMouseButtonEvent::BUTTON1 && mbe->getState() == SoButtonEvent::DOWN) {
    
    SoRayPickAction rp(viewer->getViewportRegion());
    rp.setPoint(mbe->getPosition());
    rp.apply(viewer->getSceneManager()->getSceneGraph());
    
    SoPickedPoint *point = rp.getPickedPoint();

    EditTangent = 0;
    if (point) {

      SoPath *path = point->getPath();
      SoNode *node = path->getTail();

      if(node && node->getTypeId()==SoSphere::getClassTypeId()) {

        Vector3 pp(point->getPoint()[0],
                   point->getPoint()[1],
 	           point->getPoint()[2]);
    
        Vector3 cp;
        int FOUND = 0;

        // FIXME: If we have more than one curve, give this as userData to the callback
        vector<Biarc<Vector3> >::iterator current;
        for (int l=0;l<Knot->tubes();l++) {
          current = knot_shape[l]->getKnot()->begin();
          float Tolerance = (current->getPoint()-current->getNext().getPoint()).norm()/4.0;
          while (current!=knot_shape[l]->getKnot()->end()) {
  	    if ((current->getPoint()-pp).norm()<Tolerance) {
	      cp = current->getPoint();
  	      picked_biarc = current;
	      FOUND = 1;
	      break;
  	    }
	    ++current;
          }
        }

        if (FOUND && ResamplePartFlag) {
          if (!FirstPoint) {
            FirstBiarc = picked_biarc;
            FirstPoint = 1;
          }
           else {
             // XXX only single components!
             bez_tub = knot_shape[0]->getKnot();
             bez_tub->make_default();
             // XXX resample from FirstBiarc to picked_biarc with 10 points
             bez_tub->refine(FirstBiarc,picked_biarc,5);
             bez_tub->make_default();
             for (int i=0;i<scene->getChildren()->getLength();i++)
               scene->getChildren()->remove(0);
             addBezierCurve(scene,bez_tub);
             FirstPoint = 0;
             ResamplePartFlag = 0;
           }
        }
        else if (FOUND) {
          SbViewVolume vv = viewer->getCamera()->getViewVolume();
          SbPlane s(-(vv.getProjectionDirection()), point->getPoint());
          spp = SbPlaneProjector(s);
          spp.setViewVolume(vv);

          // up and left vector according to the current camera position
          SbRotation rot = viewer->getCamera()->orientation.getValue();
          rot.multVec(SbVec3f(1,0,0),LeftVector);
          rot.multVec(SbVec3f(0,1,0),UpVector);

          SbVec2s winsize = viewer->getViewportRegion().getWindowSize();
          AspectratioX = (float)winsize[0]/(float)winsize[1];
          AspectratioY = 1;

          if (AspectratioX < 1) {
            AspectratioY = (float)winsize[1]/(float)winsize[0];
            AspectratioX = 1;
          }

          delta = SbVec3f((float*)&cp[0]) - point->getPoint();

          // If the viewport is not square we have a
          // aspect ratio problem. Meaning, that the vector
          // is actually smaller!
          delta -= (LeftVector*(1.f-1.f/AspectratioX)*(delta.dot(LeftVector))
                     + UpVector*(1.f-1.f/AspectratioY)*(delta.dot(UpVector))) ;

          PRESSED = 1;
        }
      } // Sphere end
      else
        if(node && node->getTypeId()==SoLineSet::getClassTypeId()) {
          SoLineSet *ls = (SoLineSet*)node;
          if (ls->getName() != "datatangents") {
            eventCB->setHandled();
            return;
          }

          EditTangent = 1;

          int sl_idx = ((SoLineDetail*)point->getDetail())->getPoint0()->getCoordinateIndex();
          // XXX only single component
          picked_biarc = (knot_shape[0]->getKnot()->begin()+(sl_idx>>1));
          
          // XXX same code as above!!!
          SbViewVolume vv = viewer->getCamera()->getViewVolume();
          SbPlane s(-(vv.getProjectionDirection()), point->getPoint());
          spp = SbPlaneProjector(s);
          spp.setViewVolume(vv);

          // up and left vector according to the current camera position
          SbRotation rot = viewer->getCamera()->orientation.getValue();
          rot.multVec(SbVec3f(1,0,0),LeftVector);
          rot.multVec(SbVec3f(0,1,0),UpVector);

          SbVec2s winsize = viewer->getViewportRegion().getWindowSize();
          AspectratioX = (float)winsize[0]/(float)winsize[1];
          AspectratioY = 1;

          if (AspectratioX < 1) {
            AspectratioY = (float)winsize[1]/(float)winsize[0];
            AspectratioX = 1;
          }

          delta = SbVec3f(0,0,0);
          PRESSED = 1;

        } // LineSet end
      else PRESSED = 0;
    } // point end
    else PRESSED = 0;

    eventCB->setHandled();
  }

  //Handle ungrabbing
  if(mbe->getButton() == SoMouseButtonEvent::BUTTON1 && mbe->getState() == SoButtonEvent::UP) {
    if (PRESSED) {

      SoChildList *children = new SoChildList(scene);
      Tube<Vector3>* bez_tub;
      children = scene->getChildren();

      for (int i=0;i<Knot->tubes();i++)
        children->remove(0);
       
      for (int i=0;i<Knot->tubes();i++) {
        bez_tub = knot_shape[i]->getKnot();
        bez_tub->make_default();
        addBezierCurve(scene,bez_tub);
      }
    }
       
    PRESSED = 0;
    eventCB->setHandled();
  }
}


void addBezierCurve(SoSeparator *root, Tube<Vector3>* t) {

  // OpenInventor part
  // Midpoints

  int NODES = t->nodes()-1;
  if (t->isClosed()) NODES++;

  SoSeparator *main = new SoSeparator;
  main->ref();

  SoSeparator *midpoints         = new SoSeparator;            // Root
  SoTranslation **mid_trans      = new SoTranslation*[NODES];//t->nodes()-1];
  SoSeparator **sub_midpoints    = new SoSeparator*[NODES];//t->nodes()-1];
  SoMaterial *blue               = new SoMaterial;
  midpoints->ref();

  // Datapoints
  SoSeparator *datapoints         = new SoSeparator;            // Root
  SoTranslation **data_trans      = new SoTranslation*[t->nodes()];
  SoSeparator **sub_datapoints     = new SoSeparator*[t->nodes()];
  SoMaterial *red                 = new SoMaterial;
  datapoints->ref();

  SoSphere *sphere = new SoSphere;
  sphere->ref();
  float Rad = ((*t)[0].getMidPoint()-(*t)[0].getPoint()).norm()/5.0;
  sphere->radius = Rad;

  for (int i=0;i<t->nodes();i++) {
    if (i!=t->nodes()-1 || t->isClosed())
      sub_midpoints[i] = new SoSeparator;
    sub_datapoints[i] = new SoSeparator;
    data_trans[i] = new SoTranslation;
    if (i!=t->nodes()-1 || t->isClosed())
      mid_trans[i] = new SoTranslation;

    if (i!=t->nodes()-1 || t->isClosed())
      sub_midpoints[i]->ref();
    sub_datapoints[i]->ref();
    data_trans[i]->ref();
    if (i!=t->nodes()-1 || t->isClosed())
      mid_trans[i]->ref();
  }

  SoComplexity  *complexity = new SoComplexity;
  complexity->value = 0.1;

  datapoints->addChild(complexity);
  midpoints->addChild(complexity);

  red->diffuseColor.setValue(1,0,0);
  blue->diffuseColor.setValue(0,0,1);
  datapoints->addChild(red);
  midpoints->addChild(blue);

  for (int j=0;j<t->nodes();j++) {
    Vector3 p = (*t)[j].getPoint();
    data_trans[j]->translation.setValue((float *)&p);
    if (j!=t->nodes()-1 || t->isClosed()) {
      Vector3 m = (*t)[j].getMidPoint();
      mid_trans[j]->translation.setValue((float *)&m);
      sub_midpoints[j]->addChild(mid_trans[j]);
      sub_midpoints[j]->addChild(sphere);
    }

    sub_datapoints[j]->addChild(data_trans[j]);
    sub_datapoints[j]->addChild(sphere);

    if (j!=t->nodes()-1 || t->isClosed())
      midpoints->addChild(sub_midpoints[j]);
    datapoints->addChild(sub_datapoints[j]);

  }
  
  SoMaterial *white = new SoMaterial;
  white->diffuseColor.setValue(1,1,1);

  main->addChild(datapoints);
  main->addChild(midpoints);
  main->addChild(white);
  //
  // Tangents Datapoints
  //

  SoLineSet *linesetData        = new SoLineSet;
  linesetData->setName(SbName("datatangents"));
  SoCoordinate3 *DataTanCoords  = new SoCoordinate3; 
  SoSeparator *DataTangentsNode = new SoSeparator;

  DataTangentsNode->ref();
  
  // Add _v.size() times 2 points for each line segment
  for (int i=0;i<t->nodes();i++)
    linesetData->numVertices.set1Value(i,2);
  
  // Add all the necessary coordinates
  // i.e : vertex point plus/minus Stream direction
  // where the length of each line is scaled by the biggest norm of _Stream
  // such that this _Stream vector is exactly the distance between 2 neighbour
  // points on the sphere
  
  // Distance between 2 neighbour points
  float TanLength = ((*t)[0].getMidPoint()-(*t)[0].getPoint()).norm()*.8;

  for (int i=0;i<t->nodes();i++) {
    Vector3 point    = (*t)[i].getPoint();
    Vector3 endpoint = point + (*t)[i].getTangent()*TanLength;
    
    // 2 coord points for the tangent line
    DataTanCoords->point.set1Value(i*2+0,(float *)&point);
    DataTanCoords->point.set1Value(i*2+1,(float *)&endpoint);
  }

  SoMaterial *yellow = new SoMaterial;
  yellow->diffuseColor.setValue(1,1,0);
  DataTangentsNode->addChild(yellow);
  DataTangentsNode->addChild(DataTanCoords);
  DataTangentsNode->addChild(linesetData);

  main->addChild(DataTangentsNode);

  //

  //
  // Tangents Datapoints
  //

  SoLineSet *linesetMid        = new SoLineSet;
  linesetMid->setName(SbName("midtangents"));
  SoCoordinate3 *MidTanCoords  = new SoCoordinate3; 
  SoSeparator *MidTangentsNode = new SoSeparator;

  MidTangentsNode->ref();
  
  // Add _v.size() times 2 points for each line segment
  for (int i=0;i<NODES;i++)
    linesetMid->numVertices.set1Value(i,2);
  
  // Add all the necessary coordinates
  // i.e : vertex point plus/minus Stream direction
  // where the length of each line is scaled by the biggest norm of _Stream
  // such that this _Stream vector is exactly the distance between 2 neighbour
  // points on the sphere
  
  // Distance between 2 neighbour points

  for (int i=0;i<NODES;i++) {
    Vector3 point    = (*t)[i].getMidPoint();
    Vector3 endpoint = point + (*t)[i].getMidTangent()*TanLength;
    
    // 2 coord points for the tangent line
    MidTanCoords->point.set1Value(i*2+0,(float *)&point);
    MidTanCoords->point.set1Value(i*2+1,(float *)&endpoint);
  }

  SoMaterial *green = new SoMaterial;
  green->diffuseColor.setValue(0,1,0);
  MidTangentsNode->addChild(green);
  MidTangentsNode->addChild(MidTanCoords);
  MidTangentsNode->addChild(linesetMid);

  main->addChild(MidTangentsNode);

  //

  SoSeparator *CurveSep = new SoSeparator;
  CurveSep->ref();
  SoCoordinate4 *CurvePts = new SoCoordinate4;
  SoNurbsCurve  *Curve    = new SoNurbsCurve;

  int Order = 3;// or 2?
  int Kknots = 4*NODES+1; // (t->nodes()-1)+1;
  if (Kknots<=Order) Kknots = 3;

  int KLength = (Order + Kknots);
  Curve->numControlPoints = Kknots;

  float KSequence[KLength];
  KSequence[0] = KSequence[1] = KSequence[2] = 0;
  KSequence[KLength-1] = KSequence[KLength-2] = KSequence[KLength-3] = 1;  
  if (Kknots==0|KLength==0) {cout<<"KnotVector empty\n";return;}
  int ctrlpts = (Kknots-1)/2;   // Number of Weighted Control Points
  int counter = 1;
  if (ctrlpts==1) return;
  for (int i=3;i<(KLength-3);i++) {
    KSequence[i++] = float(counter)/float(ctrlpts);
    KSequence[i]   = float(counter)/float(ctrlpts);
    counter++;
  }

  Curve->knotVector.setValues(0,KLength,KSequence);//KV.getSequence());

  assert(t->begin()->isBiarc());
  float o;
  Vector3 q0,mid0,q1,mid1,dd1,dd2;
  for (int i=0;i<NODES;i++) {//t->nodes()-1;i++) {
    (*t)[i].getBezier(q0,mid0,dd1,q1,mid1,dd2);
    CurvePts->point.set1Value(4*i+0,q0[0],q0[1],q0[2],1.0);
    o = ((mid0-q0).dot((*t)[i].getTangent()))/sqrt((mid0-q0).dot(mid0-q0));
    CurvePts->point.set1Value(4*i+1,mid0[0],mid0[1],mid0[2],o);
    CurvePts->point.set1Value(4*i+2,q1[0],q1[1],q1[2],1.0);
    o = ((mid1-q1).dot((*t)[i].getMidTangent()))/sqrt((mid1-q1).dot(mid1-q1));
    CurvePts->point.set1Value(4*i+3,mid1[0],mid1[1],mid1[2],o);
  }
  Vector3 gaga;
  if (t->isClosed())
    gaga = (*t)[0].getPoint();
  else
    gaga = (*t)[t->nodes()-1].getPoint();
  CurvePts->point.set1Value(Kknots-1,gaga[0],gaga[1],gaga[2],1.0);

  CurveSep->addChild(CurvePts);
  CurveSep->addChild(Curve);
  CurveSep->unrefNoDelete();
  main->addChild(CurveSep);

  root->addChild(main);

}

#endif // DOXYGEN_SHOULD_SKIP_THIS
