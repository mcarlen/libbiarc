#include "utils.h"

// General Stuff
int parse(int argc, char** argv, ViewerInfo *vi, CurveInfo *ci) {

  int pkf_knots = 0;
  int pkf_idx = 0;

  vi->ResamplePartFlag = 0;
  vi->FirstPoint = 0;
  vi->BackGroundFlag = 0;
  vi->TEXTURES = 0;
  vi->IV_SCENE = 0;

  // default values
  ci->N = 0;
  ci->S = 12;
  ci->R = 0.0083; // experimental knot value
  ci->Tol = 1e-02;

  if (argc<2) return 0;

  for (int i=1;i<argc;i++) {

    if (argv[i][0] == '-') {
      if (pkf_knots>0) return 0;
      if (!strncmp(&argv[i][1],"N=",2)) {
        ci->N = atoi(&argv[i][3]);
      }
      else if (!strncmp(&argv[i][1],"S=",2))
        ci->S = atoi(&argv[i][3]);
      else if (!strncmp(&argv[i][1],"R=",2))
        ci->R = atof(&argv[i][3]);
      else if (!strncmp(&argv[i][1],"Tol=",4))
        ci->Tol = atof(&argv[i][5]);
      else if (!strncmp(&argv[i][1],"closed",6))
        continue;
      else if (!strncmp(&argv[i][1],"texture",7)) {
        ci->TEXTURES = 1;
        ci->texture_file = QString(argv[i+1]); i++;
      }
      else if (!strncmp(&argv[i][1],"iv-scene",8)) {
        vi->IV_SCENE = 1;
        vi->iv_file = QString(argv[i+1]); i++;
      }
      else if (!strncmp(&argv[i][1],"whitebg",7)) {
        vi->BackGroundFlag = 1;
      }
      else if (!strncmp(&argv[i][1],"ptplot",6)) {
        vi->PT_PLOT = 1;
        vi->ptplot_file = QString(argv[i+1]); i++;
        cout << "We got a pt plot : " << vi->ptplot_file.data() << endl;
      }
      else {
        cerr << "Unknown option : " << argv[i][1] << endl;
        return 0;
      }
    }
    else {
      if (pkf_knots==0) pkf_idx = i;
      pkf_knots++;
    }
  }

  if (pkf_knots+pkf_idx!=argc) return 0;

  ci->Knot = new TubeBundle<Vector3> ;
  for (int i=pkf_idx;i<argc;i++)
    ci->filenames << argv[i];

  // closed curve requested?
  ci->Closed = 0;
  for (int i=1;i<argc;i++)
    if (!strncmp(&argv[i][1],"closed",6))
      ci->Closed = 1;
  
  return 1;
}


void usage(char *prog) {
  cout << "Usage : " << prog << " [OPTIONS] <PFK file>\n"
       << "Optional arguments\n"
       << "  -N=NODES\t\tnumber of points on the curve\n"
       << "  -S=SEGMENTS\t\tcross sectional segmentation\n" 
       << "  -R=RADIUS\t\ttube radius\n"
       << "  -Tol=TOLERANCE\tmesh adjustment tolerance\n"
       << "  -closed\t\tcloses all the curve\n"
       << "  -iv-scene <file.iv>\tadd a .iv scenegraph to the scene.\n"
       << "  -texture <img_file>\ttexture map the pkf curve\n"
       << "  -whitebg\t\twhite background color\n";
  exit(0);
}

// GFX Stuff
SoSeparator* drawCircle(const Vector3& p0, const Vector3& t0, const Vector3& p1) {
  Vector3 d = (p1-p0)-t0*(p1-p0).dot(t0);
  d.normalize();
  SoSeparator *c_sep = new SoSeparator;
  SoCoordinate4 *coords = new SoCoordinate4;
  SoNurbsCurve *nurbs = new SoNurbsCurve;
  SoDrawStyle *ds = new SoDrawStyle;
  SoPointSet *ps = new SoPointSet;

  nurbs->numControlPoints.setValue(9);
  nurbs->knotVector.set1Value(0,0);
  nurbs->knotVector.set1Value(1,0);
  nurbs->knotVector.set1Value(2,0);
  nurbs->knotVector.set1Value(3,1);
  nurbs->knotVector.set1Value(4,1);
  nurbs->knotVector.set1Value(5,2);
  nurbs->knotVector.set1Value(6,2);
  nurbs->knotVector.set1Value(7,3);
  nurbs->knotVector.set1Value(8,3);
  nurbs->knotVector.set1Value(9,4);
  nurbs->knotVector.set1Value(10,4);
  nurbs->knotVector.set1Value(11,4);

  ds->pointSize.setValue(3);
  ps->numPoints.setValue(9);

  float R;
  Vector3 D = p1 - p0;
  float Dlen = D.norm();
  float cosa = (t0.dot(D))/Dlen;
  float sina = sqrt(1.0-cosa*cosa);

  if (sina==0.0) R=1e6;
  else R = 0.5*Dlen/sina;

  float w = cos(M_PI/180.*45.);
  SbVec4f v;

  v = SbVec4f(p0[0],p0[1],p0[2],1.);
  coords->point.set1Value(0,v);

  v = SbVec4f(p0[0]+R*t0[0],p0[1]+R*t0[1],p0[2]+R*t0[2],1);
  v*=w;
  coords->point.set1Value(1,v);

  v = SbVec4f(p0[0]+R*t0[0]+R*d[0],p0[1]+R*t0[1]+R*d[1],p0[2]+R*t0[2]+R*d[2],1);
  coords->point.set1Value(2,v);

  v = SbVec4f(p0[0]+R*t0[0]+2.*R*d[0],p0[1]+R*t0[1]+2.*R*d[1],p0[2]+R*t0[2]+2.*R*d[2],1);
  v*=w;
  coords->point.set1Value(3,v);

  v = SbVec4f(p0[0]+2.*R*d[0],p0[1]+2.*R*d[1],p0[2]+2.*R*d[2],1);
  coords->point.set1Value(4,v);

  v = SbVec4f(p0[0]-R*t0[0]+2.*R*d[0],p0[1]-R*t0[1]+2.*R*d[1],p0[2]-R*t0[2]+2.*R*d[2],1);
  v*=w;
  coords->point.set1Value(5,v);

  v = SbVec4f(p0[0]-R*t0[0]+R*d[0],p0[1]-R*t0[1]+R*d[1],p0[2]-R*t0[2]+R*d[2],1);
  coords->point.set1Value(6,v);

  v = SbVec4f(p0[0]-R*t0[0],p0[1]-R*t0[1],p0[2]-R*t0[2],1);
  v*=w;
  coords->point.set1Value(7,v);

  v = SbVec4f(p0[0],p0[1],p0[2],1.);
  coords->point.set1Value(8,v);

  c_sep->addChild(coords);
  c_sep->addChild(nurbs);
// FIXME : uncomment if u wanna see the control points of the Nurb
  // c_sep->addChild(ds);
  // c_sep->addChild(ps);
  
  return c_sep;
}

void addBezierCurve(SoSeparator *root, Tube<Vector3>* t) {

  // OpenInventor part
  // Midpoints

  int NODES = t->nodes()-1;
  if (t->isClosed()) NODES++;

  SoSeparator *main = new SoSeparator;
  main->ref();

  SoSeparator *midpoints         = new SoSeparator;            // Root
  SoTranslation **mid_trans      = new SoTranslation*[NODES];//t->nodes()-1];
  SoSeparator **sub_midpoints    = new SoSeparator*[NODES];//t->nodes()-1];
  SoMaterial *blue               = new SoMaterial;
  midpoints->ref();

  // Datapoints
  SoSeparator *datapoints         = new SoSeparator;            // Root
  SoTranslation **data_trans      = new SoTranslation*[t->nodes()];
  SoSeparator **sub_datapoints     = new SoSeparator*[t->nodes()];
  SoMaterial *red                 = new SoMaterial;
  datapoints->ref();

  SoSphere *sphere = new SoSphere;
  sphere->ref();
  float Rad = ((*t)[0].getMidPoint()-(*t)[0].getPoint()).norm()/5.0;
  sphere->radius = Rad;

  for (int i=0;i<t->nodes();i++) {
    if (i!=t->nodes()-1 || t->isClosed())
      sub_midpoints[i] = new SoSeparator;
    sub_datapoints[i] = new SoSeparator;
    data_trans[i] = new SoTranslation;
    if (i!=t->nodes()-1 || t->isClosed())
      mid_trans[i] = new SoTranslation;

    if (i!=t->nodes()-1 || t->isClosed())
      sub_midpoints[i]->ref();
    sub_datapoints[i]->ref();
    data_trans[i]->ref();
    if (i!=t->nodes()-1 || t->isClosed())
      mid_trans[i]->ref();
  }

  SoComplexity  *complexity = new SoComplexity;
  complexity->value = 0.1;

  datapoints->addChild(complexity);
  midpoints->addChild(complexity);

  red->diffuseColor.setValue(1,0,0);
  blue->diffuseColor.setValue(0,0,1);
  datapoints->addChild(red);
  midpoints->addChild(blue);

  for (int j=0;j<t->nodes();j++) {
    Vector3 p = (*t)[j].getPoint();
    data_trans[j]->translation.setValue((float *)&p);
    if (j!=t->nodes()-1 || t->isClosed()) {
      Vector3 m = (*t)[j].getMidPoint();
      mid_trans[j]->translation.setValue((float *)&m);
      sub_midpoints[j]->addChild(mid_trans[j]);
      sub_midpoints[j]->addChild(sphere);
    }

    sub_datapoints[j]->addChild(data_trans[j]);
    sub_datapoints[j]->addChild(sphere);

    if (j!=t->nodes()-1 || t->isClosed())
      midpoints->addChild(sub_midpoints[j]);
    datapoints->addChild(sub_datapoints[j]);

  }
  
  SoMaterial *white = new SoMaterial;
  white->diffuseColor.setValue(1,1,1);

  main->addChild(datapoints);
  main->addChild(midpoints);
  main->addChild(white);
  //
  // Tangents Datapoints
  //

  SoLineSet *linesetData        = new SoLineSet;
  linesetData->setName(SbName("datatangents"));
  SoCoordinate3 *DataTanCoords  = new SoCoordinate3; 
  SoSeparator *DataTangentsNode = new SoSeparator;

  DataTangentsNode->ref();
  
  // Add _v.size() times 2 points for each line segment
  for (int i=0;i<t->nodes();i++)
    linesetData->numVertices.set1Value(i,2);
  
  // Add all the necessary coordinates
  // i.e : vertex point plus/minus Stream direction
  // where the length of each line is scaled by the biggest norm of _Stream
  // such that this _Stream vector is exactly the distance between 2 neighbour
  // points on the sphere
  
  // Distance between 2 neighbour points
  float TanLength = ((*t)[0].getMidPoint()-(*t)[0].getPoint()).norm()*.8;

  for (int i=0;i<t->nodes();i++) {
    Vector3 point    = (*t)[i].getPoint();
    Vector3 endpoint = point + (*t)[i].getTangent()*TanLength;
    
    // 2 coord points for the tangent line
    DataTanCoords->point.set1Value(i*2+0,(float *)&point);
    DataTanCoords->point.set1Value(i*2+1,(float *)&endpoint);
  }

  SoMaterial *yellow = new SoMaterial;
  yellow->diffuseColor.setValue(1,1,0);
  DataTangentsNode->addChild(yellow);
  DataTangentsNode->addChild(DataTanCoords);
  DataTangentsNode->addChild(linesetData);

  main->addChild(DataTangentsNode);

  //

  //
  // Tangents Datapoints
  //

  SoLineSet *linesetMid        = new SoLineSet;
  linesetMid->setName(SbName("midtangents"));
  SoCoordinate3 *MidTanCoords  = new SoCoordinate3; 
  SoSeparator *MidTangentsNode = new SoSeparator;

  MidTangentsNode->ref();
  
  // Add _v.size() times 2 points for each line segment
  for (int i=0;i<NODES;i++)
    linesetMid->numVertices.set1Value(i,2);
  
  // Add all the necessary coordinates
  // i.e : vertex point plus/minus Stream direction
  // where the length of each line is scaled by the biggest norm of _Stream
  // such that this _Stream vector is exactly the distance between 2 neighbour
  // points on the sphere
  
  // Distance between 2 neighbour points

  for (int i=0;i<NODES;i++) {
    Vector3 point    = (*t)[i].getMidPoint();
    Vector3 endpoint = point + (*t)[i].getMidTangent()*TanLength;
    
    // 2 coord points for the tangent line
    MidTanCoords->point.set1Value(i*2+0,(float *)&point);
    MidTanCoords->point.set1Value(i*2+1,(float *)&endpoint);
  }

  SoMaterial *green = new SoMaterial;
  green->diffuseColor.setValue(0,1,0);
  MidTangentsNode->addChild(green);
  MidTangentsNode->addChild(MidTanCoords);
  MidTangentsNode->addChild(linesetMid);

  main->addChild(MidTangentsNode);

  //

  SoSeparator *CurveSep = new SoSeparator;
  CurveSep->ref();
  SoCoordinate4 *CurvePts = new SoCoordinate4;
  SoNurbsCurve  *Curve    = new SoNurbsCurve;

  int Order = 3;// or 2?
  int Kknots = 4*NODES+1; // (t->nodes()-1)+1;
  if (Kknots<=Order) Kknots = 3;

  int KLength = (Order + Kknots);
  Curve->numControlPoints = Kknots;

  float KSequence[KLength];
  KSequence[0] = KSequence[1] = KSequence[2] = 0;
  KSequence[KLength-1] = KSequence[KLength-2] = KSequence[KLength-3] = 1;  
  if ((Kknots==0)||(KLength==0)) {cout<<"KnotVector empty\n";return;}
  int ctrlpts = (Kknots-1)/2;   // Number of Weighted Control Points
  int counter = 1;
  if (ctrlpts==1) return;
  for (int i=3;i<(KLength-3);i++) {
    KSequence[i++] = float(counter)/float(ctrlpts);
    KSequence[i]   = float(counter)/float(ctrlpts);
    counter++;
  }

  Curve->knotVector.setValues(0,KLength,KSequence);//KV.getSequence());

  assert(t->begin()->isBiarc());
  float o;
  Vector3 q0,mid0,q1,mid1,dd1,dd2;
  for (int i=0;i<NODES;i++) {//t->nodes()-1;i++) {
    (*t)[i].getBezier(q0,mid0,dd1,q1,mid1,dd2);
    CurvePts->point.set1Value(4*i+0,q0[0],q0[1],q0[2],1.0);
    o = ((mid0-q0).dot((*t)[i].getTangent()))/sqrt((mid0-q0).dot(mid0-q0));
    CurvePts->point.set1Value(4*i+1,mid0[0],mid0[1],mid0[2],o);
    CurvePts->point.set1Value(4*i+2,q1[0],q1[1],q1[2],1.0);
    o = ((mid1-q1).dot((*t)[i].getMidTangent()))/sqrt((mid1-q1).dot(mid1-q1));
    CurvePts->point.set1Value(4*i+3,mid1[0],mid1[1],mid1[2],o);
  }
  Vector3 gaga;
  if (t->isClosed())
    gaga = (*t)[0].getPoint();
  else
    gaga = (*t)[t->nodes()-1].getPoint();
  CurvePts->point.set1Value(Kknots-1,gaga[0],gaga[1],gaga[2],1.0);

  CurveSep->addChild(CurvePts);
  CurveSep->addChild(Curve);
  CurveSep->unrefNoDelete();
  main->addChild(CurveSep);

  root->addChild(main);

}