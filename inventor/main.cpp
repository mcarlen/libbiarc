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
#include "utils.h"
// #include "gui.h"
#include "viewer.h"

#include <QApplication>
#include <Inventor/details/SoPointDetail.h>
#include <Inventor/details/SoLineDetail.h>
#include <Inventor/nodes/SoDirectionalLight.h>
#include <Inventor/actions/SoWriteAction.h>
#include <Inventor/nodes/SoOrthographicCamera.h>

#include "mainwindow.h"

#ifdef RENDERMAN
#include "ri.h"
#endif

bool PRESSED = 0;
vector<Biarc<Vector3> >::iterator picked_biarc;
SbPlaneProjector spp;
SbVec3f UpVector, LeftVector, delta;
float AspectratioX, AspectratioY;

vector<Biarc<Vector3> >::iterator FirstBiarc;

#define _SCREEN_W_ 800
#define _SCREEN_H_ 600

QWidget *myWindow;

//SoQtExaminerViewer *myViewer;
SoSeparator *root, *interaction, *scene;
SoSeparator *circles;

char filename[50];

Tube<Vector3> tube_tmp;
// Color index used for material assignment
// static uint ColorIdx = 0;
// Number of colors in ColorTable
const uint ColorNum = 7;
SbColor ColorTable[ColorNum];

static float transp = 0.0;

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

ViewerInfo viewer_info;
CurveInterface curve_interface;
VVV *myViewer;

Aux2DPlotWindow *pl_win = NULL;

void init(ViewerInfo *vi, CurveInterface *ci) {

    // Generate ColorTable
    ColorTable[0] = SbColor(1,0,0);
    ColorTable[1] = SbColor(0,1,0);
    ColorTable[2] = SbColor(0,0,1);
    ColorTable[3] = SbColor(1,1,0);
    ColorTable[4] = SbColor(1,0,1);
    ColorTable[5] = SbColor(0,1,1);
    ColorTable[6] = SbColor(.4,.4,.4);

    ci->load(transp);
    ci->dumpInfo();

exit(0);

    myWindow = SoQt::init("KnotViewer");
    if (myWindow == NULL) exit(2);

    // init my class
    SoKnot::initClass();

    root = new SoSeparator; root->ref();
    circles = new SoSeparator; circles->ref();
    interaction = new SoSeparator; interaction->ref();
    scene = new SoSeparator; scene->ref();

    if (vi->IV_SCENE) {
      SoInput iv_in;
      if (iv_in.openFile((char*)(vi->iv_file.data()))) {
        SoSeparator *external_iv = SoDB::readAll(&iv_in);
        if (external_iv) root->addChild(external_iv);
        iv_in.closeFile();
      }
      else {
        cerr << "[Warning] " << vi->iv_file.data() << " IV file Problem. Skipped\n";
      }
    }

    // root->addChild(interaction);
    root->addChild(scene);
    root->addChild(circles);
    /*
      First time initialization. The filenames to read
      have been set in "parse"! Read them in the CurveInterface
      structure, construct meshes and the graph structure.
    */
    if (ci->info.Knot->tubes()>0)
      root->addChild(ci->curveSeparator(transp));

    cout << "\t[OK]\n";  
  
}

int main(int argc, char **argv) {

  // Q_INIT_RESOURCE(application);

  // Set parameters according to command line args
  cout << "Parse ..." << flush;
  if(!parse(argc,argv,&viewer_info,&(curve_interface.info))) usage(argv[0]);
  cout << " [OK]\n" << flush;

  init(&viewer_info, &curve_interface);

 // myViewer=new SoQtExaminerViewer(myWindow);
  myViewer = new VVV(myWindow,"viewer",TRUE,SoQtFullViewer::BUILD_ALL,SoQtViewer::BROWSER);
  MainWindow mainwindow(myWindow);

  // Create event handler for mouse
/*
  SoEventCallback *mouseEvent = new SoEventCallback;
  mouseEvent->addEventCallback(SoMouseButtonEvent::getClassTypeId(), mousefunc, myViewer);// &appData);
  interaction->addChild(mouseEvent);
  
  // Create event handler for mouse motion
  SoEventCallback *motionEvent = new SoEventCallback;
  motionEvent->addEventCallback(SoLocation2Event::getClassTypeId(), motionfunc, myViewer);//&appData);
  root->addChild(motionEvent);
*/

  myViewer->setSceneGraph(root);
  myViewer->setSize(SbVec2s(_SCREEN_W_,_SCREEN_H_));

//  myViewer->setEventCallback(myAppEventHandler,myViewer);
  myViewer->setTransparencyType(SoGLRenderAction::SORTED_OBJECT_SORTED_TRIANGLE_BLEND);

  if (viewer_info.BackGroundFlag)
    myViewer->setBackgroundColor(SbColor(1,1,1));

  myViewer->setFeedbackVisibility(TRUE);
  myViewer->setCameraType(SoOrthographicCamera::getClassTypeId());

  // mainwindow.setCentralWidget(myWindow);

  // myViewer->setTitle("KnotViewer");
  // myViewer->show();
  SoQt::setWidgetSize(&mainwindow,SbVec2s(_SCREEN_W_,_SCREEN_H_));
  //SoQt::setWidgetSize(&mainwindow,SbVec2s(_SCREEN_W_,_SCREEN_H_));
  //mainwindow.setGeomtery(50,50,200,200);
  //myWindow->setGeometry(20,20,100,100);
  //SoQt::setWidgetSize(myWindow,SbVec2s(50,50));
  SoQt::show(&mainwindow); // myWindow);

  if (viewer_info.PT_PLOT) {
    if (!pl_win) {
      pl_win = new Aux2DPlotWindow(NULL,"2dwindow");
      pl_win->setAttribute(Qt::WA_NoBackground);
      pl_win->setWindowTitle("2D Window");
      pl_win->setGeometry(_SCREEN_W_+8,0,200,200);
      QObject::connect(pl_win,SIGNAL(pos_changed(float,float,float,float)),
                       myViewer,SLOT(update_picked(float,float,float,float)));

    }
    if (pl_win->loadImage((const char*)(viewer_info.ptplot_file.data()))) {
      pl_win->repaint();
      if (pl_win->isVisible()) pl_win->hide();
      else pl_win->show();
    }
    else  {
      pl_win->hide();
      cerr << "[Warning] Could not load "
           << viewer_info.ptplot_file.data() << ". Skipped\n";
    }
  }

  SoQt::mainLoop();

}

void exportIV() {
  SoWriteAction myAction;
   
  myAction.getOutput()->openFile("curve.iv");
  myAction.getOutput()->setBinary(FALSE);
  myAction.apply(myViewer->getSceneManager()->getSceneGraph());
  myAction.getOutput()->closeFile();
}

#endif // DOXYGEN_SHOULD_SKIP_THIS
