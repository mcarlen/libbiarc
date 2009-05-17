#include "mainwindow.h"

#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoLineSet.h>

SOQT_OBJECT_SOURCE(MainWindow);

MainWindow::MainWindow(QWidget *parent, const char *name,
                       SbBool embed, SoQtFullViewer::BuildFlag flag,
                       SoQtViewer::Type type) :
                         SoQtExaminerViewer(parent,name,
                                            embed,flag,type) {

  oldx2 = oldy2 = oldx = oldy = -1;
  setCentralWidget(parent);

  view_mode = SOLID_VIEW;

  root = new SoSeparator; root->ref();
  circles = new SoSeparator; circles->ref();
  interaction = new SoSeparator; interaction->ref();
  scene = new SoSwitch; scene->ref();
  scene->whichChild = 0; // default is knot

  vi = new ViewerInfo;
  ci = new CurveInterface; 

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

/*
void MainWindow::newFile() {
  if (maybeSave()) {
  //      textEdit->clear();
    setCurrentFile("");
  }
}
*/

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

void MainWindow::createActions() {
  /*
  newAct = new QAction(QIcon(":/images/new.png"), tr("&New"), this);
  newAct->setShortcut(tr("Ctrl+N"));
  newAct->setStatusTip(tr("Create a new file"));
  connect(newAct, SIGNAL(triggered()), this, SLOT(newFile()));
*/
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
  // fileMenu->addAction(newAct);
  fileMenu->addAction(openAct);
  fileMenu->addAction(saveAct);
  fileMenu->addAction(saveAsAct);
  fileMenu->addSeparator();
  fileMenu->addAction(exitAct);

  editMenu = menuBar()->addMenu(tr("&Edit"));
  editMenu->addAction(cutAct);
  editMenu->addAction(copyAct);
  editMenu->addAction(pasteAct);

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
  }
  ci->info.filenames = fileNames;
  ci->graph_node = ci->load();
  scene->addChild(ci->graph_node);
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
//    setCurrentFile(fileName);
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

// u,v in [0,1]
void MainWindow::updatePicked(float u, float v, float u2, float v2) {
	cout << "u,v,u2,v2" << u << " " << v << " " << u2 << " " << v2 << endl;
  circles->removeAllChildren();

  Vector3 q0, q1, t0;
  if (u2<0 && v2<0) {
		q0 = (*(ci->info.Knot))[0].pointAt(u);
		q1 = (*(ci->info.Knot))[0].pointAt(v);
		t0 = (*(ci->info.Knot))[0].tangentAt(u);
    circles->addChild(drawCircle(q0,t0,q1));
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
        circles->addChild(drawCircle(q0,t0,q1));
			}
		}

		if (v2>v) {
			dx = (v2-v)/9.;
			for (int i=1;i<10;++i) {
				q0 = (*(ci->info.Knot))[0].pointAt((u2+u)*.5);
				q1 = (*(ci->info.Knot))[0].pointAt(v+dx*(float)i);
				t0 = (*(ci->info.Knot))[0].tangentAt((u2+u)*.5);
        circles->addChild(drawCircle(q0,t0,q1));
			}
		}
  }
}

void MainWindow::updatePickedPP(float u, float v, float u2, float v2) {
  // sort the values s.t u<u2 and v<v2
	
	cout << "u,v,u2,v2" << u << " " << v << " " << u2 << " " << v2 << endl;
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

void MainWindow::updatePickedTT(float u, float v, float u2, float v2) {
  // sort the values s.t u<u2 and v<v2
	
  // XXX Fixme Do this like PP updatePicked, generate a "coord system"
	// between u->u2 and v->v2 at half height (cross of spanning balls)

	cout << "u,v,u2,v2" << u << " " << v << " " << u2 << " " << v2 << endl;
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

