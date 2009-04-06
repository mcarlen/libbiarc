/*!
  \file animate.cpp
  \ingroup InventorGroup
  \brief Animate a series of pkf curves.

  Draws a series of pkf curves one after the other in
  a viewer. It is then possible to rotate, translate
  and scale the curves without stopping the animation.
*/

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include "../include/TubeBundle.h"
#include "SoKnot.h"

#include <Inventor/Qt/SoQt.h>
#include <Inventor/Qt/viewers/SoQtExaminerViewer.h>

#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSphere.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoBlinker.h>
#include <Inventor/nodes/SoText2.h>


int main(int argc,char** argv) {

  SoKnot **knots;
  TubeBundle<Vector3>* tb = new TubeBundle<Vector3>;
  for (int i=1;i<argc;i++) {
    if (!tb->readPKF(argv[i])) {
      cerr << "Aborted.\n";
      exit(-1);
    }
  }

  int N = (*tb)[0].nodes();
  int S = 36;
  float R = (*tb)[0].thickness_fast()*.25;
  if (R<0.01) R = 0.01;
  cout << "Radius " << R << endl;
  float Tol = 1e-2;

  for (int i=0;i<tb->tubes();i++) {
    (*tb)[i].link();
    (*tb)[i].makeMesh(N,S,R,Tol);
  }

  SoQt::init("tbViewer");
  SoKnot::initClass();
  knots = new SoKnot*[tb->tubes()];
  cout << "Ignore mesh warning, we set the segments later!\n";
  for (int i=0;i<tb->tubes();i++) {
    knots[i] = new SoKnot;
    knots[i]->radius.setValue(R);
    knots[i]->segments.setValue(S);
    knots[i]->nodes.setValue(N);
    knots[i]->setKnot( &((*tb)[i]), (*tb)[i].isClosed()?Tol:-1 );
    knots[i]->ref();
  }

  SoSeparator* root = new SoSeparator;
  root->ref();

  SoBlinker* blinker = new SoBlinker;
  root->addChild(blinker);
  blinker->speed = 0.1;

  SoSeparator **sep = new SoSeparator*[tb->tubes()];
  SoText2 **text = new SoText2*[tb->tubes()];
  for (int i=0;i<tb->tubes();++i) {
    sep[i]  = new SoSeparator;
    text[i] = new SoText2;
    char bbb[100];
    sprintf(bbb,"Pos : %d",i);
    text[i]->string = bbb;

    blinker->addChild(sep[i]);
    sep[i]->addChild(knots[i]);
    // sep[i]->addChild(text[i]);
  }
  
  SoQtExaminerViewer* viewer = new SoQtExaminerViewer;
  viewer->setSceneGraph(root);

  viewer->show();
  SoQt::mainLoop();

  return 0;
}

#endif
