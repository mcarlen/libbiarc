/*!
  \file curvature_view.cpp
  \ingroup CurvTorGroup
  \brief Curvature visualization

  Long explanation ... Bla bla bla
*/

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include "../include/Tube.h"

#include <Inventor/Qt/SoQt.h>
#include <Inventor/Qt/viewers/SoQtExaminerViewer.h>

// event handling
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoShapeHints.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoNormal.h>
#include <Inventor/nodes/SoNormalBinding.h>
#include <Inventor/nodes/SoMaterialBinding.h>
#include <Inventor/nodes/SoQuadMesh.h>

#define _SCREEN_W_ 800
#define _SCREEN_H_ 600

QWidget *myWindow;
SoQtExaminerViewer *myViewer;
SoSeparator *root;
Tube<Vector3>* tube;

void makeScene(SoSeparator* root, Tube<Vector3>* tube);

int main(int argc, char **argv) {
  if(argc != 5) {
    cerr << argv[0] << " <PFK file> <Tube Radius> <Nodes> <Segments>\n";
    exit(0);
  }

  tube = new Tube<Vector3>(argv[1]);
  tube->link();
  tube->makeMesh(atoi(argv[3]),atoi(argv[4]),atof(argv[2]),1e-3);

  myWindow = SoQt::init("KnotViewer");
  if (myWindow == NULL) exit(2);

  root = new SoSeparator;            // Root
  root->ref();

  makeScene(root, tube);

  myViewer=new SoQtExaminerViewer(myWindow);
  myViewer->setSize(SbVec2s(_SCREEN_W_,_SCREEN_H_));
  myViewer->setSceneGraph(root);
  myViewer->setTitle("Knot Curvature Visualizer");

  myViewer->show();

  SoQt::setWidgetSize(myWindow,SbVec2s(_SCREEN_W_,_SCREEN_H_));
  SoQt::show(myWindow);  // Display main window
  SoQt::mainLoop();      // Main Inventor event loop

  return 0;

}


void makeScene(SoSeparator* root, Tube<Vector3>* tube) {
    
  // Inventor Node Generation
  SoSeparator *subroot       = new SoSeparator;
  SoCoordinate3 *Coords      = new SoCoordinate3;
  SoQuadMesh *QuadMesh       = new SoQuadMesh;
  SoShapeHints *Hints        = new SoShapeHints;
  SoNormal *ShapeNormals          = new SoNormal;
  SoNormalBinding *NormBind  = new SoNormalBinding;

  subroot->ref();

  Hints->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE;
  Hints->shapeType = SoShapeHints::SOLID;
  //Hints->creaseAngle = M_PI;

  for (int i=0;i<((tube->nodes()+1)*(tube->segments()+1));i++) {
    Coords->point.set1Value(i,
			    tube->meshPoint(i)[0],
			    tube->meshPoint(i)[1],
			    tube->meshPoint(i)[2]);
    ShapeNormals->vector.set1Value(i,
				   tube->meshNormal(i)[0],
				   tube->meshNormal(i)[1],
				   tube->meshNormal(i)[2]);
  }
  
  NormBind->value = SoNormalBinding::PER_VERTEX;

  int N;
  if (tube->isClosed())
    N = tube->nodes()+1;
  else
    N = tube->nodes();

  QuadMesh->verticesPerColumn = N;
  QuadMesh->verticesPerRow = tube->segments()+1;

  // Color part
  SoMaterial *material       = new SoMaterial;
  SoMaterialBinding *matbind = new SoMaterialBinding;

  float max_c = 0, min_c = 1000;

  float curvature[tube->nodes()];
  for (int i=0;i<tube->nodes();i++) {
    curvature[i] = tube->curvature(i);
    if (curvature[i]<min_c) min_c = curvature[i];
    if (curvature[i]>max_c) max_c = curvature[i];
  }

  int count = 0;
  /* 
   * We project the curvature (which is between 0 and inf)
   * onto the interval [0,1] with x/(1+x)
   * this leads to a coloring where
   * red is high curvature
   * and blue low curvature
   * 
   * The mapping algorithm is (c) by Benoit Crouzy
   * benoit.crouzy@epfl.ch
   *
   */

  int mod;
  Vector3 rgb;
  for (int i=0;i<N;i++) {

    mod = i%tube->nodes();
    
    /* map [0,inf) -> [0,1]
    rgb[1] = 0.0;
    rgb[0] = curvature[mod]/(1+curvature[mod]);
    rgb[2] = 1.0-(curvature[mod]/(1+curvature[mod]));
    */

    rgb[1] = 0.0;
    rgb[0] = (curvature[mod]-min_c)/(max_c - min_c);
    rgb[2] = 1 - rgb[0];

    for (int j=0;j<=tube->segments();j++)
      material->diffuseColor.set1Value(count++,rgb[0],rgb[1],rgb[2]);
  }

  matbind->value = SoMaterialBinding::PER_VERTEX;

  subroot->addChild(matbind);
  subroot->addChild(material);

  subroot->addChild(Hints);

  subroot->addChild(NormBind);
  subroot->addChild(ShapeNormals);

  subroot->addChild(Coords);
  subroot->addChild(QuadMesh);

  root->addChild(subroot);

}

#endif // DOXYGEN_SHOULD_SKIP_THIS
