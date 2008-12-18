#include "mainwindow.h"

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
  ci->info.filenames = fileNames;
  if (ci->graph_node!=NULL) {
    root->removeChild(ci->graph_node);
    ci->graph_node = NULL;
  }
  root->addChild(ci->load());
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

#if 0
// u,v in [0,1]
void VVV::update_picked(float u, float v, float u2, float v2) {
  // FIXME : one component only supported for now
  int x,y,x2,y2,i;
  float a,b;

  cout << "u,v  = " << u << " ," << v << endl;
  cout << "u2,v2 = " << u2 << " ," << v2 << endl;

  // reset highlight to zero
  if (oldx2>=0 && oldy2>=0) {
    for (i=oldx;i<=oldx2;i++)
      materials[0]->diffuseColor.set1Value(i,ColorTable[0]);
    for (i=oldy;i<=oldy2;i++)
      materials[0]->diffuseColor.set1Value(i,ColorTable[0]);
  }
  else {
    if (oldx >= 0)
      materials[0]->diffuseColor.set1Value(oldx,ColorTable[0]);
    if (oldy >= 0)
      materials[0]->diffuseColor.set1Value(oldy,ColorTable[0]);
  }

  if (u2>=0 && v2>=0) {
    x = (*Knot)[0].biarcPos(u); //*(*Knot)[0].length());
    y = (*Knot)[0].biarcPos(v); //*(*Knot)[0].length());
    x2 = (*Knot)[0].biarcPos(u2); //*(*Knot)[0].length());
    y2 = (*Knot)[0].biarcPos(v2); //*(*Knot)[0].length());

    cout << "x,y  = " << x << " , " << y << endl;
    cout << "2x,y = " << x2 << " , " << y2 << endl;

    for (i=x;i<=x2;i++)
      materials[0]->diffuseColor.set1Value(i,ColorTable[1]);
    for (i=y;i<=y2;i++)
      materials[0]->diffuseColor.set1Value(i,ColorTable[1]);


    // sort the values s.t u<u2 and v<v2
    circles->removeAllChildren();
    if (x>x2) { int xtmp = x; x = x2; x2 = xtmp; }
    if (y>y2) { int ytmp = y; y = y2; y2 = ytmp; }
    if (u>u2) { float utmp = u; u = u2; u2 = utmp; }
    if (v>v2) { float vtmp = v; v = v2; v2 = vtmp; }

 //   float NUMX = ((float)(*Knot)[0].nodes())*(u2-u)/(*Knot)[0].length()/5.;
 //   float NUMY = ((float)(*Knot)[0].nodes())*(v2-v)/(*Knot)[0].length()/5.;

    float Nx = (u2-u)/(float)(x2-x);
    // float Ny = (v2-v)/(float)(y2-y);
    for (i=0;i<=(x2-x);i++) {
      a = u+(float)i*Nx;
      //for (j=0;j<=(y2-y);j++) {
      b = v; //+(float)j*Ny;
        circles->addChild(drawCircle((*Knot)[0].pointAt(a),
                                  (*Knot)[0].tangentAt(a),
                                  (*Knot)[0].pointAt(b)));
/*
      b = v+(float)(y2-y)*Ny;
        circles->addChild(drawCircle((*Knot)[0].pointAt(a),
                                  (*Knot)[0].tangentAt(a),
                                  (*Knot)[0].pointAt(b)));
*/
      //}
   }

/*
    for (j=0;j<=(y2-y);j++) {
      b = v+(float)j*Ny;
      a = u;
        circles->addChild(drawCircle((*Knot)[0].pointAt(a),
                                  (*Knot)[0].tangentAt(a),
                                  (*Knot)[0].pointAt(b)));
      a = u+(float)(x2-x)*Ny;
        circles->addChild(drawCircle((*Knot)[0].pointAt(a),
                                  (*Knot)[0].tangentAt(a),
                                  (*Knot)[0].pointAt(b)));
    }
*/

    // reset old values
    if (x<x2 && x>=0) { oldx = x; oldx2 = x2; }
    else { oldx = x2; oldx2 = x; }
    if (y<y2 && y>0) { oldy = y; oldy2 = y2; }
    else { oldy = y2; oldy2 = y; }
  }
  else {
    x = (*Knot)[0].biarcPos(u);//*(*Knot)[0].length());
    y = (*Knot)[0].biarcPos(v);//*(*Knot)[0].length());
    materials[0]->diffuseColor.set1Value(x,ColorTable[1]);
    materials[0]->diffuseColor.set1Value(y,ColorTable[1]);
    oldx = x; oldx2 = -1;
    oldy = y; oldy2 = -1;

    SoSeparator *sep = drawCircle((*Knot)[0].pointAt(u),//*(*Knot)[0].length()),
                                  (*Knot)[0].tangentAt(u),//*(*Knot)[0].length()),
                                  (*Knot)[0].pointAt(v));//*(*Knot)[0].length()));
    //cout << "Add child\n";
    if (circles->getNumChildren()==1)
      circles->replaceChild(0,sep);
    else {
      circles->removeAllChildren();
      circles->addChild(sep);
    }
  }
}
#endif
