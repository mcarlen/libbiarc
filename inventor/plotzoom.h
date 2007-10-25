#include <qobject.h>
#include <qimage.h>
#include <qapplication.h>
#include <qpainter.h>
#include <qwidget.h>
#include <qradiobutton.h>
#include <qbuttongroup.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qfiledialog.h>
#include <qpopupmenu.h>
#include <qmenubar.h>

#include <iostream>

// colors.h comes from pngmanip
// so be careful to keep 'em sync'd
#include "../include/Curve.h"

using namespace std;

enum PLOT_TYPE { PT_PLOT = 1, PP_PLOT, TT_PLOT };

typedef struct Pl {
  QImage img;
  float fromx,fromy,tox,toy;
} Pl;

class Plot : public QWidget {
  Q_OBJECT

  int oldfx,oldfy,oldtx,oldty;

  int FileLoaded;
  int mbarh;
  Pl plots[10];
  int Depth;
  Curve<Vector3>* curve;
  QWidget *plotwin;
  int clickx,clicky,movex,movey;
  float fx,fy,tx,ty;
  int w,h;
  float length,thickness,alen,ptmin,ptmax,cur;
  float oldlength,sigma_i,sigma_j;
  // ptplot uses fromx,fromy,tox and toy;
  void ptplot();
  void loadPKF(QString filename);

  // MenuBar
  QMenuBar *menubar;
  QPopupMenu *file;

public:
  int height();
  PLOT_TYPE ptype;
  QButtonGroup *bg;
  ~Plot();
  Plot(const char* pkfname = NULL, int w = 500, int h = 500,
       PLOT_TYPE type = PT_PLOT, QWidget *parent = 0);
  int SIZE;
public slots:
  void test(int id);
  void zoomout();
  void openFile();
protected:
  void paintEvent(QPaintEvent *e);
  void keyReleaseEvent ( QKeyEvent *e);
  void mousePressEvent ( QMouseEvent *e );
  void mouseReleaseEvent (QMouseEvent *e);
  void mouseMoveEvent( QMouseEvent *e);
};

