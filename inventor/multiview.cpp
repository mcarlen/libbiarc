/*!
  \file multiview.cpp
  \ingroup InventorGroup
  \brief Visualize a PKF file (more than one component possible).

  Long explanation ... 

*/

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include "../include/TubeBundle.h"
#include <Inventor/Qt/SoQt.h>
#include <Inventor/Qt/viewers/SoQtExaminerViewer.h>
#include "SoKnot.h"

#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoCoordinate4.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoNurbsCurve.h>
#include <Inventor/nodes/SoSphere.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoCylinder.h>
#include <Inventor/nodes/SoRotation.h>
#include <Inventor/fields/SoSFRotation.h>
#include <Inventor/nodes/SoQuadMesh.h>
#include <Inventor/nodes/SoShapeHints.h>
#include <Inventor/nodes/SoNormal.h>
#include <Inventor/nodes/SoNormalBinding.h>
#include <Inventor/nodes/SoMaterial.h>

/*
 * Inventor interaction stuff
 */

#include <Inventor/Qt/devices/SoQtKeyboard.h>
#include <Inventor/nodes/SoEventCallback.h>
#include <Inventor/events/SoEvent.h>
#include <Inventor/events/SoKeyboardEvent.h>

SbBool myAppEventHandler(void *userData, QEvent *anyevent);

#define _SCREEN_W_ 800
#define _SCREEN_H_ 600

QWidget *myWindow;
SoQtExaminerViewer *myViewer;
SoSeparator *root;

/* kick this */
SoMaterial *material;
static float transp = 0.0;
/* kick this end */

void usage(char* prog);
int parse(int argc, char** argv);

// Number of nodes, number of circular segments
int N, S;
// Tube radius, mesh correction tolerance
float R, Tol;
char filename[50];

TubeBundle<Vector3> *Knot;
SoKnot **knot_node;

int main(int argc, char **argv) {

  // Set parameters according to command line args
  
  
  cout << "Parse command line";
  if(!parse(argc,argv)) usage(argv[0]);
  cout << "\t\t[OK]\n";

  for (int i=0;i<Knot->tubes();i++)
    (*Knot)[i].link();

  cout << "Generate mesh for all curves"<<flush;
  Knot->makeMesh(N,S,R,1);
  cout << "\t[OK]\n";

  //#ifdef __INFO__
  cout << "Curves    : "<< Knot->tubes() << endl;
  for (int i=0;i<Knot->tubes();i++) {
    cout << "Curve " << i+1 << endl;
    cout << "Nodes     : "<< (*Knot)[i].nodes() << endl;
    cout << "Segments  : "<< (*Knot)[i].segments() << endl;
    cout << "Radius    : "<< (*Knot)[i].radius() << endl;
  }
    cout << "Tolerance : "<<Tol<<endl;
  //#endif
  myWindow = SoQt::init("KnotViewer");
  if (myWindow == NULL) exit(2);
  
  // init my class
  SoKnot::initClass();
  
  root = new SoSeparator; root->ref();
  
  cout << "Create " << Knot->tubes() << " SoKnot objects";
  knot_node = new SoKnot*[Knot->tubes()];
  cout << "\t[OK]\n";

  material = new SoMaterial;
  material->diffuseColor.setValue(1.0,0.0,0.0);
  material->transparency = transp;
  root->addChild(material);

  for (int i=0;i<Knot->tubes();i++) {

    knot_node[i] = new SoKnot;

    knot_node[i]->setKnot( &((*Knot)[i]), 1 );
    knot_node[i]->radius.setValue(R);
    knot_node[i]->segments.setValue(S);
    knot_node[i]->nodes.setValue(N);
    root->addChild(knot_node[i]);
  }

  myViewer=new SoQtExaminerViewer(myWindow);
  myViewer->setSceneGraph(root);
  myViewer->setSize(SbVec2s(_SCREEN_W_,_SCREEN_H_));

  myViewer->setEventCallback(myAppEventHandler,myViewer);
  myViewer->setTransparencyType(SoGLRenderAction::SORTED_OBJECT_SORTED_TRIANGLE_BLEND);

  myViewer->setTitle("KnotViewer");
  myViewer->show();

  SoQt::show(myWindow);
  SoQt::setWidgetSize(myWindow,SbVec2s(_SCREEN_W_,_SCREEN_H_));
  SoQt::mainLoop();

}

void usage(char *prog) {
  cerr << "Usage : " << prog << " [OPTIONS] <PFK file>\n"
       << "Optional arguments\n"
       << "  -N=NODES\t\tnumber of points on the curve\n"
       << "  -S=SEGMENTS\t\tcross sectional segmentation\n" 
       << "  -R=RADIUS\t\ttube radius\n"
       << "  -Tol=TOLERANCE\tmesh adjustment tolerance\n";
  
  exit(0);
}

int parse(int argc, char** argv) {

  if (argc<2) return 0;

  // create knot from pkf
  Knot = new TubeBundle<Vector3>(argv[argc-1]);
  // close the knot

  // default values
  S = 12;
  R = 0.0083; // experimental knot value
  Tol = 1e-03;
  N = (*Knot)[0].nodes();

  // Parse Options
  for (int i=1;i<argc-1;i++) {

    if (argv[i][0] != '-')
      return 0;

    if (!strncmp(&argv[i][1],"N=",2)) {
      N = atoi(&argv[i][3]);
    }
    else if (!strncmp(&argv[i][1],"S=",2))
      S = atoi(&argv[i][3]);
    else if (!strncmp(&argv[i][1],"R=",2))
      R = atof(&argv[i][3]);
    else if (!strncmp(&argv[i][1],"Tol=",4))
      Tol = atof(&argv[i][5]);
    else
      return 0;
  }

  return 1;
}

SbBool myAppEventHandler(void *userData, QEvent *anyevent) {

  //  SoQtRenderArea *myRenderArea = (SoQtRenderArea *) userData;
  QKeyEvent *myKeyEvent;
  //SbBool handled = TRUE;

  SoQtViewer::DrawStyle DStyle;

  if(anyevent->type()==QEvent::KeyPress) {
    myKeyEvent = (QKeyEvent *) anyevent;

    switch(myKeyEvent->key()) {

    // Transparency changing
    case Qt::Key_F:
      transp+=0.01;
      if (transp>1.0) transp = 1.0;      
      material->transparency = transp;
      break;

    case Qt::Key_V:
      transp-=0.01;
      if (transp<0.0) transp = 0.0;
      material->transparency = transp;
      break;

    case Qt::Key_A:
      cerr << " alive before \n"<<flush;
      for (int i=0;i<Knot->tubes();i++) {
	cerr << i <<'-'<<flush;
	float RR = knot_node[i]->radius.getValue();
	cout << "R="<<RR<<flush;
	knot_node[i]->radius = RR*1.1;
	//cout << "Radius : " << knot_node[i]->radius.getValue() << endl;
      }
      cerr << " alive after \n"<<flush;
      break;

    case Qt::Key_Z:
      for (int i=0;i<Knot->tubes();i++) {
	knot_node[i]->radius = knot_node[i]->radius.getValue()*0.9;
	//cout << "Radius : " << knot_node[i]->radius.getValue() << endl;
      }
      break;

    case Qt::Key_S:
      for (int i=0;i<Knot->tubes();i++) {
	knot_node[i]->segments.setValue(knot_node[i]->segments.getValue() + 1);
	//cout << "Segments : " << knot_node[i]->segments.getValue() << endl;
      }
      break;

    case Qt::Key_X:
      for (int i=0;i<Knot->tubes();i++) {
	knot_node[i]->segments.setValue(knot_node[i]->segments.getValue() - 1);
	//cout << "Segments : " << knot_node[i]->segments.getValue() << endl;
      }
      break;

    case Qt::Key_D:
      for (int i=0;i<Knot->tubes();i++) {
	knot_node[i]->nodes.setValue(knot_node[i]->nodes.getValue() + 10);
	//cout << "Nodes : " << knot_node[i]->nodes.getValue() << endl;
      }
      break;

    case Qt::Key_C:
      for (int i=0;i<Knot->tubes();i++) {
	knot_node[i]->nodes.setValue(knot_node[i]->nodes.getValue() - 10);
	//cout << "Nodes : " << knot_node[i]->nodes.getValue() << endl;
      }
      break;

    case Qt::Key_Space:
      DStyle = myViewer->getDrawStyle(SoQtViewer::STILL);
      if (DStyle==SoQtViewer::VIEW_AS_IS)
	DStyle = SoQtViewer::VIEW_HIDDEN_LINE;
      else
	DStyle = SoQtViewer::VIEW_AS_IS;

      myViewer->setDrawStyle(SoQtViewer::STILL,DStyle);      
      break;

    // Quit program
    case Qt::Key_Q:
      exit(0);

    }

    return TRUE;

  }
  return FALSE;
}

#endif // DOXYGEN_SHOULD_SKIP_THIS
