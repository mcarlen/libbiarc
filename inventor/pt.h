#ifndef __QT_PT_H__
#define __QT_PT_H__

#include <QApplication>
#include <QWidget>
#include <QImage>
#include <QPainter>
#include <QPaintEvent>

#include "../include/Curve.h"
#include "../experimental/pngmanip/plot_funcs.h"
#include "mainwindow.h"

class PTPlotWindow: public QWidget {
Q_OBJECT
public:
  PTPlotWindow(MainWindow* mainWin, QWidget *parent=0, const char *name=0, Qt::WindowFlags wFlags=0);
  ~PTPlotWindow();
protected:
  void paintEvent( QPaintEvent * );
  void resizeEvent( QResizeEvent * );
  void mousePressEvent( QMouseEvent * );
  void mouseReleaseEvent( QMouseEvent * );
  void mouseMoveEvent( QMouseEvent * );
  void keyPressEvent( QKeyEvent * );
private:
  MainWindow* mainwin;
  bool PRESSED, UNION;
  bool convertEvent(QMouseEvent* e, int& x, int& y);
  QImage  image, orig, bkp;
  
  int e1,e2,e3,oe1,oe2;

  int    pickx, picky;
  int    clickx, clicky;
  int    releasex, releasey;
private slots:
  void popcoords();
  void recompute();
signals:
  void pos_changed(float,float,float,float,bool);
};

#endif // __QT_PT_H__
