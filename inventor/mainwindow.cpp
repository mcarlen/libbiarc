#include "mainwindow.h"
#include "pp.h"
#include "pt.h"
#include "tt.h"

#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/details/SoPointDetail.h>
#include <Inventor/details/SoLineDetail.h>

/*!
  \class MainWindow inventor/mainwindow.h inventor/mainwindow.h
	\ingroup InventorGroup
	\brief Main viewer class.

	The main viewer contains information about the curve objects and the
	current scene. It inherits SoQtExaminerViewer and add custom
	menu and toolbars. Various plots can be generated and inventor
	scene files *.iv can be loaded and included into the scene as well.
*/

SOQT_OBJECT_SOURCE(MainWindow);

//Mouse motion callback
static void motionfunc(void *data, SoEventCallback *eventCB) {

  MainWindow* viewer = (MainWindow*)data;

  if (viewer->view_mode==BIARC_VIEW && viewer->PRESSED) {
    
    const SoMouseButtonEvent *mbe=(SoMouseButtonEvent* )eventCB->getEvent();
    
    if (viewer->EditTangent) {
      SbVec3f loc = viewer->spp.project(mbe->getNormalizedPosition(viewer->getViewportRegion())) + viewer->delta;
      loc += (viewer->LeftVector*(viewer->AspectratioX-1.f)*(loc.dot(viewer->LeftVector))
               + viewer->UpVector*(viewer->AspectratioY-1.f)*(loc.dot(viewer->UpVector))) ;
      Vector3 editT((float*)&loc[0]);
      viewer->picked_biarc->setTangent(editT-viewer->picked_biarc->getPoint());
    }
    else {
      SbVec3f loc = viewer->spp.project(mbe->getNormalizedPosition(viewer->getViewportRegion())) + viewer->delta;
    
      loc += (viewer->LeftVector*(viewer->AspectratioX-1.f)*(loc.dot(viewer->LeftVector))
               + viewer->UpVector*(viewer->AspectratioY-1.f)*(loc.dot(viewer->UpVector))) ;

      viewer->picked_biarc->setPoint(Vector3((float*)&loc[0]));
    }

    SoSeparator* sep = new SoSeparator;
    Tube<Vector3>* bez_tub;
    
    for (int i=0;i<viewer->ci->info.Knot->tubes();i++) {
      bez_tub = viewer->ci->knot_shape[i]->getKnot();
      bez_tub->make_default();
      addBezierCurve(sep,bez_tub,viewer->vi->BackGroundFlag);
    }
    viewer->scene->replaceChild(1,sep);
  }
}


static void mousefunc(void* data, SoEventCallback* eventCB) {

  MainWindow* viewer = (MainWindow*)data;
  if (viewer->view_mode!=BIARC_VIEW) return;

  Tube<Vector3>* bez_tub;
  const SoMouseButtonEvent *mbe=(SoMouseButtonEvent*)eventCB->getEvent();
  
  //Handle point grabbing
  if(mbe->getButton() == SoMouseButtonEvent::BUTTON1 && mbe->getState() == SoButtonEvent::DOWN) {
    
    SoRayPickAction rp(viewer->getViewportRegion());
    rp.setPoint(mbe->getPosition());
    rp.apply(viewer->getSceneManager()->getSceneGraph());
    
    SoPickedPoint *point = rp.getPickedPoint();

    viewer->EditTangent = 0;
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
        for (int l=0;l<viewer->ci->info.Knot->tubes();l++) {
          current = viewer->ci->knot_shape[l]->getKnot()->begin();
          float Tolerance = (current->getPoint()-current->getNext().getPoint()).norm()/4.0;
          while (current!=viewer->ci->knot_shape[l]->getKnot()->end()) {
  	    if ((current->getPoint()-pp).norm()<Tolerance) {
	      cp = current->getPoint();
  	      viewer->picked_biarc = current;
              cout << "Picked biarc " << viewer->picked_biarc->id() << endl;
	      FOUND = 1;
	      break;
  	    }
	    ++current;
          }
        }

        if (FOUND && viewer->vi->ResamplePartFlag) {
          cout << "Resample\n";
          if (!viewer->vi->FirstPoint) {
            viewer->vi->FirstBiarc = viewer->picked_biarc;
            viewer->vi->FirstPoint = 1;
          }
           else {
             // XXX only single components!
             bez_tub = viewer->ci->knot_shape[0]->getKnot();
             bez_tub->make_default();
             // XXX resample from FirstBiarc to picked_biarc with 10 points
             bez_tub->refine(viewer->vi->FirstBiarc,viewer->picked_biarc,5);
             bez_tub->make_default();
             cout << "BEZ REFINE OK\n" <<flush;
             for (int i=0;i<viewer->scene->getChildren()->getLength();i++)
               viewer->scene->getChildren()->remove(1);
             viewer->ci->knot_shape[0]->updateMesh(viewer->ci->info.Tol);
             addBezierCurve((SoSeparator*)viewer->scene,bez_tub,viewer->vi->BackGroundFlag);
             viewer->vi->FirstPoint = 0;
             viewer->vi->ResamplePartFlag = 0;
           }
        }
        else if (FOUND) {
          cout << "Found\n";
          SbViewVolume vv = viewer->getCamera()->getViewVolume();
          SbPlane s(-(vv.getProjectionDirection()), point->getPoint());
          viewer->spp = SbPlaneProjector(s);
          viewer->spp.setViewVolume(vv);

          // up and left vector according to the current camera position
          SbRotation rot = viewer->getCamera()->orientation.getValue();
          rot.multVec(SbVec3f(1,0,0),viewer->LeftVector);
          rot.multVec(SbVec3f(0,1,0),viewer->UpVector);

          SbVec2s winsize = viewer->getViewportRegion().getWindowSize();
          viewer->AspectratioX = (float)winsize[0]/(float)winsize[1];
          viewer->AspectratioY = 1;

          if (viewer->AspectratioX < 1) {
            viewer->AspectratioY = (float)winsize[1]/(float)winsize[0];
            viewer->AspectratioX = 1;
          }

          viewer->delta = SbVec3f((float*)&cp[0]) - point->getPoint();

          // If the viewport is not square we have a
          // aspect ratio problem. Meaning, that the vector
          // is actually smaller!
          viewer->delta -= (viewer->LeftVector*(1.f-1.f/viewer->AspectratioX)*(viewer->delta.dot(viewer->LeftVector))
                     + viewer->UpVector*(1.f-1.f/viewer->AspectratioY)*(viewer->delta.dot(viewer->UpVector))) ;

          viewer->PRESSED = 1;
          cout << "PRESSED = 1\n";
        }
      } // Sphere end
      else
        if(node && node->getTypeId()==SoLineSet::getClassTypeId()) {
          SoLineSet *ls = (SoLineSet*)node;
          if (ls->getName() != "datatangents") {
            eventCB->setHandled();
            return;
          }

          viewer->EditTangent = 1;

          int sl_idx = ((SoLineDetail*)(point)->getDetail())->getPoint0()->getCoordinateIndex();
          // XXX only single component
          viewer->picked_biarc = (viewer->ci->knot_shape[0]->getKnot()->begin()+(sl_idx>>1));
          
          // XXX same code as above!!!
          SbViewVolume vv = viewer->getCamera()->getViewVolume();
          SbPlane s(-(vv.getProjectionDirection()), point->getPoint());
          viewer->spp = SbPlaneProjector(s);
          viewer->spp.setViewVolume(vv);

          // up and left vector according to the current camera position
          SbRotation rot = viewer->getCamera()->orientation.getValue();
          rot.multVec(SbVec3f(1,0,0),viewer->LeftVector);
          rot.multVec(SbVec3f(0,1,0),viewer->UpVector);

          SbVec2s winsize = viewer->getViewportRegion().getWindowSize();
          viewer->AspectratioX = (float)winsize[0]/(float)winsize[1];
          viewer->AspectratioY = 1;

          if (viewer->AspectratioX < 1) {
            viewer->AspectratioY = (float)winsize[1]/(float)winsize[0];
            viewer->AspectratioX = 1;
          }

          viewer->delta = SbVec3f(0,0,0);
          viewer->PRESSED = 1;

        } // LineSet end
      else viewer->PRESSED = 0;
    } // point end
    else viewer->PRESSED = 0;

    eventCB->setHandled();
    cout << "HANDLED\n";
  }

  //Handle ungrabbing
  if(mbe->getButton() == SoMouseButtonEvent::BUTTON1 && mbe->getState() == SoButtonEvent::UP) {
    if (viewer->PRESSED) {
      cout << "Ungrab\n";
      Tube<Vector3>* bez_tub;
      SoSeparator* sep = new SoSeparator;

      for (int i=0;i<viewer->ci->info.Knot->tubes();i++) {
        bez_tub = viewer->ci->knot_shape[i]->getKnot();
        viewer->ci->knot_shape[i]->updateMesh(viewer->ci->info.Tol);
        bez_tub->make_default();
        addBezierCurve(sep,bez_tub,viewer->vi->BackGroundFlag);
      }
      viewer->scene->replaceChild(1,sep);
      
      // XXX notify pp,pt,tt plot windows that curve
      // has changed
			viewer->emitChanged();
    }
 
    viewer->PRESSED = 0;
    eventCB->setHandled();
  }
}



MainWindow::MainWindow(QWidget *parent, const char *name,
                       SbBool embed, SoQtFullViewer::BuildFlag flag,
                       SoQtViewer::Type type) :
                         SoQtExaminerViewer(parent,name,
                                            embed,flag,type) {

  vi = new ViewerInfo;
  ci = new CurveInterface; 

  oldx2 = oldy2 = oldx = oldy = -1;
  setCentralWidget(parent);

  view_mode = SOLID_VIEW;
  PRESSED = 0;
  EditTangent = 0;
  vi->ResamplePartFlag = 0;

  // Init default gradient
  gradient = map_color_sine_end;

  pp_win = NULL;

  root = new SoSeparator; root->ref();
  circles = new SoSeparator; circles->ref();
  interaction = new SoSeparator; interaction->ref();
  scene = new SoSwitch; scene->ref();
  scene->whichChild = 0; // default is knot

  root->addChild(interaction);
	root->addChild(scene);
  // add 2 separators. first for mesh, second for biarc view
  scene->addChild(new SoSeparator);
  scene->addChild(new SoSeparator);

	root->addChild(circles);

	// Create event handler for mouse 
	SoEventCallback *mouseEvent = new SoEventCallback; 
	mouseEvent->addEventCallback(SoMouseButtonEvent::getClassTypeId(), mousefunc, this); 
	interaction->addChild(mouseEvent);

  // Create event handler for mouse motion
  SoEventCallback *motionEvent = new SoEventCallback;
  motionEvent->addEventCallback(SoLocation2Event::getClassTypeId(), motionfunc, this);
  interaction->addChild(motionEvent);

  // Init custom file dialog
/*
  fileDialog = new QFileDialog(this);
  fileDialog->setFileMode(QFileDialog::AnyFile);
  fileDialog->setNameFilter(tr("Curve file (*.pkf)"));
  fileDialog->setDirectory(tr("$LIBBIARC"));
  fileDialog->setViewMode(QFileDialog::Detail);
*/

  createActions();
  createMenus();
//  createToolBars();
  createStatusBar();

 //   readSettings();
/*
    connect(textEdit->document(), SIGNAL(contentsChanged()),
            this, SLOT(documentWasModified()));

    setCurrentFile("");
*/
}

MainWindow::~MainWindow() {
  root->removeAllChildren();
  root->unref();
  root->unref();
  delete vi;
  delete ci;
  delete fileMenu;
  delete editMenu;
	delete prefsMenu;
  delete gradMenu;
  delete framingMenu;
	delete addMenu;
  delete helpMenu;
  delete fileToolBar;
  delete editToolBar;
  delete newAct;
  delete openAct;
  delete saveAct;
  delete saveAsAct;
  delete exitAct;
  delete cutAct;
  delete copyAct;
  delete pasteAct;
	
	delete gradientAct;
	delete grad1Act;
	delete grad2Act;
	delete grad3Act;
	delete grad4Act;
	delete grad5Act;
	delete grad6Act;
	delete grad7Act;
	delete grad8Act;
	delete grad9Act;

  delete framingNoneAct;
  delete framingFrenetAct;
  delete framingFrenetFourierAct;
  delete framingParallelAct;
  delete framingParallelODEAct;
  delete framingWritheAct;
  delete framingWritheODEAct;

  delete plotPPWindowAct;
  delete plotPTWindowAct;
  delete plotTTWindowAct;

  delete addIVSceneAct;
//	delete addContactSurfaceAct;

  delete aboutAct;
  delete aboutQtAct;
  delete fileDialog;
}

void MainWindow::closeEvent(QCloseEvent *event) {
  if (maybeSave()) {
    writeSettings();
    event->accept();
  }
  else {
    event->ignore();
  }
}

void MainWindow::newFile() {
  if (ci->graph_node!=NULL) {
    cout << "REMOVE ALL\n";
    scene->removeAllChildren();
    // Set 2 empty Separators (0=mesh,1=biarcview)
    scene->addChild(new SoSeparator);
    scene->addChild(new SoSeparator);
    ci->graph_node = NULL;
  }
  ci->info.Knot->clear_tb();
  ci->info.Closed = 0;

  /*
  if (maybeSave()) {
  //      textEdit->clear();
    setCurrentFile("");
  }
  */
}

void MainWindow::open() {
//  if (maybeSave()) {
  loadFile(QFileDialog::getOpenFileNames(
                         this,
                         "Select one or more files to open",
                         NULL,
                         "Curve file(s) (*.pkf *.xyz)"));
/*
  if (fileDialog->exec())
    loadFile(fileDialog->selectedFiles());
  else
    cerr << "[Warn] MainWindow::open : Files selection cancelled.\n";
*/
// }
}

void MainWindow::addIVScene() {
  QStringList fileNames = QFileDialog::getOpenFileNames(this, "Select one or more files to open", NULL, "IV file(s) (*.iv)");
	SoInput iv_in;
  for (int i = 0; i < fileNames.size(); ++i) {
	  cout << "Read file " <<  fileNames.at(i).toLocal8Bit().constData() << endl;
	  if (iv_in.openFile((char*)(fileNames.at(i).toLocal8Bit().constData()))) {
		  SoSeparator *external_iv = SoDB::readAll(&iv_in);
  		if (external_iv) root->addChild(external_iv);
	  	iv_in.closeFile();
  	}
	  else {
		  cerr << "[Warning] " << fileNames.at(i).toLocal8Bit().constData() << " IV file Problem. Skipped\n";
	  }
	}
}

bool MainWindow::save() {
  /*
  if (curFile.isEmpty()) {
    return saveAs();
  }
  else {
    return saveFile(curFile);
  }
  */
  return 1;
}

bool MainWindow::saveAs() {
  /*
  QString fileName = QFileDialog::getSaveFileName(this);
  if (fileName.isEmpty())
    return false;

  return saveFile(fileName);
  */
  return 1;
}

void MainWindow::about() {
  QMessageBox::about(this, tr("About Application"),
            tr("The <b>Application</b> example demonstrates how to "
               "write modern GUI applications using Qt, with a menu bar, "
               "toolbars, and a status bar."));
}

void MainWindow::setGrad1() { gradient = map_color_rainbow; emit changed(); }
void MainWindow::setGrad2() { gradient = map_color_rainbow_cycle; emit changed(); }
void MainWindow::setGrad3() { gradient = map_color_fine; emit changed(); }
void MainWindow::setGrad4() { gradient = map_color_sine; emit changed(); }
void MainWindow::setGrad5() { gradient = map_color_sine_end; emit changed(); }
void MainWindow::setGrad6() { gradient = map_color_rainbow_fast; emit changed(); }
void MainWindow::setGrad7() { gradient = map_color_sine_acc; emit changed(); }
void MainWindow::setGrad8() { gradient = height_map; emit changed(); }
void MainWindow::setGrad9() { gradient = map_bw; emit changed(); }

void MainWindow::setFraming0() { setFraming(0); }
void MainWindow::setFraming1() { setFraming(1); }
void MainWindow::setFraming2() { setFraming(2); }
void MainWindow::setFraming3() { setFraming(3); }
void MainWindow::setFraming4() { setFraming(4); }
void MainWindow::setFraming5() { setFraming(5); }
void MainWindow::setFraming6() { setFraming(6); }

void MainWindow::ppWindow() { plotWindow(0); }
void MainWindow::ptWindow() { plotWindow(1); }
void MainWindow::ttWindow() { plotWindow(2); }

void MainWindow::plotWindow(int w = 0) {
  switch(w) {
  case 0:
    if (!pp_win) {
      pp_win = new PPPlotWindow(this,NULL,"pp Plot");
      pp_win->setAttribute(Qt::WA_NoBackground);
      pp_win->setWindowTitle("pp Plot");
      QObject::connect(pp_win,SIGNAL(pos_changed(float,float,float,float,bool)),
                       this,SLOT(updatePickedPP(float,float,float,float,bool)));
      QObject::connect(this,SIGNAL(changed()),
                       pp_win,SLOT(recompute()));
    }
    if (pp_win->isVisible()) pp_win->hide();
    else pp_win->show();
    break;
  case 1:
    if (!pt_win) {
      pt_win = new PTPlotWindow(this,NULL,"pt Plot");
      pt_win->setAttribute(Qt::WA_NoBackground);
      pt_win->setWindowTitle("pt Plot");
      QObject::connect(pt_win,SIGNAL(pos_changed(float,float,float,float,bool)),
                       this,SLOT(updatePickedPT(float,float,float,float,bool)));
      QObject::connect(this,SIGNAL(changed()),
                       pt_win,SLOT(recompute()));
    }
    if (pt_win->isVisible()) pt_win->hide();
    else pt_win->show();
    break;
  case 2:
    if (!tt_win) {
      tt_win = new TTPlotWindow(this,NULL,"tt Plot");
      tt_win->setAttribute(Qt::WA_NoBackground);
      tt_win->setWindowTitle("tt Plot");
      QObject::connect(tt_win,SIGNAL(pos_changed(float,float,float,float,bool)),
                       this,SLOT(updatePickedTT(float,float,float,float,bool)));
      QObject::connect(this,SIGNAL(changed()),
                       tt_win,SLOT(recompute()));
    }
    if (tt_win->isVisible()) tt_win->hide();
    else tt_win->show(); 
    break;
  }
}

void MainWindow::setFraming(int FRAME = 0) {
  if (ci->frame_node) {
    scene->removeChild(ci->frame_node);
    ci->frame_node = NULL;
  }
  if (FRAME==0) return;
  for (int i=0;i<ci->info.Knot->tubes();++i) {
    ci->frame_node = ci->frame(FRAME-1);
    scene->addChild(ci->frame_node);
  }
  scene->whichChild.setValue(SO_SWITCH_ALL); // only frenet frame
}

void MainWindow::createActions() {

  newAct = new QAction(QIcon(":/images/new.png"), tr("&New"), this);
  newAct->setShortcut(tr("Ctrl+N"));
  newAct->setStatusTip(tr("Clear scene"));
  connect(newAct, SIGNAL(triggered()), this, SLOT(newFile()));

  openAct = new QAction(QIcon(":/images/open.png"), tr("&Open..."), this);
  openAct->setShortcut(tr("Ctrl+O"));
  openAct->setStatusTip(tr("Open an existing file"));
  connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

  saveAct = new QAction(QIcon(":/images/save.png"), tr("&Save"), this);
  saveAct->setShortcut(tr("Ctrl+S"));
  saveAct->setStatusTip(tr("Save the document to disk"));
  connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));

  saveAsAct = new QAction(tr("Save &As..."), this);
  saveAsAct->setStatusTip(tr("Save the document under a new name"));
  connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

  exitAct = new QAction(tr("E&xit"), this);
  exitAct->setShortcut(tr("Ctrl+Q"));
  exitAct->setStatusTip(tr("Exit the application"));
  connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

  cutAct = new QAction(QIcon(":/images/cut.png"), tr("Cu&t"), this);
  cutAct->setShortcut(tr("Ctrl+X"));
  cutAct->setStatusTip(tr("Cut the current selection's contents to the "
                          "clipboard"));
  // connect(cutAct, SIGNAL(triggered()), textEdit, SLOT(cut()));

  copyAct = new QAction(QIcon(":/images/copy.png"), tr("&Copy"), this);
  copyAct->setShortcut(tr("Ctrl+C"));
  copyAct->setStatusTip(tr("Copy the current selection's contents to the "
                           "clipboard"));
  // connect(copyAct, SIGNAL(triggered()), textEdit, SLOT(copy()));

  pasteAct = new QAction(QIcon(":/images/paste.png"), tr("&Paste"), this);
  pasteAct->setShortcut(tr("Ctrl+V"));
  pasteAct->setStatusTip(tr("Paste the clipboard's contents into the current "
                            "selection"));
  // connect(pasteAct, SIGNAL(triggered()), textEdit, SLOT(paste()));

  grad1Act = new QAction(tr("&Rainbow"), this);
	grad1Act->setCheckable(true);
  grad2Act = new QAction(tr("&Rainbow (cycl)"), this);
	grad2Act->setCheckable(true);
  grad3Act = new QAction(tr("&Fine"), this);
	grad3Act->setCheckable(true);
  grad4Act = new QAction(tr("&Sine"), this);
	grad4Act->setCheckable(true);
  grad5Act = new QAction(tr("&Sine Details"), this);
	grad5Act->setCheckable(true);
  grad6Act = new QAction(tr("&Rainbow (fast)"), this);
	grad6Act->setCheckable(true);
  grad7Act = new QAction(tr("&Sine (fast)"), this);
	grad7Act->setCheckable(true);
  grad8Act = new QAction(tr("&Height map (b/w)"), this);
	grad8Act->setCheckable(true);
  grad9Act = new QAction(tr("&B/W contours"), this);
	grad9Act->setCheckable(true);

  gradientAct = new QActionGroup(this);
	gradientAct->addAction(grad1Act);
	gradientAct->addAction(grad2Act);
	gradientAct->addAction(grad3Act);
	gradientAct->addAction(grad4Act);
	gradientAct->addAction(grad5Act);
	gradientAct->addAction(grad6Act);
	gradientAct->addAction(grad7Act);
	gradientAct->addAction(grad8Act);
	gradientAct->addAction(grad9Act);
	grad5Act->setChecked(true);

	// XXX connect gradients
  connect(grad1Act, SIGNAL(triggered()), this, SLOT(setGrad1()));
  connect(grad2Act, SIGNAL(triggered()), this, SLOT(setGrad2()));
  connect(grad3Act, SIGNAL(triggered()), this, SLOT(setGrad3()));
  connect(grad4Act, SIGNAL(triggered()), this, SLOT(setGrad4()));
  connect(grad5Act, SIGNAL(triggered()), this, SLOT(setGrad5()));
  connect(grad6Act, SIGNAL(triggered()), this, SLOT(setGrad6()));
  connect(grad7Act, SIGNAL(triggered()), this, SLOT(setGrad7()));
  connect(grad8Act, SIGNAL(triggered()), this, SLOT(setGrad8()));
  connect(grad9Act, SIGNAL(triggered()), this, SLOT(setGrad9()));

  plotPPWindowAct = new QAction(tr("pp plot"), this);
  plotPTWindowAct = new QAction(tr("pt plot"), this);
  plotTTWindowAct = new QAction(tr("tt plot"), this);
  connect(plotPPWindowAct, SIGNAL(triggered()), this, SLOT(ppWindow()));
  connect(plotPTWindowAct, SIGNAL(triggered()), this, SLOT(ptWindow()));
  connect(plotTTWindowAct, SIGNAL(triggered()), this, SLOT(ttWindow()));

  framingNoneAct = new QAction(tr("Disable"), this);
  framingNoneAct->setCheckable(true);
  framingFrenetAct = new QAction(tr("Frenet"), this);
  framingFrenetAct->setCheckable(true);
  framingFrenetFourierAct = new QAction(tr("Frenet (Fourier)"), this);
  framingFrenetFourierAct->setCheckable(true);
  framingParallelAct = new QAction(tr("Parallel Transport"), this);
  framingParallelAct->setCheckable(true);
  framingParallelODEAct = new QAction(tr("Parallel ODE"), this);
  framingParallelODEAct->setCheckable(true);
  framingWritheAct = new QAction(tr("Writhe"), this);
  framingWritheAct->setCheckable(true);
  framingWritheODEAct = new QAction(tr("Writhe ODE"), this);
  framingWritheODEAct->setCheckable(true);
  framingAct = new QActionGroup(this);
  framingAct->addAction(framingNoneAct);
  framingAct->addAction(framingFrenetAct);
  framingAct->addAction(framingFrenetFourierAct);
  framingAct->addAction(framingParallelAct);
  framingAct->addAction(framingParallelODEAct);
  framingAct->addAction(framingWritheAct);
  framingAct->addAction(framingWritheODEAct);
  framingNoneAct->setChecked(true);

  connect(framingNoneAct, SIGNAL(triggered()), this, SLOT(setFraming0()));
  connect(framingFrenetAct, SIGNAL(triggered()), this, SLOT(setFraming1()));
  connect(framingFrenetFourierAct, SIGNAL(triggered()), this, SLOT(setFraming2()));
  connect(framingParallelAct, SIGNAL(triggered()), this, SLOT(setFraming3()));
  connect(framingParallelODEAct, SIGNAL(triggered()), this, SLOT(setFraming4()));
  connect(framingWritheAct, SIGNAL(triggered()), this, SLOT(setFraming5()));
  connect(framingWritheODEAct, SIGNAL(triggered()), this, SLOT(setFraming6()));

  addIVSceneAct = new QAction(tr("Add Inventor Scene(s)"), this);
	connect(addIVSceneAct, SIGNAL(triggered()), this, SLOT(addIVScene()));
//  addContactSurfaceAct = new QAction(tr("Compute contact surface"), this);
	// connect(addContactSurfaceAct, SIGNAL(triggered()), this, SLOT(XXX))

  aboutAct = new QAction(tr("&About"), this);
  aboutAct->setStatusTip(tr("Show the application's About box"));
  connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

  aboutQtAct = new QAction(tr("About &Qt"), this);
  aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
  connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

  cutAct->setEnabled(false);
  copyAct->setEnabled(false);
  //connect(textEdit, SIGNAL(copyAvailable(bool)),
  //        cutAct, SLOT(setEnabled(bool)));
  //connect(textEdit, SIGNAL(copyAvailable(bool)),
  //        copyAct, SLOT(setEnabled(bool)));
}

void MainWindow::createMenus() {
  fileMenu = menuBar()->addMenu(tr("&File"));
  fileMenu->addAction(newAct);
  fileMenu->addAction(openAct);
  fileMenu->addAction(saveAct);
  fileMenu->addAction(saveAsAct);
  fileMenu->addSeparator();
  fileMenu->addAction(exitAct);

  editMenu = menuBar()->addMenu(tr("&Edit"));
  editMenu->addAction(cutAct);
  editMenu->addAction(copyAct);
  editMenu->addAction(pasteAct);

  prefsMenu = menuBar()->addMenu(tr("&Preferences"));
	gradMenu = prefsMenu->addMenu(tr("&Color gradient"));
	gradMenu->addAction(grad1Act);
	gradMenu->addAction(grad2Act);
	gradMenu->addAction(grad3Act);
	gradMenu->addAction(grad4Act);
	gradMenu->addAction(grad5Act);
	gradMenu->addAction(grad6Act);
	gradMenu->addAction(grad7Act);
	gradMenu->addAction(grad8Act);
	gradMenu->addAction(grad9Act);

  framingMenu = prefsMenu->addMenu(tr("&Framings"));
  framingMenu->addAction(framingNoneAct);
  framingMenu->addAction(framingFrenetAct);
  framingMenu->addAction(framingFrenetFourierAct);
  framingMenu->addAction(framingParallelAct);
  framingMenu->addAction(framingParallelODEAct);
  framingMenu->addAction(framingWritheAct);
  framingMenu->addAction(framingWritheODEAct);

  plotMenu = menuBar()->addMenu(tr("2D P&lots"));
  plotMenu->addAction(plotPPWindowAct);
  plotMenu->addAction(plotPTWindowAct);
  plotMenu->addAction(plotTTWindowAct);

  addMenu = menuBar()->addMenu(tr("&Add"));
	addMenu->addAction(addIVSceneAct);
//	addMenu->addAction(addContactSurfaceAct);

  menuBar()->addSeparator();

  helpMenu = menuBar()->addMenu(tr("&Help"));
  helpMenu->addAction(aboutAct);
  helpMenu->addAction(aboutQtAct);
}

void MainWindow::createToolBars() {
  fileToolBar = addToolBar(tr("File"));
  fileToolBar->addAction(newAct);
  fileToolBar->addAction(openAct);
  fileToolBar->addAction(saveAct);

  editToolBar = addToolBar(tr("Edit"));
  editToolBar->addAction(cutAct);
  editToolBar->addAction(copyAct);
  editToolBar->addAction(pasteAct);
}

void MainWindow::createStatusBar() {
  statusBar()->showMessage(tr("Ready"));
}

void MainWindow::readSettings() {
  QSettings settings("Trolltech", "Application Example");
  QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
  QSize size = settings.value("size", QSize(400, 400)).toSize();
  resize(size);
  move(pos);
}

void MainWindow::writeSettings() {
  QSettings settings("Trolltech", "Application Example");
  settings.setValue("pos", pos());
  settings.setValue("size", size());
}

bool MainWindow::maybeSave() {/*
  if (textEdit->document()->isModified()) {
    int ret = QMessageBox::warning(this, tr("Application"),
                   tr("The document has been modified.\n"
                      "Do you want to save your changes?"),
                   QMessageBox::Yes | QMessageBox::Default,
                   QMessageBox::No,
                   QMessageBox::Cancel | QMessageBox::Escape);
    if (ret == QMessageBox::Yes)
      return save();
    else if (ret == QMessageBox::Cancel)
      return false;
  }
*/
  return true;
}

void MainWindow::loadFile(const QStringList fileNames) {
  if (ci->graph_node!=NULL) {
    scene->removeAllChildren();
    ci->graph_node = NULL;
		// Set 2 empty Separators (0=mesh,1=biarcview)
		scene->addChild(new SoSeparator);
		scene->addChild(new SoSeparator);
  }
  ci->info.filenames = fileNames;
  ci->graph_node = ci->load();
  if (ci->graph_node!=NULL)
    scene->replaceChild(0,ci->graph_node);
  else
    view_mode = BIARC_VIEW;
  this->getCamera()->viewAll(root, getViewportRegion());
/*
  if (!file.open(QFile::ReadOnly | QFile::Text)) {
  QMessageBox::warning(this, tr("Application"),
    tr("Cannot read file %1:\n%2.")
    .arg(fileName)
    .arg(file.errorString()));
*/

/*
    QTextStream in(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    textEdit->setPlainText(in.readAll());
    QApplication::restoreOverrideCursor();
*/
//  setCurrentFile(fileName);
  statusBar()->showMessage(tr("File(s) loaded"), 2000);
}

bool MainWindow::saveFile(const QString &fileName) {
  QFile file(fileName);
  if (!file.open(QFile::WriteOnly | QFile::Text)) {
    QMessageBox::warning(this, tr("Application"),
                               tr("Cannot write file %1:\n%2.")
                               .arg(fileName)
                               .arg(file.errorString()));
      return false;
    }
/*
  QTextStream out(&file);
  QApplication::setOverrideCursor(Qt::WaitCursor);
  out << textEdit->toPlainText();
  QApplication::restoreOverrideCursor();
*/
  // setCurrentFile(fileName);
  statusBar()->showMessage(tr("File saved"), 2000);
  return true;
}

// Other

// XXX knot length problem!!! plots in [0,1], curve length not necessarily
// u,v in [0,1]
void MainWindow::updatePickedPT(float u, float v, float u2, float v2, bool UNION) {
	cout << "u,v,u2,v2" << u << " " << v << " " << u2 << " " << v2 << endl;
	if (!UNION)
    circles->removeAllChildren();

  Vector3 q0, q1, t0;
  if (u2<0 && v2<0) {
		q0 = (*(ci->info.Knot))[0].pointAt(u);
		q1 = (*(ci->info.Knot))[0].pointAt(v);
		t0 = (*(ci->info.Knot))[0].tangentAt(u);
    circles->addChild(drawCircle(q0,t0,q1,vi->BackGroundFlag));
	}
  else {
		// Make cross as coordinate sys if u2>u
		float dx;
		if (u2>u) {
			dx = (u2-u)/9.;
			for (int i=0;i<10;++i) {
				q0 = (*(ci->info.Knot))[0].pointAt(u+dx*(float)i);
				q1 = (*(ci->info.Knot))[0].pointAt((v2+v)*.5);
				t0 = (*(ci->info.Knot))[0].tangentAt(u+dx*(float)i);
        circles->addChild(drawCircle(q0,t0,q1,vi->BackGroundFlag));
			}
		}

		if (v2>v) {
			dx = (v2-v)/9.;
			for (int i=1;i<10;++i) {
				q0 = (*(ci->info.Knot))[0].pointAt((u2+u)*.5);
				q1 = (*(ci->info.Knot))[0].pointAt(v+dx*(float)i);
				t0 = (*(ci->info.Knot))[0].tangentAt((u2+u)*.5);
        circles->addChild(drawCircle(q0,t0,q1,vi->BackGroundFlag));
			}
		}
  }
}

void MainWindow::updatePickedPP(float u, float v, float u2, float v2, bool UNION) {
  // sort the values s.t u<u2 and v<v2
	
	cout << "u,v,u2,v2" << u << " " << v << " " << u2 << " " << v2 << endl;
  if (!UNION)
    circles->removeAllChildren();

	SoLineSet *ls = new SoLineSet;
	SoCoordinate3 *coords = new SoCoordinate3;
 	int pos = 0;

  Vector3 vec1,vec2;
  if (u2<0 && v2<0) {
	  ls->numVertices.set1Value(0, 2);
		vec1 = (*(ci->info.Knot))[0].pointAt(u);
		vec2 = (*(ci->info.Knot))[0].pointAt(v);
	  coords->point.set1Value(0,SbVec3f(vec1[0],vec1[1],vec1[2]));
	  coords->point.set1Value(1,SbVec3f(vec2[0],vec2[1],vec2[2]));
	}
  else {
    // Make cross as coordinate sys if u2>u
	  float dx;
	  if (u2>u) {
  	  dx = (u2-u)/9.;
	    for (int i=0;i<10;++i) {
   	    ls->numVertices.set1Value(pos, 2);
				vec1 = (*(ci->info.Knot))[0].pointAt(u+dx*(float)i);
				vec2 = (*(ci->info.Knot))[0].pointAt((v2+v)*.5);
				cout << vec1 << " => " << vec2 << endl;
  	    coords->point.set1Value(2*pos+0,SbVec3f(vec1[0],vec1[1],vec1[2]));
	      coords->point.set1Value(2*pos+1,SbVec3f(vec2[0],vec2[1],vec2[2]));
		    pos++;
  	  }
	  }

    if (v2>v) {
      dx = (v2-v)/9.;
      for (int i=1;i<10;++i) {
        ls->numVertices.set1Value(pos, 2);
				vec1 = (*(ci->info.Knot))[0].pointAt((u2+u)*.5);
				vec2 = (*(ci->info.Knot))[0].pointAt(v+dx*(float)i);
        coords->point.set1Value(2*pos+0,SbVec3f(vec1[0],vec1[1],vec1[2]));
        coords->point.set1Value(2*pos+1,SbVec3f(vec2[0],vec2[1],vec2[2]));
        pos++;
      }
    }
  }

	circles->addChild(coords);
	circles->addChild(ls);
}

SoSeparator* addTTSphere(const Vector3 &q0, const Vector3 &t0, const Vector3 &q1, const Vector3 &t1) {
	Vector3 d = q1-q0;
	float dlen = d.norm();
	Vector3 e = d/dlen;
	Vector3 t1star = t1.reflect(e);
	float val = dlen*.5/(t0.cross(t1)).dot(e);

	// compute radius
	SoSphere* sphere = new SoSphere;
	float rad = fabsf(val*sqrt(1-pow(t1star.dot(t0),2)));
	sphere->radius = rad;

	// compute centre
	Vector3 centre = q0 + val*t1star.cross(t0);
	SoTranslation* trans = new SoTranslation;
	trans->translation.setValue(centre[0],centre[1],centre[2]);

	SoSeparator* sep = new SoSeparator;

	SoComplexity *comp = new SoComplexity;
	comp->value = 1.0;

	SoMaterial *mat = new SoMaterial;
//	mat->transparency = 0.7;

	sep->addChild(mat);
	sep->addChild(comp);

	sep->addChild(trans);
	sep->addChild(sphere);
  return sep;
}

void MainWindow::updatePickedTT(float u, float v, float u2, float v2, bool UNION) {
  // sort the values s.t u<u2 and v<v2
	
  // XXX Fixme Do this like PP updatePicked, generate a "coord system"
	// between u->u2 and v->v2 at half height (cross of spanning balls)

	cout << "u,v,u2,v2" << u << " " << v << " " << u2 << " " << v2 << endl;
	if (!UNION)
    circles->removeAllChildren();

  Vector3 q0, q1, t0, t1;
  if (u2<0 && v2<0) {

		q0 = (*(ci->info.Knot))[0].pointAt(u);
		q1 = (*(ci->info.Knot))[0].pointAt(v);
		t0 = (*(ci->info.Knot))[0].tangentAt(u);
		t1 = (*(ci->info.Knot))[0].tangentAt(v);

		circles->addChild(addTTSphere(q0,t0,q1,t1));
	}
  else {
		// Make cross as coordinate sys if u2>u
		float dx;
		if (u2>u) {
			dx = (u2-u)/9.;
			for (int i=0;i<10;++i) {
				q0 = (*(ci->info.Knot))[0].pointAt(u+dx*(float)i);
				q1 = (*(ci->info.Knot))[0].pointAt((v2+v)*.5);
				t0 = (*(ci->info.Knot))[0].tangentAt(u+dx*(float)i);
				t1 = (*(ci->info.Knot))[0].tangentAt((v2+v)*.5);
				circles->addChild(addTTSphere(q0,t0,q1,t1));
			}
		}

		if (v2>v) {
			dx = (v2-v)/9.;
			for (int i=1;i<10;++i) {
				q0 = (*(ci->info.Knot))[0].pointAt((u2+u)*.5);
				q1 = (*(ci->info.Knot))[0].pointAt(v+dx*(float)i);
				t0 = (*(ci->info.Knot))[0].tangentAt((u2+u)*.5);
				t1 = (*(ci->info.Knot))[0].tangentAt(v+dx*(float)i);
				circles->addChild(addTTSphere(q0,t0,q1,t1));
			}
		}
  }
}

