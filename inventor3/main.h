#include "../include/TubeBundle.h"
#include <Inventor/Qt/SoQt.h>
#include <Inventor/Qt/viewers/SoQtExaminerViewer.h>
#include "SoKnot.h"

#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoCoordinate4.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoNurbsCurve.h>
#include <Inventor/nodes/SoSphere.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoCylinder.h>
#include <Inventor/nodes/SoRotation.h>
#include <Inventor/fields/SoSFRotation.h>
#include <Inventor/nodes/SoQuadMesh.h>
#include <Inventor/nodes/SoShapeHints.h>
#include <Inventor/nodes/SoNormal.h>
#include <Inventor/nodes/SoNormalBinding.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoComplexity.h>
#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/misc/SoChildList.h>
#include <Inventor/nodes/SoTexture2.h>
#include <Inventor/nodes/SoMaterialBinding.h>

/*
 * Inventor interaction struff
 */

#include <Inventor/Qt/devices/SoQtKeyboard.h>
#include <Inventor/nodes/SoEventCallback.h>
//#include <Inventor/events/SoEvent.h>
#include <Inventor/events/SoKeyboardEvent.h>

/* new new new */
//#include <Inventor/events/SoMouseMotionEvent.h>
#include <Inventor/events/SoMouseButtonEvent.h>
#include <Inventor/events/SoLocation2Event.h>
#include <Inventor/actions/SoRayPickAction.h>
#include <Inventor/SoPickedPoint.h>
#include <Inventor/projectors/SbPlaneProjector.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>

/* new new new */
#include "Widgets.h"

#include <qwidget.h>
#include <Inventor/Qt/SoQt.h>
#include <Inventor/Qt/viewers/SoQtExaminerViewer.h>

#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoPointSet.h>
#include <Inventor/nodes/SoDrawStyle.h>


/*
class SoMaterial;
class SbColor;
*/
extern SoMaterial** materials;
extern SbColor ColorTable[];

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
  void update_picked(float u,float v,float u2,float v2);
};

SoSeparator* drawCircle(Vector3 p0, Vector3 t0, Vector3 p1);

