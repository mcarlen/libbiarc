/*!
  \file animate.cpp
  \ingroup InventorGroup
  \brief Animate a series of pkf curves.

  Draws a series of pkf curves one after the other in
  a viewer. It is then possible to rotate, translate
  and scale the curves without stopping the animation.
*/

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include <Inventor/Qt/SoQt.h>
#include <Inventor/Qt/viewers/SoQtExaminerViewer.h>

#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoBlinker.h>

int main(int argc,char** argv) {

  SoQt::init("tbViewer");
  SoSeparator* root = new SoSeparator;
  root->ref();

  SoBlinker* blinker = new SoBlinker;
  root->addChild(blinker);
  blinker->speed = 0.1;

  SoInput iv_in;
  for (int i=1;i<argc;i++) {
    printf("%s\n",argv[i]);
    iv_in.openFile(argv[i]);
    SoSeparator *external_iv = SoDB::readAll(&iv_in);
    SoSeparator *bla = new SoSeparator;
    bla->addChild(external_iv);
    if (external_iv) blinker->addChild(bla);
    iv_in.closeFile();
  }

  SoQtExaminerViewer* viewer = new SoQtExaminerViewer;
  viewer->setSceneGraph(root);

  viewer->show();
  SoQt::mainLoop();

  return 0;
}

#endif
