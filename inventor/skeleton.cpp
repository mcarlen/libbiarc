/*!
  \file skeleton.cpp
  \ingroup InventorGroup
  \brief Visualize a closed PKF knot, the data points/tangnets,
  	matching points and tangents.

  Long explanation ... 

*/

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include "../include/Tube.h"
#include "../include/Vector4.h"
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
#include <Inventor/nodes/SoComplexity.h>

/*
 * Inventor interaction stuff
 */

#include <Inventor/Qt/devices/SoQtKeyboard.h>
#include <Inventor/nodes/SoEventCallback.h>
#include <Inventor/events/SoEvent.h>
#include <Inventor/events/SoKeyboardEvent.h>

SbBool myAppEventHandler(void *userData, QEvent *anyevent);
SoSeparator* addBezierCurve(SoSeparator *root, Tube<Vector3>* t);
SoSeparator* dummy;

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

  Knot->center();
  Knot->make_default();
  Knot->normalize();

  // FIXME
  // Caution, rebuild midpoints after call to normalize,
  // since midpoints are not!!! updated
  Knot->make_default();

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
  SoSeparator *tube_sep = new SoSeparator;
  tube_sep->addChild(material);

  knot_node = new SoKnot;
  knot_node->setKnot( Knot, 1e-3 );

  knot_node->radius.setValue(R);
  knot_node->segments.setValue(S);
  knot_node->nodes.setValue(N);

  dummy = addBezierCurve(root,Knot);
  tube_sep->addChild(knot_node);
  root->addChild(tube_sep);

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
  Knot = new Tube<Vector3>(argv[argc-1]);
  // close the knot
  Knot->link();

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

  int tS, tN;
  SoQtViewer::DrawStyle DStyle;

  if(anyevent->type()==QEvent::KeyPress) {
    myKeyEvent = (QKeyEvent *) anyevent;

    switch(myKeyEvent->key()) {

    // Transparency changing
    case Qt::Key_F:
      transp+=0.01;
      cout << transp << endl;
      if (transp>1.0) transp = 1.0;      
      material->transparency = transp;
      break;

    case Qt::Key_V:
      transp-=0.01;
      cout << transp << endl;
      if (transp<0.0) transp = 0.0;
      material->transparency = transp;
      break;

    case Qt::Key_A:
      knot_node->radius = knot_node->radius.getValue()*1.1;
      cout << "Radius : " << knot_node->radius.getValue() << endl;
      break;

    case Qt::Key_Y:
      knot_node->radius = knot_node->radius.getValue()*0.9;
      cout << "Radius : " << knot_node->radius.getValue() << endl;
      break;

    case Qt::Key_S:
      tS = knot_node->segments.getValue()+1;
      if (tS>120) {
	cerr << "Warning : more than 120 segments requested. Set to 120!\n";
	tS = 120;
      }
      knot_node->segments.setValue(tS);
      cout << "Segments : " << tS << endl;
      break;

    case Qt::Key_X:
      tS = knot_node->segments.getValue()-1;
      if (tS<3) {
	cerr << "Warning : less than 3 segments not allowed. Set to 3!\n";
	tS = 3;
      }
      knot_node->segments.setValue(tS);
      cout << "Segments : " << tS << endl;
      break;

    case Qt::Key_D:
      tN = knot_node->nodes.getValue()+10;
      knot_node->nodes.setValue(tN);
      cout << "Nodes : " << tN << endl;
      dummy->unref();
      addBezierCurve(root,Knot);
      break;

    case Qt::Key_C:
      tN = knot_node->nodes.getValue()-10;
      if (tN<2) {
	cerr << "Warning : less than 2 nodes requested. Set to 2!\n";
	tN=2;
      }
      knot_node->nodes.setValue(tN);
      cout << "Nodes : " << tN << endl;
      dummy->unref();
      addBezierCurve(root,Knot);
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

SoSeparator* addBezierCurve(SoSeparator *root, Tube<Vector3>* t) {

  // OpenInventor part
  // Midpoints

  SoSeparator *midpoints         = new SoSeparator;            // Root
  SoTranslation **mid_trans      = new SoTranslation*[t->nodes()];
  SoSeparator **sub_midpoints    = new SoSeparator*[t->nodes()];
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
    sub_midpoints[i] = new SoSeparator;
    sub_datapoints[i] = new SoSeparator;
    data_trans[i] = new SoTranslation;
    mid_trans[i] = new SoTranslation;

    sub_midpoints[i]->ref();
    sub_datapoints[i]->ref();
    data_trans[i]->ref();
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
    Vector3 m = (*t)[j]->getMidPoint();
    mid_trans[j]->translation.setValue((float *)&m);
    data_trans[j]->translation.setValue((float *)&p);

    sub_midpoints[j]->addChild(mid_trans[j]);
    sub_midpoints[j]->addChild(sphere);
    sub_datapoints[j]->addChild(data_trans[j]);
    sub_datapoints[j]->addChild(sphere);

    midpoints->addChild(sub_midpoints[j]);
    datapoints->addChild(sub_datapoints[j]);

  }
  
  SoMaterial *white = new SoMaterial;
  white->diffuseColor.setValue(1,1,1);

  root->addChild(datapoints);
  root->addChild(midpoints);
  root->addChild(white);
  /////

  SoSeparator *CurveSep = new SoSeparator;
  CurveSep->ref();
  SoCoordinate4 *CurvePts = new SoCoordinate4;
  SoNurbsCurve  *Curve    = new SoNurbsCurve;

  int Order = 3;// or 2?
  int Kknots = 4*t->nodes()+1;
  if (Kknots<=Order) Kknots = 3;

  int KLength = (Order + Kknots);
  Curve->numControlPoints = Kknots;

  float KSequence[KLength];
  KSequence[0] = KSequence[1] = KSequence[2] = 0;
  KSequence[KLength-1] = KSequence[KLength-2] = KSequence[KLength-3] = 1;  
  if (Kknots==0|KLength==0) {cout<<"KnotVector empty\n";return NULL;}
  int ctrlpts = (Kknots-1)/2;   // Number of Weighted Control Points
  int counter = 1;
  if (ctrlpts==1) return NULL;
  for (int i=3;i<(KLength-3);i++) {
    KSequence[i++] = float(counter)/float(ctrlpts);
    KSequence[i]   = float(counter)/float(ctrlpts);
    counter++;
  }

  Curve->knotVector.setValues(0,KLength,KSequence);//KV.getSequence());

  assert(t->listHead()->isBiarc());
  float o;
  Vector3 q0,mid0,q1,mid1,dd1,dd2;
  for (int i=0;i<t->nodes();i++) {
    (*t)[i]->getBezier(q0,mid0,dd1,q1,mid1,dd2);
    CurvePts->point.set1Value(4*i+0,q0[0],q0[1],q0[2],1.0);
    o = ((mid0-q0).dot((*t)[i]->getTangent()))/sqrt((mid0-q0).dot(mid0-q0));
    CurvePts->point.set1Value(4*i+1,mid0[0],mid0[1],mid0[2],o);
    CurvePts->point.set1Value(4*i+2,q1[0],q1[1],q1[2],1.0);
    o = ((mid1-q1).dot((*t)[i]->getMidTangent()))/sqrt((mid1-q1).dot(mid1-q1));
    CurvePts->point.set1Value(4*i+3,mid1[0],mid1[1],mid1[2],o);
  }
  (*t)[0]->getBezier(q0,mid0,dd1,q1,mid1,dd2);
  CurvePts->point.set1Value(Kknots-1,q0[0],q0[1],q0[2],1.0);

  CurveSep->addChild(CurvePts);
  CurveSep->addChild(Curve);
  //CurveSep->unrefNoDelete();
  root->addChild(CurveSep);

  return CurveSep;
}


#endif // DOXYGEN_SHOULD_SKIP_THIS
