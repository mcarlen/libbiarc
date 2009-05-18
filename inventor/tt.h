#ifndef __TT_H__
#define __TT_H__

#include <QImage>
#include <QPainter>
#include <QPaintEvent>
#include <QWidget>
#include <QApplication>

#include "../include/Curve.h"
#include "../experimental/pngmanip/plot_funcs.h"
#include "mainwindow.h"

class TTPlotWindow: public QWidget {
Q_OBJECT
public:
  TTPlotWindow(MainWindow* mainWin, QWidget *parent=0, const char *name=0, Qt::WindowFlags wFlags=0);
  ~TTPlotWindow();
protected:
  void paintEvent( QPaintEvent * );
  void resizeEvent( QResizeEvent * );
  void mousePressEvent( QMouseEvent * );
  void mouseReleaseEvent( QMouseEvent * );
  void mouseMoveEvent( QMouseEvent * );
	void keyPressEvent( QKeyEvent * );
private:
  void recompute();
	MainWindow* mainwin;
  bool PRESSED, UNION;
  bool    convertEvent(QMouseEvent* e, int& x, int& y);
	QImage  orig, image, bkp;
  
  int e1,e2,e3,oe1,oe2;

  int    pickx, picky;
  int    clickx, clicky;
  int    releasex, releasey;

private slots:
  void popcoords();
signals:
  void pos_changed(float,float,float,float,bool);
};

#endif // __TT_H__
