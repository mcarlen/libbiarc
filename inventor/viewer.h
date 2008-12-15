#ifndef _VIEWER_H_
#define _VIEWER_H_

#include "main.h"
#include "utils.h"

class VVV : public QObject, public SoQtExaminerViewer {
  Q_OBJECT
  SOQT_OBJECT_HEADER(VVV, SoQtExaminerViewer);
public:
  VVV(QWidget *parent=NULL, const char *name=NULL, SbBool embed=TRUE, SoQtFullViewer::BuildFlag flag=BUILD_ALL, SoQtViewer::Type type=BROWSER);
// : SoQtExaminerViewer(parent,name,embed,flag,type) { oldx = oldy = -1; }
  ~VVV();
private:
  int oldx,oldy,oldx2,oldy2;
public slots:
  //void update_picked(float u,float v,float u2,float v2);
};

#endif /* _VIEWER_H_ */
