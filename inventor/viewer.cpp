#include "viewer.h"

SOQT_OBJECT_SOURCE(VVV);

VVV::VVV(QWidget *parent, const char *name, SbBool embed, SoQtFullViewer::BuildFlag flag, SoQtViewer::Type type) :
SoQtExaminerViewer(parent,name,embed,flag,type) { oldx2 = oldy2 = oldx = oldy = -1; }
VVV::~VVV() {}

#if 0
// u,v in [0,1]
void VVV::update_picked(float u, float v, float u2, float v2) {
  // FIXME : one component only supported for now
  int x,y,x2,y2,i;
  float a,b;

  cout << "u,v  = " << u << " ," << v << endl;
  cout << "u2,v2 = " << u2 << " ," << v2 << endl;

  // reset highlight to zero
  if (oldx2>=0 && oldy2>=0) {
    for (i=oldx;i<=oldx2;i++)
      materials[0]->diffuseColor.set1Value(i,ColorTable[0]);
    for (i=oldy;i<=oldy2;i++)
      materials[0]->diffuseColor.set1Value(i,ColorTable[0]);
  }
  else {
    if (oldx >= 0)
      materials[0]->diffuseColor.set1Value(oldx,ColorTable[0]);
    if (oldy >= 0)
      materials[0]->diffuseColor.set1Value(oldy,ColorTable[0]);
  }

  if (u2>=0 && v2>=0) {
    x = (*Knot)[0].biarcPos(u); //*(*Knot)[0].length());
    y = (*Knot)[0].biarcPos(v); //*(*Knot)[0].length());
    x2 = (*Knot)[0].biarcPos(u2); //*(*Knot)[0].length());
    y2 = (*Knot)[0].biarcPos(v2); //*(*Knot)[0].length());

    cout << "x,y  = " << x << " , " << y << endl;
    cout << "2x,y = " << x2 << " , " << y2 << endl;

    for (i=x;i<=x2;i++)
      materials[0]->diffuseColor.set1Value(i,ColorTable[1]);
    for (i=y;i<=y2;i++)
      materials[0]->diffuseColor.set1Value(i,ColorTable[1]);


    // sort the values s.t u<u2 and v<v2
    circles->removeAllChildren();
    if (x>x2) { int xtmp = x; x = x2; x2 = xtmp; }
    if (y>y2) { int ytmp = y; y = y2; y2 = ytmp; }
    if (u>u2) { float utmp = u; u = u2; u2 = utmp; }
    if (v>v2) { float vtmp = v; v = v2; v2 = vtmp; }

 //   float NUMX = ((float)(*Knot)[0].nodes())*(u2-u)/(*Knot)[0].length()/5.;
 //   float NUMY = ((float)(*Knot)[0].nodes())*(v2-v)/(*Knot)[0].length()/5.;

    float Nx = (u2-u)/(float)(x2-x);
    // float Ny = (v2-v)/(float)(y2-y);
    for (i=0;i<=(x2-x);i++) {
      a = u+(float)i*Nx;
      //for (j=0;j<=(y2-y);j++) {
      b = v; //+(float)j*Ny;
        circles->addChild(drawCircle((*Knot)[0].pointAt(a),
                                  (*Knot)[0].tangentAt(a),
                                  (*Knot)[0].pointAt(b)));
/*
      b = v+(float)(y2-y)*Ny;
        circles->addChild(drawCircle((*Knot)[0].pointAt(a),
                                  (*Knot)[0].tangentAt(a),
                                  (*Knot)[0].pointAt(b)));
*/
      //}
   }

/*
    for (j=0;j<=(y2-y);j++) {
      b = v+(float)j*Ny;
      a = u;
        circles->addChild(drawCircle((*Knot)[0].pointAt(a),
                                  (*Knot)[0].tangentAt(a),
                                  (*Knot)[0].pointAt(b)));
      a = u+(float)(x2-x)*Ny;
        circles->addChild(drawCircle((*Knot)[0].pointAt(a),
                                  (*Knot)[0].tangentAt(a),
                                  (*Knot)[0].pointAt(b)));
    }
*/

    // reset old values
    if (x<x2 && x>=0) { oldx = x; oldx2 = x2; }
    else { oldx = x2; oldx2 = x; }
    if (y<y2 && y>0) { oldy = y; oldy2 = y2; }
    else { oldy = y2; oldy2 = y; }
  }
  else {
    x = (*Knot)[0].biarcPos(u);//*(*Knot)[0].length());
    y = (*Knot)[0].biarcPos(v);//*(*Knot)[0].length());
    materials[0]->diffuseColor.set1Value(x,ColorTable[1]);
    materials[0]->diffuseColor.set1Value(y,ColorTable[1]);
    oldx = x; oldx2 = -1;
    oldy = y; oldy2 = -1;

    SoSeparator *sep = drawCircle((*Knot)[0].pointAt(u),//*(*Knot)[0].length()),
                                  (*Knot)[0].tangentAt(u),//*(*Knot)[0].length()),
                                  (*Knot)[0].pointAt(v));//*(*Knot)[0].length()));
    //cout << "Add child\n";
    if (circles->getNumChildren()==1)
      circles->replaceChild(0,sep);
    else {
      circles->removeAllChildren();
      circles->addChild(sep);
    }
  }
}
#endif