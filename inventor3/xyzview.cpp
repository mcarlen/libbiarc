/*!
  \file xyzview.cpp
  \ingroup InventorGroup
  \brief Visualize a closed data file.

  Long explanation ... Bla bla bla
*/

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include "../include/Tube.h"
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
 * Inventor interaction struff
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

SoKnot *knot_node;
Tube<Vector3> *Knot;

int main(int argc, char **argv) {

  // Set parameters according to command line args

  if(!parse(argc,argv)) usage(argv[0]);

  //Knot->center();
  //Knot->make(.5);
  //Knot->normalise();
  Knot->makeMesh(N,S,R,Tol);

  cout << "Nodes     : "<<Knot->nodes() << endl;
  cout << "Segments  : "<<Knot->segments() << endl;
  cout << "Radius    : "<<Knot->radius() << endl;
  cout << "Tolerance : "<<Tol<<endl;

  myWindow = SoQt::init("KnotViewer");
  if (myWindow == NULL) exit(2);

  // init my class
  SoKnot::initClass();

  root = new SoSeparator; root->ref();

  material = new SoMaterial;
  material->diffuseColor.setValue(1.0,0.0,0.0);
  material->transparency = transp;
  root->addChild(material);

  knot_node = new SoKnot;
  knot_node->setKnot( Knot, 1e-3 );

  knot_node->radius.setValue(R);
  knot_node->segments.setValue(S);
  knot_node->nodes.setValue(N);

  root->addChild(knot_node);

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
  cerr << "Usage : " << prog << " [OPTIONS] <closed curve xyz file>\n"
       << "Optional arguments\n"
       << "  -N=NODES\t\tnumber of points on the curve\n"
       << "  -S=SEGMENTS\t\tcross sectional segmentation\n" 
       << "  -R=RADIUS\t\ttube radius\n"
       << "  -Tol=TOLERANCE\tmesh adjustment tolerance\n";
  
  exit(0);
}

int parse(int argc, char** argv) {

  if (argc<2) return 0;

  Knot = new Tube<Vector3>; 
  Knot->readData(argv[argc-1], ",\t");
  Knot->link();
  Knot->computeTangents();

  // default values
  S = 12;
  R = 0.0083; // experimental knot value
  Tol = 1e-03;
  N = Knot->nodes();

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
      knot_node->radius = knot_node->radius.getValue()*1.1;
      cout << "Radius : " << knot_node->radius.getValue() << endl;
      break;

    case Qt::Key_Z:
      knot_node->radius = knot_node->radius.getValue()*0.9;
      cout << "Radius : " << knot_node->radius.getValue() << endl;
      break;

    case Qt::Key_S:
      knot_node->segments.setValue(knot_node->segments.getValue() + 1);
      cout << "Segments : " << knot_node->segments.getValue() << endl;
      break;

    case Qt::Key_X:
      knot_node->segments.setValue(knot_node->segments.getValue() - 1);
      cout << "Segments : " << knot_node->segments.getValue() << endl;
      break;

    case Qt::Key_D:
      knot_node->nodes.setValue(knot_node->nodes.getValue() + 10);
      cout << "Nodes : " << knot_node->nodes.getValue() << endl;
      break;

    case Qt::Key_C:
      knot_node->nodes.setValue(knot_node->nodes.getValue() - 10);
      cout << "Nodes : " << knot_node->nodes.getValue() << endl;
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
