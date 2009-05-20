#ifndef __PP_H__
#define __PP_H__

#include <QImage>
#include <QPainter>
#include <QPaintEvent>
#include <QWidget>
#include <QApplication>

#include "../include/Curve.h"
#include "../experimental/pngmanip/plot_funcs.h"
#include "mainwindow.h"

class PPPlotWindow: public QWidget {
Q_OBJECT
public:
  PPPlotWindow(MainWindow* mainWin, QWidget *parent=0, const char *name=0, Qt::WindowFlags wFlags=0);
  ~PPPlotWindow();
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
  bool    convertEvent(QMouseEvent* e, int& x, int& y);
	QImage  orig, image, bkp;
  
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

#endif // __PP_H__
