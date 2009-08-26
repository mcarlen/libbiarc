#include "gui.h"
#include "mainwindow.h"
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/actions/SoGetBoundingBoxAction.h>

SoSeparator* addPrincipalAxis(const Matrix3 &m, const Curve<Vector3> *c) {
  SoSeparator *SteadyNode = new SoSeparator;
  //    SteadyNode->ref();

  SoSeparator **AxisNode    = new SoSeparator*[3];
  // Cylinder
  SoSeparator **CylNode     = new SoSeparator*[3];
  SoCylinder **Cyl          = new SoCylinder*[3];
  SoTranslation **CylTrans  = new SoTranslation*[3];
  SoRotation **CylRot       = new SoRotation*[3];
 
  SbVec3f rot_ax;
  float rot_angle;

  // Init all the nodes
  for (int i=0;i<3;i++) {
    AxisNode[i] = new SoSeparator; CylNode[i] = new SoSeparator;
    Cyl[i] = new SoCylinder; CylTrans[i] = new SoTranslation;
    CylRot[i] = new SoRotation;
  }
  
  SoMaterial** StableAxisMaterial = new SoMaterial*[3];
  StableAxisMaterial[0] = new SoMaterial;
  StableAxisMaterial[0]->diffuseColor.setValue(1,0,0);
  StableAxisMaterial[1] = new SoMaterial;
  StableAxisMaterial[1]->diffuseColor.setValue(0,1,0);
  StableAxisMaterial[2] = new SoMaterial;
  StableAxisMaterial[2]->diffuseColor.setValue(0,0,1);

  float scale = c->span();
  for (int i=0;i<3;i++) {

    // needed general vars
    Vector3 ssvec = m[i];
    ssvec.normalize();
    
	  CylNode[i]->addChild(StableAxisMaterial[i]);
    
    // Cylinder for steady state
    Cyl[i]->radius = (scale/100.0);
    Cyl[i]->height = (scale*.5);
    
    CylTrans[i]->translation.setValue(0,(scale*.25),0);
    Vector3 CylOrientation(0.0,1.0,0.0);
    
    if (ssvec.dot(CylOrientation)<.999f) {
      Vector3 temp;
      temp = ssvec.cross(CylOrientation);
      rot_ax.setValue(temp[0],temp[1],temp[2]);
      rot_angle = -(float)acos(ssvec.dot(CylOrientation));
      CylRot[i]->rotation.setValue(rot_ax,rot_angle);
      
      CylNode[i]->addChild(CylRot[i]);
    }
    
    CylNode[i]->addChild(CylTrans[i]);
    CylNode[i]->addChild(Cyl[i]);
    
    //      AxisNode[i]->addChild(TextNode[i]);
    AxisNode[i]->addChild(CylNode[i]);
    
    // add axis to all axis separator
    SteadyNode->addChild(AxisNode[i]);
    
  }
  return SteadyNode;
}


SbBool myAppEventHandler(void *userData, QEvent *anyevent) {

  QKeyEvent *myKeyEvent;
  MainWindow *viewer = (MainWindow*)userData;

  static bool INERTIA_AXIS = false;
  static SoSeparator* inertiaNode = NULL;
  static int AXIS_VIEW = 0;
  static int FRAME_VIEW = 0;
  static const char* frame_str[] = { "Disabled", "Standard Frenet", "Fourier Frenet", "Parallel", "Parallel ODE", "Writhe", "Writhe ODE" };

  SoChildList *children = new SoChildList(viewer->scene);
  int child_len;
  Tube<Vector3>* bez_tub, tube_tmp;
	SoSeparator* sep;

  SbVec3f CamAxis, CamPos;
  SbRotation CamOrientation;
  SbMatrix sbmat; Matrix3 mat3;

  static int mx,my;
  static int SPECIAL_MOUSE = 0;

  if (SPECIAL_MOUSE && (anyevent->type()==QEvent::MouseMove)) {
    QMouseEvent *mouse = (QMouseEvent*)anyevent;
    cout << "RelMove : " << mouse->x()-mx << " "<< mouse->y()-my << endl;
    mx = mouse->x(); my = mouse->y();
  }

  if(anyevent->type()==QEvent::KeyPress) {

    children = viewer->scene->getChildren();
    child_len = children->getLength();

    myKeyEvent = (QKeyEvent *) anyevent;

    // For new tube
    Tube<Vector3> tube;

    switch(myKeyEvent->key()) {

    case Qt::Key_G:
      SPECIAL_MOUSE = !SPECIAL_MOUSE;
      break;

    // Transparency changing
    case Qt::Key_F:
      viewer->ci->increaseTransparency();
      break;

    case Qt::Key_V:
      viewer->ci->decreaseTransparency();
      break;

    case Qt::Key_A:
      viewer->ci->increaseRadius();
      break;

    case Qt::Key_Y:
    case Qt::Key_Z:
      viewer->ci->decreaseRadius();
      break;

    case Qt::Key_S:
      viewer->ci->increaseSegments();
      break;

    case Qt::Key_X:
      viewer->ci->decreaseSegments();
      break;

    case Qt::Key_D:
      viewer->ci->setNumberOfNodes(viewer->ci->knot_shape[0]->nodes.getValue()+10);
    	// if (viewer->view_mode==BIARC_VIEW) {
				sep = new SoSeparator;
	      for (int i=0;i<viewer->ci->info.Knot->tubes();i++) {
	        bez_tub = viewer->ci->knot_shape[i]->getKnot();
	        bez_tub->make_default();
	        bez_tub->resample(viewer->ci->knot_shape[0]->nodes.getValue());
	        bez_tub->make_default();
	        addBezierCurve(sep,bez_tub,viewer->vi->BackGroundFlag);
	      }
        viewer->scene->replaceChild(1, sep);
      // }
      viewer->ci->knot_shape[0]->getKnot()->make_default();
      viewer->ci->knot_shape[0]->updateMesh(viewer->ci->info.Tol);

      break;

    case Qt::Key_C:
      viewer->ci->setNumberOfNodes(viewer->ci->knot_shape[0]->nodes.getValue()-10);
    	// if (viewer->view_mode==BIARC_VIEW) {
				sep = new SoSeparator;
	      for (int i=0;i<viewer->ci->info.Knot->tubes();i++) {
	        bez_tub = viewer->ci->knot_shape[i]->getKnot();
	        bez_tub->make_default();
	        bez_tub->resample(viewer->ci->knot_shape[0]->nodes.getValue());
	        bez_tub->make_default();
	        addBezierCurve(sep,bez_tub,viewer->vi->BackGroundFlag);
	      }
        viewer->scene->replaceChild(1,sep);
     // }
      viewer->ci->knot_shape[0]->getKnot()->make_default();
      viewer->ci->knot_shape[0]->updateMesh(viewer->ci->info.Tol);
      break;

    case Qt::Key_1:
      FRAME_VIEW = (FRAME_VIEW + 1)%7;
      cout << frame_str[FRAME_VIEW]  << " Frame\n";
      viewer->setFraming(FRAME_VIEW);
      break;

    case Qt::Key_L:
      if (viewer->view_mode==BIARC_VIEW) {
        if (viewer->ci->knot_shape[0]->getKnot()->isClosed()) {
          viewer->ci->knot_shape[0]->getKnot()->unlink();
        }
        else {
          viewer->ci->knot_shape[0]->getKnot()->link();
        }
				sep = new SoSeparator;
	      bez_tub = viewer->ci->knot_shape[0]->getKnot();
	      bez_tub->make_default();
        viewer->ci->knot_shape[0]->updateMesh(viewer->ci->info.Tol);
	      addBezierCurve(sep,bez_tub,viewer->vi->BackGroundFlag);
        viewer->scene->replaceChild(1,sep);
      }
      break;

    case Qt::Key_Return:
      if (viewer->view_mode==BIARC_VIEW) {
        if (viewer->ci->info.Knot->tubes()==0) {
          cout << "NEW CURVE\n";
          tube.append(Vector3(0,0,0),Vector3(1,0,0));
          tube.append(Vector3(1,0,0),Vector3(1,0,0));
          viewer->ci->info.Knot->newTube(tube);
          viewer->ci->info.N = 2;
          viewer->ci->info.R = 0.2;
          viewer->ci->makeMesh();
          viewer->ci->graph_node = viewer->ci->curveSeparator();
          viewer->scene->replaceChild(0, viewer->ci->graph_node);
        }
        else {
          if (viewer->ci->knot_shape[0]->getKnot()->isClosed()) {
            cout << "[Warn] Can not add node to closed curve!\n";
            break;
          }
          Vector3 p = (viewer->ci->knot_shape[0]->getKnot()->end()-1)->getPoint();
          Vector3 t = (viewer->ci->knot_shape[0]->getKnot()->end()-1)->getTangent();
          viewer->ci->knot_shape[0]->getKnot()->append(p+t,t);
          viewer->ci->knot_shape[0]->updateMesh(viewer->ci->info.Tol);
        }
				sep = new SoSeparator;
	      bez_tub = viewer->ci->knot_shape[0]->getKnot();
	      bez_tub->make_default();
	      addBezierCurve(sep,bez_tub,viewer->vi->BackGroundFlag);
        viewer->scene->replaceChild(1,sep);
 
        cout << "ADD NODE\n";
      }
      break;

    case Qt::Key_Delete:
      if (viewer->view_mode==BIARC_VIEW) {
        viewer->ci->knot_shape[0]->getKnot()->remove(viewer->picked_biarc);
	      bez_tub = viewer->ci->knot_shape[0]->getKnot();
      	bez_tub->make_default();
        sep = new SoSeparator;
	      addBezierCurve(sep,bez_tub,viewer->vi->BackGroundFlag);
				viewer->scene->replaceChild(1, sep);
        viewer->ci->knot_shape[0]->updateMesh(viewer->ci->info.Tol);
      }
      break;

    case Qt::Key_Space:
      viewer->swap_view();
      switch(viewer->view_mode) {
      case SOLID_VIEW: 
      	viewer->scene->whichChild.setValue(0);  // 0 is mesh curve
	      viewer->setDrawStyle(SoQtViewer::STILL,
		                         SoQtViewer::VIEW_AS_IS);
        break;
      case WIRE_VIEW:
	      viewer->setDrawStyle(SoQtViewer::STILL,
	                           SoQtViewer::VIEW_HIDDEN_LINE);
	      break;
      case BIARC_VIEW:
	      viewer->setDrawStyle(SoQtViewer::STILL,
	                           SoQtViewer::VIEW_AS_IS);
        // XXX rebuild bezier curve only if N or S changed!!
        sep = new SoSeparator;
        for (int i=0;i<viewer->ci->info.Knot->tubes();i++) {
            bez_tub = viewer->ci->knot_shape[i]->getKnot();
            bez_tub->make_default();
            addBezierCurve(sep,bez_tub,viewer->vi->BackGroundFlag);
        }
        viewer->scene->replaceChild(1, sep);
        viewer->scene->whichChild.setValue(1); // is biarc curve
	      break;

      default: cerr << "View Mode problem. Should not happen\n"; exit(3);
      }
      break;

    //
    case Qt::Key_W:

      CamOrientation = viewer->getCamera()->orientation.getValue();
      CamOrientation.getValue(sbmat);
      mat3[0][0] = sbmat[0][0]; mat3[0][1] = sbmat[1][0]; mat3[0][2] = sbmat[2][0];
      mat3[1][0] = sbmat[0][1]; mat3[1][1] = sbmat[1][1]; mat3[1][2] = sbmat[2][1];
      mat3[2][0] = sbmat[0][2]; mat3[2][1] = sbmat[1][2]; mat3[2][2] = sbmat[2][2];
      
      tube_tmp = *(viewer->ci->knot_shape[0]->getKnot());
      tube_tmp.apply(mat3).writePKF("curve.pkf");
      cout << "Current curve state written to curve.pkf.\n";
      break;

      // TODO TODO
#ifdef RENDERMAN
    case Qt::Key_E:
      // FIXME get camera and lighting info and pass to RIB export
      cout << "Export RIB File (knot.rib)" << flush;
      CamPos = viewer->getCamera()->position.getValue();
      CamOrientation = viewer->getCamera()->orientation.getValue();
      CamOrientation.getValue(CamAxis,CamAngle);
      viewer->ci->knot_shape[0]->getKnot()
                               ->exportRIBFile("knot.rib",320,240,
                                               (Vector3)&CamPos[0],
                                               (Vector3)&CamAxis[0],CamAngle,
             (Vector3)&(viewer->getHeadlight()->direction.getValue())[0]);
      cout << " [OK]\n";
      break;
#else
    case Qt::Key_E:
      cout << "Export RIB File (knot.rib) : No Pixie support compiled in!\n";
      break;
#endif

    /* Start "resample curve between two points" procedure
       This is only working in BIARC_VIEW
    */
    case Qt::Key_R:
      if (viewer->view_mode == BIARC_VIEW) {
        viewer->vi->ResamplePartFlag = 1;
        cout << "Resample a part of the curve!\n";
      }
      break;

/*
    case Qt::Key_O:
      //knot_shape[0]->getKnot()->exportPOVFile("knot.pov");
      cout << "[Not Implemented] Current curve is exported to a Povray file knot.pov.\n";
      break;
			*/
      
/*
    case Qt::Key_I:
      cout << "Export curve.iv file\n";
      exportIV();
      break;
 */

    case Qt::Key_2:
      if (myKeyEvent->modifiers()&Qt::CTRL) {
        viewer->ci->knot_shape[0]->reset();
        Tube<Vector3>* tube = viewer->ci->knot_shape[0]->getKnot();
        tube->principalAxis(mat3);
        for (int i=0;i<AXIS_VIEW;++i) {
          Vector3 vtmp = mat3[0];
          mat3[0] = mat3[1]; mat3[1] = mat3[2];
          mat3[2] = vtmp;
        }
        tube->apply(mat3.transpose());
        CurveInfo& ci = viewer->ci->info;
        tube->clear_tube();
        tube->makeMesh(ci.N,ci.S,ci.R,ci.Tol);
        AXIS_VIEW = (AXIS_VIEW + 1)%3;
      }
      else {
        INERTIA_AXIS = !INERTIA_AXIS;
      }
      if (inertiaNode!=NULL) {
        viewer->root->removeChild(inertiaNode);
        inertiaNode = 0;
      }
      if (INERTIA_AXIS) {
        viewer->ci->knot_shape[0]->getKnot()->principalAxis(mat3);
        inertiaNode = addPrincipalAxis(mat3,viewer->ci->knot_shape[0]->getKnot());
        viewer->root->addChild(inertiaNode);
      }
      if (myKeyEvent->modifiers()&Qt::CTRL) {
        SoGetBoundingBoxAction* act = new SoGetBoundingBoxAction(viewer->getViewportRegion());
        act->apply(viewer->root);
        viewer->scheduleRedraw();
      }
 
      break;

    case Qt::Key_P:
      viewer->plotWindow(1);
      break;

    case Qt::Key_O:
      viewer->plotWindow(0);  
      break;

    case Qt::Key_I:
      viewer->plotWindow(2);  
      break;


    // Quit program
    case Qt::Key_Q:
      QCoreApplication::exit(0);

    }

    return TRUE;

  }
  return FALSE;
}

