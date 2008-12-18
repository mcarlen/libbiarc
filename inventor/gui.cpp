#include "gui.h"
#include "mainwindow.h"
#include <Inventor/nodes/SoSwitch.h>

extern Aux2DPlotWindow* pl_win;

SbBool myAppEventHandler(void *userData, QEvent *anyevent) {

  QKeyEvent *myKeyEvent;
  MainWindow *viewer = (MainWindow*)userData;

  SoChildList *children = new SoChildList(viewer->scene);
  int child_len;
  Tube<Vector3>* bez_tub, tube_tmp;

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
    	if (viewer->view_mode==BIARC_VIEW) {
	      children->remove(1);
	      for (int i=0;i<viewer->ci->info.Knot->tubes();i++) {
	        bez_tub = viewer->ci->knot_shape[i]->getKnot();
	        bez_tub->make_default();
	        bez_tub->resample(viewer->ci->knot_shape[0]->nodes.getValue());
	        bez_tub->make_default();
	        addBezierCurve((SoSeparator*)(viewer->scene),bez_tub);
	      }
      }

      break;

    case Qt::Key_C:
      viewer->ci->setNumberOfNodes(viewer->ci->knot_shape[0]->nodes.getValue()-10);
    	if (viewer->view_mode==BIARC_VIEW) {
	      children->remove(1);
	      for (int i=0;i<viewer->ci->info.Knot->tubes();i++) {
	        bez_tub = viewer->ci->knot_shape[i]->getKnot();
	        bez_tub->make_default();
	        bez_tub->resample(viewer->ci->knot_shape[0]->nodes.getValue());
	        bez_tub->make_default();
	        addBezierCurve((SoSeparator*)(viewer->scene),bez_tub);
	      }
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
        if (child_len==2)
          viewer->scene->removeChild(1);
      	for (int i=0;i<viewer->ci->info.Knot->tubes();i++) {
	        bez_tub = viewer->ci->knot_shape[i]->getKnot();
      	  bez_tub->make_default();
	        addBezierCurve((SoSeparator*)(viewer->scene),bez_tub);
      	}
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

    case Qt::Key_O:
      //knot_shape[0]->getKnot()->exportPOVFile("knot.pov");
      cout << "[Not Implemented] Current curve is exported to a Povray file knot.pov.\n";
      break;
      
/*
    case Qt::Key_I:
      cout << "Export curve.iv file\n";
      exportIV();
      break;
 */

    case Qt::Key_P:
      if (!pl_win) {
        pl_win = new Aux2DPlotWindow(NULL,"2dwindow");
        pl_win->setAttribute(Qt::WA_NoBackground);
        pl_win->setWindowTitle("2D Window");
        // XXX Screen width hardcoded!
        pl_win->setGeometry(800+8,0,200,200);
        QObject::connect(pl_win,SIGNAL(pos_changed(float,float,float,float)),
                         viewer,SLOT(update_picked(float,float,float,float)));

      }
      if (pl_win->isVisible()) pl_win->hide();
      else pl_win->show();    
      break;

    // Quit program
    case Qt::Key_Q:
      exit(0);

    }

    return TRUE;

  }
  return FALSE;
}

#if XXX // to end

//Mouse motion callback
static void motionfunc(void *data, SoEventCallback *eventCB) {

  if (view_mode==BIARC_VIEW && PRESSED) {
    
    const SoMouseButtonEvent *mbe=(SoMouseButtonEvent* )eventCB->getEvent();
    VVV *viewer = (VVV*)data;
    
    if (EditTangent) {
      SbVec3f loc = spp.project(mbe->getNormalizedPosition(viewer->getViewportRegion())) + delta;
      loc += (LeftVector*(AspectratioX-1.f)*(loc.dot(LeftVector))
               + UpVector*(AspectratioY-1.f)*(loc.dot(UpVector))) ;
      Vector3 editT((float*)&loc[0]);
      picked_biarc->setTangent(editT-picked_biarc->getPoint());
    }
    else {
      SbVec3f loc = spp.project(mbe->getNormalizedPosition(viewer->getViewportRegion())) + delta;
    
      loc += (LeftVector*(AspectratioX-1.f)*(loc.dot(LeftVector))
               + UpVector*(AspectratioY-1.f)*(loc.dot(UpVector))) ;

      picked_biarc->setPoint(Vector3((float*)&loc[0]));
    }

    SoChildList *children = new SoChildList(scene);
    Tube<Vector3>* bez_tub;
    children = scene->getChildren();
    
    for (int i=0;i<Knot->tubes();i++)
      children->remove(0);
    
    for (int i=0;i<Knot->tubes();i++) {
      bez_tub = knot_shape[i]->getKnot();
      bez_tub->make_default();
      addBezierCurve(scene,bez_tub);
    }
  }
}

//Mouse callback
static void mousefunc(void *data, SoEventCallback *eventCB) {

  if (view_mode!=BIARC_VIEW) return;

  VVV *viewer = (VVV*)data;
  Tube<Vector3>* bez_tub;
  const SoMouseButtonEvent *mbe=(SoMouseButtonEvent*)eventCB->getEvent();
  
  //Handle point grabbing
  if(mbe->getButton() == SoMouseButtonEvent::BUTTON1 && mbe->getState() == SoButtonEvent::DOWN) {
    
    SoRayPickAction rp(viewer->getViewportRegion());
    rp.setPoint(mbe->getPosition());
    rp.apply(viewer->getSceneManager()->getSceneGraph());
    
    SoPickedPoint *point = rp.getPickedPoint();

    EditTangent = 0;
    if (point) {

      SoPath *path = point->getPath();
      SoNode *node = path->getTail();

      if(node && node->getTypeId()==SoSphere::getClassTypeId()) {

        Vector3 pp(point->getPoint()[0],
                   point->getPoint()[1],
 	           point->getPoint()[2]);
    
        Vector3 cp;
        int FOUND = 0;

        // FIXME: If we have more than one curve, give this as userData to the callback
        vector<Biarc<Vector3> >::iterator current;
        for (int l=0;l<Knot->tubes();l++) {
          current = knot_shape[l]->getKnot()->begin();
          float Tolerance = (current->getPoint()-current->getNext().getPoint()).norm()/4.0;
          while (current!=knot_shape[l]->getKnot()->end()) {
  	    if ((current->getPoint()-pp).norm()<Tolerance) {
	      cp = current->getPoint();
  	      picked_biarc = current;
              cout << "Picked biarc " << picked_biarc->id() << endl;
	      FOUND = 1;
	      break;
  	    }
	    ++current;
          }
        }

        if (FOUND && ResamplePartFlag) {
          if (!FirstPoint) {
            FirstBiarc = picked_biarc;
            FirstPoint = 1;
          }
           else {
             // XXX only single components!
             bez_tub = knot_shape[0]->getKnot();
             bez_tub->make_default();
             // XXX resample from FirstBiarc to picked_biarc with 10 points
             bez_tub->refine(FirstBiarc,picked_biarc,5);
             bez_tub->make_default();
             for (int i=0;i<scene->getChildren()->getLength();i++)
               scene->getChildren()->remove(0);
             addBezierCurve(scene,bez_tub);
             FirstPoint = 0;
             ResamplePartFlag = 0;
           }
        }
        else if (FOUND) {
          SbViewVolume vv = viewer->getCamera()->getViewVolume();
          SbPlane s(-(vv.getProjectionDirection()), point->getPoint());
          spp = SbPlaneProjector(s);
          spp.setViewVolume(vv);

          // up and left vector according to the current camera position
          SbRotation rot = viewer->getCamera()->orientation.getValue();
          rot.multVec(SbVec3f(1,0,0),LeftVector);
          rot.multVec(SbVec3f(0,1,0),UpVector);

          SbVec2s winsize = viewer->getViewportRegion().getWindowSize();
          AspectratioX = (float)winsize[0]/(float)winsize[1];
          AspectratioY = 1;

          if (AspectratioX < 1) {
            AspectratioY = (float)winsize[1]/(float)winsize[0];
            AspectratioX = 1;
          }

          delta = SbVec3f((float*)&cp[0]) - point->getPoint();

          // If the viewport is not square we have a
          // aspect ratio problem. Meaning, that the vector
          // is actually smaller!
          delta -= (LeftVector*(1.f-1.f/AspectratioX)*(delta.dot(LeftVector))
                     + UpVector*(1.f-1.f/AspectratioY)*(delta.dot(UpVector))) ;

          PRESSED = 1;
        }
      } // Sphere end
      else
        if(node && node->getTypeId()==SoLineSet::getClassTypeId()) {
          SoLineSet *ls = (SoLineSet*)node;
          if (ls->getName() != "datatangents") {
            eventCB->setHandled();
            return;
          }

          EditTangent = 1;

          int sl_idx = ((SoLineDetail*)point->getDetail())->getPoint0()->getCoordinateIndex();
          // XXX only single component
          picked_biarc = (knot_shape[0]->getKnot()->begin()+(sl_idx>>1));
          
          // XXX same code as above!!!
          SbViewVolume vv = viewer->getCamera()->getViewVolume();
          SbPlane s(-(vv.getProjectionDirection()), point->getPoint());
          spp = SbPlaneProjector(s);
          spp.setViewVolume(vv);

          // up and left vector according to the current camera position
          SbRotation rot = viewer->getCamera()->orientation.getValue();
          rot.multVec(SbVec3f(1,0,0),LeftVector);
          rot.multVec(SbVec3f(0,1,0),UpVector);

          SbVec2s winsize = viewer->getViewportRegion().getWindowSize();
          AspectratioX = (float)winsize[0]/(float)winsize[1];
          AspectratioY = 1;

          if (AspectratioX < 1) {
            AspectratioY = (float)winsize[1]/(float)winsize[0];
            AspectratioX = 1;
          }

          delta = SbVec3f(0,0,0);
          PRESSED = 1;

        } // LineSet end
      else PRESSED = 0;
    } // point end
    else PRESSED = 0;

    eventCB->setHandled();
  }

  //Handle ungrabbing
  if(mbe->getButton() == SoMouseButtonEvent::BUTTON1 && mbe->getState() == SoButtonEvent::UP) {
    if (PRESSED) {

      SoChildList *children = new SoChildList(scene);
      Tube<Vector3>* bez_tub;
      children = scene->getChildren();

      for (int i=0;i<Knot->tubes();i++)
        children->remove(0);
       
      for (int i=0;i<Knot->tubes();i++) {
        bez_tub = knot_shape[i]->getKnot();
        bez_tub->make_default();
        addBezierCurve(scene,bez_tub);
      }
    }
       
    PRESSED = 0;
    eventCB->setHandled();
  }
}
#endif
