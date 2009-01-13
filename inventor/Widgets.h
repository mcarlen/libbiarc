#ifndef __QT_WIDGETS_H__
#define __QT_WIDGETS_H__

#include <QObject>
#include <QApplication>
#include <QWidget>
#include <QImage>
#include <QMenuBar>
#include <QMenu>
#include <QFileDialog>
#include <QStringList>
#include <QPixmap>
#include <QMessageBox>
#include <QFileIconProvider>
#include <QLabel>

#include <QPainter>
#include <QPaintEvent>
#include <QImageReader>

#include <iostream>

#ifdef WITH_ICON_PROVIDER
class ImageIconProvider : public QFileIconProvider {
Q_OBJECT
  QList<QByteArray> fmts;
  QIcon imagepm;
public:
  ImageIconProvider( QWidget *parent=0, const char *name=0 );
  ~ImageIconProvider();
  QIcon icon( const QFileInfo &fi ) const;
};
#endif

class Aux2DPlotWindow: public QWidget {
Q_OBJECT
public:
  Aux2DPlotWindow(QWidget *parent=0, const char *name=0, Qt::WindowFlags wFlags=0);
  ~Aux2DPlotWindow();
  bool loadImage(const QString &);
protected:
  void paintEvent( QPaintEvent * );
  void resizeEvent( QResizeEvent * );
  void mousePressEvent( QMouseEvent * );
  void mouseReleaseEvent( QMouseEvent * );
  void mouseMoveEvent( QMouseEvent * );
private:
  bool    convertEvent(QMouseEvent* e, int& x, int& y);
  QString filename;
  QImage  image;
  QPixmap pm;
  
  QMenuBar *menubar;
  QMenu *file; // ,extras;
  
  QAction *openFileAct, *quitAct;

  int e1,e2,e3,oe1,oe2;

  QFileDialog *fd;
#ifdef WITH_ICON_PROVIDER
  ImageIconProvider *iip;
#endif

  QMessageBox *pop_info;

  QLabel *status;
  void   updateStatus();
  int    pickx, picky;
  int    clickx, clicky;
  int    releasex, releasey;
  // I can probably kick this
  // bool   may_be_other;
  void   setImage(const QImage& newimage);
  bool   reconvertImage();
private slots:
  void openFile();
  void popcoords();
signals:
  void pos_changed(float,float,float,float);
};

#endif // __QT_WIDGETS_H__
