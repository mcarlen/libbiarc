#ifndef __QT_WIDGETS_H__
#define __QT_WIDGETS_H__

#include <qwidget.h>
#include <qimage.h>
#include <qpopupmenu.h>
#include <qfiledialog.h>
#include <qstrlist.h>
#include <qpixmap.h>
#include <qmessagebox.h>

class QLabel;
class QMenuBar;

class Aux2DPlotWindow: public QWidget {
  Q_OBJECT
public:
  Aux2DPlotWindow(QWidget *parent=0, const char *name=0, int wFlags=0);
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
  QPopupMenu *file;
  QPopupMenu *extras;
  int e1,e2,e3,oe1,oe2;

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

class ImageIconProvider : public QFileIconProvider {
  Q_OBJECT
    QStrList fmts;
  QPixmap imagepm;
public:
  ImageIconProvider( QWidget *parent=0, const char *name=0 );
  ~ImageIconProvider();
  const QPixmap * pixmap( const QFileInfo &fi );
};

#endif // __QT_WIDGETS_H__
