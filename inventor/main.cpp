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
#include "gui.h"
// #include "viewer.h"

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
// SoSeparator *root, *interaction, *scene;
// SoSeparator *circles;

char filename[50];

Tube<Vector3> tube_tmp;
// Color index used for material assignment
// static uint ColorIdx = 0;
// Number of colors in ColorTable
const uint ColorNum = 7;
SbColor ColorTable[ColorNum];

ViewerInfo viewer_info;
CurveInterface curve_interface;
MainWindow *myViewer;

Aux2DPlotWindow *pl_win = NULL;

void init(MainWindow *viewer) {

  // Generate ColorTable
  ColorTable[0] = SbColor(1,0,0);
  ColorTable[1] = SbColor(0,1,0);
  ColorTable[2] = SbColor(0,0,1);
  ColorTable[3] = SbColor(1,1,0);
  ColorTable[4] = SbColor(1,0,1);
  ColorTable[5] = SbColor(0,1,1);
  ColorTable[6] = SbColor(.4,.4,.4);

  /*
    First time initialization. The filenames to read
    have been set in "parse"! Read them in the CurveInterface
    structure, construct meshes and the graph structure.
  */
  if (viewer->ci->info.filenames.size()>0) {
    viewer->ci->load();
    viewer->ci->dumpInfo();
  }
  else
    cout << "[Info] No files to be loaded!\n";

  if (viewer->vi->IV_SCENE) {
    SoInput iv_in;
    cout << "Read file " << viewer->vi->iv_file.toLocal8Bit().constData() << endl;
    if (iv_in.openFile((char*)(viewer->vi->iv_file.toLocal8Bit().constData()))) {
      SoSeparator *external_iv = SoDB::readAll(&iv_in);
      if (external_iv) viewer->root->addChild(external_iv);
      iv_in.closeFile();
    }
    else {
      cerr << "[Warning] " << viewer->vi->iv_file.toLocal8Bit().constData() << " IV file Problem. Skipped\n";
    }
  }

  // root->addChild(interaction);
  viewer->root->addChild(viewer->scene);
  viewer->root->addChild(viewer->circles);
  if (viewer->ci->info.Knot->tubes()>0)
    viewer->scene->addChild(viewer->ci->graph_node);

  // root->removeChild(ci->graph_node);
  // ci->graph_node = NULL;
  // root->addChild(ci->graph_node);
}

int main(int argc, char **argv) {

  // Q_INIT_RESOURCE(application);

  myWindow = SoQt::init("libbiarc viewer");
  if (!myWindow) { cerr << "[Err] SoQt::init failed!\n"; exit(2); }
  SoKnot::initClass();

  // Setup viewer (main window)
  cout << "Viewer ..." << flush;
  myViewer = new MainWindow(myWindow,"libbiarc viewer",TRUE,
                            SoQtFullViewer::BUILD_ALL,
                            SoQtViewer::BROWSER);
  cout << " [OK]\n";

  // Set parameters according to command line args
  cout << "Parse ..." << flush;
  if(!parse(argc,argv,myViewer->vi,&(myViewer->ci->info))) usage(argv[0]);
  cout << " [OK]\n";

  init(myViewer);

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

  myViewer->setSceneGraph(myViewer->root);
  myViewer->setSize(SbVec2s(_SCREEN_W_,_SCREEN_H_));

  myViewer->setEventCallback(myAppEventHandler,myViewer);
  myViewer->setTransparencyType(SoGLRenderAction::SORTED_OBJECT_SORTED_TRIANGLE_BLEND);

  if (myViewer->vi->BackGroundFlag)
    myViewer->setBackgroundColor(SbColor(1,1,1));

  myViewer->setFeedbackVisibility(TRUE);
  myViewer->setCameraType(SoOrthographicCamera::getClassTypeId());

  // mainwindow.setCentralWidget(myWindow);

  // myViewer->setTitle("KnotViewer");
  // myViewer->show();
  SoQt::setWidgetSize(myViewer,SbVec2s(_SCREEN_W_,_SCREEN_H_));
  //SoQt::setWidgetSize(&mainwindow,SbVec2s(_SCREEN_W_,_SCREEN_H_));
  //mainwindow.setGeomtery(50,50,200,200);
  //myWindow->setGeometry(20,20,100,100);
  //SoQt::setWidgetSize(myWindow,SbVec2s(50,50));
  SoQt::show(myViewer); // myWindow);

  if (myViewer->vi->PT_PLOT) {
    if (!pl_win) {
      pl_win = new Aux2DPlotWindow(myViewer->ci->knot_shape[0]->getKnot(),NULL,"2dwindow");
      pl_win->setAttribute(Qt::WA_NoBackground);
      pl_win->setWindowTitle("2D Window");
      pl_win->setGeometry(_SCREEN_W_+8,0,200,200);
      QObject::connect(pl_win,SIGNAL(pos_changed(float,float,float,float)),
                       myViewer,SLOT(updatePicked(float,float,float,float)));

    }
    if (pl_win->loadImage((const char*)(myViewer->vi->ptplot_file.toLocal8Bit().constData()))) {
      pl_win->repaint();
      if (pl_win->isVisible()) pl_win->hide();
      else pl_win->show();
    }
    else  {
      pl_win->hide();
      cerr << "[Warning] Could not load "
           << myViewer->vi->ptplot_file.toLocal8Bit().constData() << ". Skipped\n";
    }
  }

  cout << "Start main loop\n" << flush;
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
