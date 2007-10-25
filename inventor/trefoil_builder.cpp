/*!
  \file merge_all.cpp
  \ingroup InventorGroup
  \brief Supposed to become the unique viewer combining
         the now separated multiview, skeleton open/closed etc

*/

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include "main.h"
#include <qlayout.h>
#include <qpainter.h>
#include <qlabel.h>
#include <qgroupbox.h>
#include <qslider.h>

SbBool myAppEventHandler(void *userData, QEvent *anyevent);

static void motionfunc(void *data, SoEventCallback *eventCB);
static void mousefunc(void *data, SoEventCallback *eventCB);

bool PRESSED = 0;
Vector3 PickedCurvePoint;
SbVec2f PRESSED_AT;
Biarc<Vector3>* picked_biarc = NULL;
SbVec3f EXACT_PICKED_POINT;
SbPlaneProjector *MAIN_PROJ;

// background
unsigned int BackGroundFlag;

void addBezierCurve(SoSeparator *root, Tube<Vector3>* t);

#define _SCREEN_W_ 800
#define _SCREEN_H_ 600

QWidget *myWindow;
//SoQtExaminerViewer *myViewer;
SoSeparator *root, *interaction, *scene;
SoSeparator *circles;

void usage(char* prog);
int parse(int argc, char** argv);
void trefoil();

QPixmap* pt_plot;
QPixmap* curvature_plot;
QLabel *lab1,*lab2;

// Number of nodes, number of circular segments
int N, S;
// Tube radius, mesh correction tolerance
float R, Tol;
char filename[50];

SoSeparator **knot_node;
SoKnot **knot_shape;
TubeBundle<Vector3> *Knot;
float rad,sweep_angle,TS,ang;
int nodes;

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
//static uint PT_PLOT = 0;
char iv_file[100];
QString ptplot_file;

enum VIEW_MODE { SOLID_VIEW = 1, WIRE_VIEW, BIARC_VIEW };
static VIEW_MODE view_mode = SOLID_VIEW;

void generate_pt() {
  cout  << "Update pt plot" << flush;
  lab1->setText(QString("Please wait"));
  lab1->update();
  knot_shape[0]->getKnot()->writePKF("/tmp/curve.pkf");
  if(system("/home/lcvm1/carlen/work/plot_pt.py /tmp/curve.pkf /tmp/out.png 300 >/dev/null")>=0) {
    system("convert /tmp/out.png -crop 350x600-50-300 /tmp/pt.png");
    pt_plot->load(QString("/tmp/pt.png"));
    lab1->setPixmap(*pt_plot);
    lab1->update();
  } 
  else
    cout << "Warning : Hardcoded plot_pt.py path! Change path\n";
  cout << "\t[Ok]\n";
}

void generate_curv() {
  cout  << "Update curvature plot" <<flush;
  knot_shape[0]->getKnot()->writePKF("/tmp/curve.pkf");
  system("curvature /tmp/curve.pkf > /tmp/k");
  system("gnuplot $LIBBIARC/inventor/trefoil_curv_plot.sh");
  system("convert /tmp/curv.png -resize 400x300 /tmp/curv.png");
  curvature_plot->load(QString("/tmp/curv.png"));
  lab2->setPixmap(*curvature_plot);
  lab2->update();
  cout << "\t[Ok]\n";
}

void swap_view() {
  switch(view_mode) {
  case SOLID_VIEW: view_mode = WIRE_VIEW; break;
  case WIRE_VIEW: view_mode = BIARC_VIEW; break;
  case BIARC_VIEW: view_mode = SOLID_VIEW; break;
  default: view_mode = SOLID_VIEW; break;
  }
}

void pointat_circle(float rad, float s, Vector3& p, Vector3& t) {
  // s in [0,1]
  p = Vector3(rad*sin(2.*M_PI*s), rad*cos(2.*M_PI*s),0.);
  t = Vector3(cos(2.*M_PI*s),-sin(2.*M_PI*s),0.);
  t.normalize();
}

Curve<Vector3>* gen_pcircle(float rad, int nodes,float offset) {
  Curve<Vector3>* circle = new Curve<Vector3>;
  Vector3 p,t;
  circle->header("Partial Circle","libbiarc","","");

  for (int i=0;i<nodes;i++) {
    pointat_circle(rad, offset/2./M_PI+(float)i*(sweep_angle/2./M_PI)/(float)(nodes-1),p,t);
    circle->append(p,t);
  }

  return circle;
}

#include "trefoil_builder.h"
#define RADSTART 1.50
#define SWEEPSTART 3.2*M_PI/3.
#define TSSTART 2.0
#define ANGSTART M_PI/6.

void ControlPanel::updateRad(int Val) {
  rad = RADSTART + (float)Val*1./100.;
  trefoil();
  slidertext[0]->setText(QString("Ear Radius (")+
                         QString().setNum(rad)+QString(")"));
} 

void ControlPanel::updateSweep(int Val) {
  sweep_angle = SWEEPSTART + (float)Val*2./100.*M_PI/3.;
  trefoil();
  slidertext[1]->setText(QString("Sweep Angle (")+
                         QString().setNum(sweep_angle)+QString(")"));
} 

void ControlPanel::updateTS(int Val) {
  TS = (TSSTART + (float)Val*5./100.);
  trefoil();
  slidertext[2]->setText(QString("Triangle Size (")+
                         QString().setNum(TS)+QString(")"));
} 

void ControlPanel::updateAng(int Val) {
  ang = ANGSTART + (float)Val/100.*M_PI;
  trefoil();
  slidertext[3]->setText(QString("Out of plane rot (")+
                         QString().setNum(ang)+QString(")"));
} 

ControlPanel::ControlPanel(QWidget *parent, const char *name,int wFlags)
  : QGroupBox(10,Qt::Vertical,QString("Parameters"),parent) {
    slidertext[0] = new QLabel(QString("Ear Radius (")+
			       QString().setNum(rad)+QString(")"),
			       this);
    sliders[0] = new QSlider(0,100,1,0,Qt::Horizontal,this);
 //   sliders[0]->setTracking(FALSE);
    connect(sliders[0],SIGNAL(valueChanged(int)),this,
            SLOT(updateRad(int)));
    slidertext[1] = new QLabel(QString("Sweep Angle (")+
			       QString().setNum(sweep_angle)+QString(")"),
			       this);
    sliders[1] = new QSlider(Qt::Horizontal,this);
    connect(sliders[1],SIGNAL(valueChanged(int)),this,
            SLOT(updateSweep(int)));
    slidertext[2] = new QLabel(QString("Triangle Size (")+
			       QString().setNum(TS)+QString(")"),
			       this);
    sliders[2] = new QSlider(Qt::Horizontal,this);
    connect(sliders[2],SIGNAL(valueChanged(int)),this,
            SLOT(updateTS(int)));
    slidertext[3] = new QLabel(QString("Out of plane rot (")+
			       QString().setNum(ang)+QString(")"),
			       this);
    sliders[3] = new QSlider(Qt::Horizontal,this);
    connect(sliders[3],SIGNAL(valueChanged(int)),this,
            SLOT(updateAng(int)));
    slidertext[4] = new QLabel(QString("vacat"),this);
    sliders[4] = new QSlider(Qt::Horizontal,this);
    sliders[4]->setDisabled(TRUE);

}
ControlPanel::~ControlPanel() {}

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
return;
}

SoSeparator* drawCircle(Vector3 p0, Vector3 t0, Vector3 p1) {
  return (new SoSeparator);
}

VVV *myViewer;

void trefoil() {

  Curve<Vector3> *c1,*c2,*c3;

  // generate 3 parts of circles
  c1 = gen_pcircle(rad,nodes/3,2.*M_PI-sweep_angle/2.);
  c2 = gen_pcircle(rad,nodes/3,2.*M_PI-sweep_angle/2.+2.*M_PI/3.);
  c3 = gen_pcircle(rad,nodes/3,2.*M_PI-sweep_angle/2.-2.*M_PI/3.);

  // out of plane rotation
  c1->rotAroundAxis(ang,Vector3(0,1,0));
  c2->rotAroundAxis(ang,Vector3(sin(2.*M_PI/3.),cos(2.*M_PI/3.),0));
  c3->rotAroundAxis(ang,Vector3(-sin(2.*M_PI/3.),cos(2.*M_PI/3.),0));

  // translate the 3 curves
  (*c1)+=(Vector3(0,cos(M_PI/3.),0)*TS);
  (*c2)+=(Vector3(sin(2.*M_PI/3.),cos(2.*M_PI/3.),0)*cos(M_PI/3.)*TS);
  (*c3)+=(Vector3(-sin(2.*M_PI/3.),cos(2.*M_PI/3.),0)*cos(M_PI/3.)*TS);

  Curve<Vector3> cc;
  for (int i=1;i<c1->nodes()-1;i++)
    cc.append(*(*c1)[i]);

  cc.append(*(c1->listTail()));
  cc.append(*(c3->listHead()));

  for (int i=1;i<c3->nodes()-1;i++)
    cc.append(*(*c3)[i]);

  cc.append(*(c3->listTail()));
  cc.append(*(c2->listHead()));

  for (int i=1;i<c2->nodes()-1;i++)
    cc.append(*(*c2)[i]);

  cc.append(*(c2->listTail()));
  cc.append(*(c1->listHead()));

  cc.link();
  cc.make_default();
  cc.resample(nodes);
  cc.make_default();
  cc.link();
  cc.normalize();

  CurveBundle<Vector3> *cb = new CurveBundle<Vector3>;
  cb->newCurve(cc);
  if (Knot) {
    TubeBundle<Vector3>* oldcb = Knot;
    Knot = new TubeBundle<Vector3>(*cb);
    Knot->makeMesh(N,S,R,Tol);
    delete oldcb;
    knot_shape[0]->setKnot( &((*Knot)[0]), (*Knot)[0].isClosed()?Tol:-1 );
  }
  else
    Knot = new TubeBundle<Vector3>(*cb);
}


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
    for (int z=0;z<(*Knot)[i].nodes();z++) {
      materials[i]->diffuseColor.set1Value(z,ColorTable[i%ColorNum]);
      // materials[i]->diffuseColor.set1Value(z,ColorTable[ColorIdx]);
      ColorIdx=((ColorIdx+1)%ColorNum);
    }
    materials[i]->transparency = transp;

    if (1) {
      // Material bundle
      material_bindings[i] = new SoMaterialBinding;
      material_bindings[i]->value = SoMaterialBinding::PER_PART;
      knot_node[i]->addChild(material_bindings[i]);
    }
    knot_node[i]->addChild(materials[i]);
    //addBezierCurve(root,&(*Knot)[i]);
    knot_node[i]->addChild(knot_shape[i]);
    scene->addChild(knot_node[i]);
  }

  cout << "\t[OK]\n";

  QBoxLayout* vl = new QVBoxLayout(myWindow,1,4);
  QBoxLayout *hl0 = new QHBoxLayout(vl,4);

  myViewer = new VVV(myWindow,"viewer",TRUE,SoQtFullViewer::BUILD_NONE,SoQtViewer::BROWSER);
  hl0->addWidget(myViewer->getWidget());
  ControlPanel *control_panel = new ControlPanel(myWindow);
  hl0->addWidget(control_panel);
 // control_panel->resize(200,myViewer->getWidget()->height());

  QBoxLayout *hl = new QHBoxLayout(vl,4);
  lab1 = new QLabel(myWindow);
  lab2 = new QLabel(myWindow);

  pt_plot = new QPixmap;//(QString("/tmp/pt.png"));
  curvature_plot = new QPixmap;//(QString("/tmp/curv.png"));
  generate_pt();
  generate_curv();

  lab1->show();
  lab2->show();
  hl->addWidget(lab1);
  hl->addWidget(lab2);

  myViewer->getWidget()->setMinimumHeight(1.5*(float)pt_plot->height());
  myViewer->getWidget()->setMinimumWidth(1.5*(float)pt_plot->height());
/*
  QBoxLayout* vl2 = new QVBoxLayout(topLayout);
  vl2->addWidget(new QLabel(myWindow));
  vl2->addWidget(new QLabel(myWindow));
  vl2->addWidget(new QLabel(myWindow));
*/
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

  // layout.addWidget(myViewer->getNormalWidget());
  // layout.addWidget(bla);
  // bla->show();

//  SoQt::setWidgetSize(myWindow,SbVec2s(_SCREEN_W_,_SCREEN_H_));
  SoQt::show(myWindow);

  SoQt::mainLoop();

  system("rm -f /tmp/{curve.pkf,curv.png,pt.png,a.png,out.png,k}");

}

void usage(char *prog) {
  exit(0);
}

int parse(int argc, char** argv) {

  N = 0;

  S = 12;
  R = 0.0083; // experimental knot value
  Tol = 1e-02;
  BackGroundFlag = 0;


  // Construct the first trefoil ...
  rad = RADSTART;
  nodes = 160;
  sweep_angle = SWEEPSTART;//3.5*M_PI/3.;
  TS = TSSTART;//2;
  ang = ANGSTART;//M_PI/6.;
  trefoil();

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
    case Qt::Key_2:
      generate_curv();
      break;
    case Qt::Key_1:
      generate_pt();
      break;
      // TODO TODO
    case Qt::Key_E:
      //knot_shape[0]->getKnot()->exportRIBFile("knot.rib");
      cout << "[Not Implemented] Current curve is exported to a Renderman RIB file knot.rib.\n";
      break;

    case Qt::Key_R:
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
    //AllData *appData=(AllData*)data;
//    SoQtExaminerViewer *viewer = (SoQtExaminerViewer*)data;
    VVV *viewer = (VVV*)data;
    // Scene *scene=appData->scene;
    
    // if(!scene->Grabbed())
    // {
    //    return;
    // }
    
    SbVec2f pos=mbe->getNormalizedPosition(viewer->getViewportRegion());
    
    //SbVec2f dif = pos - PRESSED_AT;

    cout << "x = " << pos[0] << ", y = "
    	 << pos[1] << endl;
    // scene->UpdateGrab(pos);
    //cout << "change PickedCurvePoint " << PickedCurvePoint
    //	 << " to " << (PickedCurvePoint+Vector3(dif[0],dif[1],0.0)) << endl;
  

    // FIXME : The dif Vector should be in the Observation plane!!!
    // right now we can only translate the points in the x,y plane!
    // Since the original curve is resampled, we'll find after every resampling
    // the initial curve shape, and not BIARC_VIEW modified shape!!!
    // HOWTO prevent resampling from error accumulation and reshaping the
    // curve at the same time!!!
    // implement an export to PKF routine!
    //cout << picked_biarc << endl;


      //It's a sphere
      //Set the projection plane
    //SbVec3f PICKED_POINT((float *)&PRESSED_AT[0]);//PickedCurvePoint[0]);

    //SbPlane plane(-viewer->getCamera()->getViewVolume().getProjectionDirection(),PICKED_POINT);
    //SbPlaneProjector *proj=new SbPlaneProjector;
    //proj->setViewVolume(viewer->getCamera()->getViewVolume());
    //cout << "TEST pos  : " << *((Vector3*)&pos[0]) << endl;
    //cout << "TEST proj : " << *((Vector3*)&proj->project(pos)[0]) << endl;

    SbVec3f uaaa = MAIN_PROJ->project(pos);
    Vector3 uaaavec(*(Vector3*)&uaaa[0]);
    cout << "Proj :  to " << uaaavec << endl;

    picked_biarc->setPoint(uaaavec);

    SoChildList *children = new SoChildList(scene);
    Tube<Vector3>* bez_tub;
    children = scene->getChildren();
    
    for (int i=0;i<Knot->tubes();i++) {
      //knot_shape[i]->nodes.setValue(tN);
      if (view_mode==BIARC_VIEW)
	children->remove(0);
    }
    
    if (view_mode==BIARC_VIEW) {
      for (int i=0;i<Knot->tubes();i++) {
	bez_tub = knot_shape[i]->getKnot();
	bez_tub->make_default();
	addBezierCurve(scene,bez_tub);
      }
    }
    

  }
}

//Mouse callback
static void mousefunc(void *data, SoEventCallback *eventCB) {

  if (view_mode!=BIARC_VIEW) return;

  //AllData *appData=(AllData*)data;
  //SoQtExaminerViewer *viewer = (SoQtExaminerViewer*)data;
  VVV *viewer = (VVV*)data;

  // Scene *scene=appData->scene;
  const SoMouseButtonEvent *mbe=(SoMouseButtonEvent*)eventCB->getEvent();
  
  //Handle point grabbing
  if(mbe->getButton() == SoMouseButtonEvent::BUTTON1 && mbe->getState() == SoButtonEvent::DOWN) {
    
    //Get viewport point and search for 3D picked point

    // viewer does not give back the realroot!!!
    SoRayPickAction rp(viewer->getViewportRegion());

    //assert(viewer!=NULL);
    //viewer->getViewportRegion();

    rp.setPoint(mbe->getPosition());
    rp.apply(viewer->getSceneManager()->getSceneGraph());
    SoPickedPoint *point = rp.getPickedPoint();

cout << "Ray : " << *((Vector3*)&(rp.getLine().getDirection()[0])) <<endl;

    if(point == NULL) {
      cout << "no point picked\n";
      return;
    }
    eventCB->setHandled();

    //Get the first picked node
    SoNode *node=point->getPath()->getTail();
    if(node->getTypeId()!=SoSphere::getClassTypeId()) {
      cout << "picked point not a SoSphere\n";
      return;
    }

    //It's a sphere

    Vector3 pp(point->getPoint()[0],
	       point->getPoint()[1],
	       point->getPoint()[2]);
    //cout << "Mouse down at " << point->getObjectPoint()[0] << ' '
    // << point->getObjectPoint()[1] << ' '
    // << point->getObjectPoint()[2] << endl;
    
    cout << "Screen " << mbe->getNormalizedPosition(viewer->getViewportRegion())[0] << " " << mbe->getNormalizedPosition(viewer->getViewportRegion())[1] << " - World : " << pp << endl ;

    // TEMP TEMP TEMP
    EXACT_PICKED_POINT = point->getPoint();

    Vector3 cp;
    int FOUND = 0;

    // FIXME: If we have more than one curve, give this as userData to the callback
    Biarc<Vector3>* current;
    for (int l=0;l<Knot->tubes();l++) {
      current = knot_shape[l]->getKnot()->listHead();
      float Tolerance = (current->getPoint()-current->getNext()->getPoint()).norm()/4.0;
      //cout << "Tolerance is " << Tolerance << endl;
      while (current!=knot_shape[l]->getKnot()->listTail()) {
	if ((current->getPoint()-pp).norm()<Tolerance) {
	  cp = current->getPoint();
	  picked_biarc = current;
	  //cout << "found : " << current << endl;
	  FOUND = 1;
	  break;
	}
	current = current->getNext();
      }
      if (!FOUND) {
	if ((current->getPoint()-pp).norm()<Tolerance) {
	  cp = current->getPoint();
	  picked_biarc = current;
	  //cout << "found : " << current << endl;
	  FOUND = 1;
	}
      }
   
      //if (cp==Vector3(0,0,0))
      //	cout << "No near point on curve\n";
      //else
      if (FOUND) {
    //Set the projection plane

    // origianal : SbPlane plane(viewer->getCamera()->getViewVolume().getProjectionDirection(), point->getPoint());// SbVec3f((float*)&EXACT_PICKED_POINT[0])); //point->getPoint());
SbPlane plane(viewer->getCamera()->getViewVolume().getProjectionDirection(), (SbVec3f)&cp[0]);

    SbPlaneProjector *proj=new SbPlaneProjector(plane,TRUE);
    proj->setViewVolume(viewer->getCamera()->getViewVolume());
    
//cout << "Cam info : " << viewer->getCamera()->position.getValue()[0] << " "
//<< viewer->getCamera()->position.getValue()[1] << " " << viewer->getCamera()->position.getValue()[2] << endl;
//#define show(x) (cout << (x[0]) << " " << (x[1]) << " " << (x[2])<<endl)
//cout << "Plane info : ";
//show(plane.getNormal());
//cout << plane.getDistanceFromOrigin() << endl;

    MAIN_PROJ = proj;
	//cout << "Closest point on curve : " << cp << endl;
	PickedCurvePoint = cp;
	PRESSED = 1;
	PRESSED_AT = mbe->getNormalizedPosition(viewer->getViewportRegion());
	cout << "Screen " << PRESSED_AT[0] << " " << PRESSED_AT[1] << " - World : "
    << *((Vector3*)&(MAIN_PROJ->project(mbe->getNormalizedPosition(viewer->getViewportRegion())))[0])
	 << " !!\n" << "--------\n";
cout << "Biarc is : " << picked_biarc->getPoint() << endl;
cout << "Obj coord: " << (Vector3((float*)&(point->getObjectPoint())[0])) << endl;

    SbPlaneProjector *proj2=new SbPlaneProjector;
    proj2->setViewVolume(viewer->getCamera()->getViewVolume());
    cout << "TEST pos  : " << *((Vector3*)&PRESSED_AT[0]) << endl;
    cout << "TEST proj : " << *((Vector3*)&proj2->project(PRESSED_AT)[0]) << endl;


      }
      else
	PRESSED = 0;
    }
     
   }

   //Handle ungrabbing
   if(mbe->getButton() == SoMouseButtonEvent::BUTTON1 && mbe->getState() == SoButtonEvent::UP)
   {
     if (PRESSED) {

       SoChildList *children = new SoChildList(scene);
       Tube<Vector3>* bez_tub;
       children = scene->getChildren();

       for (int i=0;i<Knot->tubes();i++) {
	 //knot_shape[i]->nodes.setValue(tN);
	 if (view_mode==BIARC_VIEW)
	   children->remove(0);
       }
       
       if (view_mode==BIARC_VIEW) {
	 for (int i=0;i<Knot->tubes();i++) {
	   bez_tub = knot_shape[i]->getKnot();
	   bez_tub->make_default();
	   addBezierCurve(scene,bez_tub);
	 }
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
  float Rad = ((*t)[0]->getMidPoint()-(*t)[0]->getPoint()).norm()/5.0;
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
    Vector3 p = (*t)[j]->getPoint();
    data_trans[j]->translation.setValue((float *)&p);
    if (j!=t->nodes()-1 || t->isClosed()) {
      Vector3 m = (*t)[j]->getMidPoint();
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
  float TanLength = ((*t)[0]->getMidPoint()-(*t)[0]->getPoint()).norm()*.8;

  for (int i=0;i<t->nodes();i++) {
    Vector3 point    = (*t)[i]->getPoint();
    Vector3 endpoint = point + (*t)[i]->getTangent()*TanLength;
    
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
    Vector3 point    = (*t)[i]->getMidPoint();
    Vector3 endpoint = point + (*t)[i]->getMidTangent()*TanLength;
    
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

  assert(t->listHead()->isBiarc());
  float o;
  Vector3 q0,mid0,q1,mid1,dd1,dd2;
  for (int i=0;i<NODES;i++) {//t->nodes()-1;i++) {
    (*t)[i]->getBezier(q0,mid0,dd1,q1,mid1,dd2);
    CurvePts->point.set1Value(4*i+0,q0[0],q0[1],q0[2],1.0);
    o = ((mid0-q0).dot((*t)[i]->getTangent()))/sqrt((mid0-q0).dot(mid0-q0));
    CurvePts->point.set1Value(4*i+1,mid0[0],mid0[1],mid0[2],o);
    CurvePts->point.set1Value(4*i+2,q1[0],q1[1],q1[2],1.0);
    o = ((mid1-q1).dot((*t)[i]->getMidTangent()))/sqrt((mid1-q1).dot(mid1-q1));
    CurvePts->point.set1Value(4*i+3,mid1[0],mid1[1],mid1[2],o);
  }
  Vector3 gaga;
  if (t->isClosed())
    gaga = (*t)[0]->getPoint();
  else
    gaga = (*t)[t->nodes()-1]->getPoint();
  CurvePts->point.set1Value(Kknots-1,gaga[0],gaga[1],gaga[2],1.0);

  CurveSep->addChild(CurvePts);
  CurveSep->addChild(Curve);
  CurveSep->unrefNoDelete();
  main->addChild(CurveSep);

  root->addChild(main);

}

#endif // DOXYGEN_SHOULD_SKIP_THIS
