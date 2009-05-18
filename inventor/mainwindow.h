#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QApplication>
#include <QMainWindow>
#include <QToolBar>
#include <QStatusBar>
#include <QSettings>
#include <QFileDialog>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>

#include "main.h"
#include "utils.h"
#include "../experimental/pngmanip/colors.h"

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

  // Gradient function pointer for plots
	void(*gradient)(RGB*,float,float,float);

  bool PRESSED;
  vector<Biarc<Vector3> >::iterator picked_biarc;
  SbPlaneProjector spp;
  SbVec3f UpVector, LeftVector, delta;
  float AspectratioX, AspectratioY;
  int EditTangent;

	// Partial Resample of the curve globals
	unsigned int ResamplePartFlag;
	unsigned int FirstPoint;
	vector<Biarc<Vector3> >::iterator FirstBiarc;

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

  // XXX That's ugly!
	void setGrad1();
	void setGrad2();
	void setGrad3();
	void setGrad4();
	void setGrad5();
	void setGrad6();
	void setGrad7();
	void setGrad8();
	void setGrad9();

  void updatePickedPP(float u, float v, float u2, float v2, bool UNION);
  void updatePickedPT(float u, float v, float u2, float v2, bool UNION);
  void updatePickedTT(float u, float v, float u2, float v2, bool UNION);

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
  QMenu *prefsMenu;
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

	QActionGroup *gradientAct;
	QAction *grad1Act;
	QAction *grad2Act;
	QAction *grad3Act;
	QAction *grad4Act;
	QAction *grad5Act;
	QAction *grad6Act;
	QAction *grad7Act;
	QAction *grad8Act;
	QAction *grad9Act;

  QAction *aboutAct;
  QAction *aboutQtAct;
};

#endif
