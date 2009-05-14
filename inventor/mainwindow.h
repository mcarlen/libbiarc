#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QToolBar>
#include <QStatusBar>
#include <QSettings>
#include "main.h"
#include "utils.h"

class QAction;
class QMenu;

enum VIEW_MODE { SOLID_VIEW = 1, WIRE_VIEW, BIARC_VIEW };
class MainWindow : public QMainWindow, public SoQtExaminerViewer {

  Q_OBJECT
  SOQT_OBJECT_HEADER(MainWindow, SoQtExaminerViewer);

public:
  MainWindow(QWidget* parent=NULL, const char *name = NULL,
             SbBool embed = TRUE,
             SoQtFullViewer::BuildFlag flag = BUILD_ALL,
             SoQtViewer::Type = BROWSER);
  ~MainWindow();

  VIEW_MODE view_mode;
  void swap_view() {
    switch(view_mode) {
    case SOLID_VIEW: view_mode = WIRE_VIEW; break;
    case WIRE_VIEW: view_mode = BIARC_VIEW; break;
    case BIARC_VIEW: view_mode = SOLID_VIEW; break;
    default: view_mode = SOLID_VIEW; break;
    }
  }

  ViewerInfo* vi;
  CurveInterface *ci;

  QFileDialog* fileDialog;

  // Important separators in the scene graph
  SoSeparator *root, *circles, *interaction;
  SoSwitch *scene;

protected:
  void closeEvent(QCloseEvent *event);

private slots:
//  void newFile();
  void open();
  bool save();
  bool saveAs();
  void about();

  void updatePicked(float u, float v, float u2, float v2);

private:

  int oldx, oldy, oldx2, oldy2;

  void createActions();
  void createMenus();
  void createToolBars();
  void createStatusBar();
  void readSettings();
  void writeSettings();
  bool maybeSave();
  void loadFile(const QStringList fileNames);
  bool saveFile(const QString &fileName);

  QMenu *fileMenu;
  QMenu *editMenu;
  QMenu *helpMenu;
  QToolBar *fileToolBar;
  QToolBar *editToolBar;
  QAction *newAct;
  QAction *openAct;
  QAction *saveAct;
  QAction *saveAsAct;
  QAction *exitAct;
  QAction *cutAct;
  QAction *copyAct;
  QAction *pasteAct;
  QAction *aboutAct;
  QAction *aboutQtAct;
};

#endif
