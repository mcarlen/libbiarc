#ifndef __PP_H__
#define __PP_H__

#include <QImage>
#include <QPainter>
#include <QPaintEvent>
#include <QWidget>
#include <QApplication>

#include "../include/Curve.h"
#include "../experimental/pngmanip/plot_funcs.h"

class PPPlotWindow: public QWidget {
Q_OBJECT
public:
  PPPlotWindow(Curve<Vector3> *c, QWidget *parent=0, const char *name=0, Qt::WindowFlags wFlags=0);
  ~PPPlotWindow();
protected:
  void paintEvent( QPaintEvent * );
  void resizeEvent( QResizeEvent * );
  void mousePressEvent( QMouseEvent * );
  void mouseReleaseEvent( QMouseEvent * );
  void mouseMoveEvent( QMouseEvent * );
private:
  bool PRESSED;
  bool    convertEvent(QMouseEvent* e, int& x, int& y);
	QImage  orig;
  QImage  image;
  
  int e1,e2,e3,oe1,oe2;

  int    pickx, picky;
  int    clickx, clicky;
  int    releasex, releasey;
  bool   reconvertImage();

private slots:
  void popcoords();
signals:
  void pos_changed(float,float,float,float);
};

#endif // __PP_H__
