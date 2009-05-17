#ifndef _MAIN_H_
#define _MAIN_H_

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
#include <Inventor/nodes/SoSwitch.h>

/*
 * Inventor interaction struff
 */

#include <Inventor/Qt/devices/SoQtKeyboard.h>
#include <Inventor/nodes/SoEventCallback.h>
//#include <Inventor/events/SoEvent.h>
#include <Inventor/events/SoKeyboardEvent.h>

//#include <Inventor/events/SoMouseMotionEvent.h>
#include <Inventor/events/SoMouseButtonEvent.h>
#include <Inventor/events/SoLocation2Event.h>
#include <Inventor/actions/SoRayPickAction.h>
#include <Inventor/SoPickedPoint.h>
#include <Inventor/projectors/SbPlaneProjector.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>

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
extern const uint ColorNum;
extern SbColor ColorTable[];

/*!
  Global variables, configuration and
  viewer setings structure.
*/
struct ViewerInfo {

  // Flags
  int ResamplePartFlag;
  int FirstPoint;
  int BackGroundFlag;
  
  // Other Flags
  uint TEXTURES, IV_SCENE;
//  uint PT_PLOT;
  
  // Qt
  QString ptplot_file, texture_file, iv_file;

};

typedef ViewerInfo ViewerInfo;

#endif /* _MAIN_H_ */
