#ifndef COIN_SOKNOT_H
#define COIN_SOKNOT_H

#include <Inventor/nodes/SoSubNode.h>
#include <Inventor/nodes/SoShape.h>
#include <Inventor/fields/SoSFFloat.h>
#include <Inventor/fields/SoSFInt32.h>

#include "../include/Tube.h"

#define SOGL_MATERIAL_PER_PART   0x08
#define SOGL_NEED_NORMALS        0x10
#define SOGL_NEED_TEXCOORDS      0x20
#define SOGL_NEED_3DTEXCOORDS    0x40
#define SOGL_NEED_MULTITEXCOORDS 0x80 // internal

class SoKnot : public SoShape {

  SO_NODE_HEADER(SoKnot);

  void sogl_render_tube(const float radius,
			const int segments,
			const int nodes,
			const float Tol,
			SoMaterialBundle * const material,
			const unsigned int flagsin,
			SoState * state);

  float* sogl_tube_texcoords;
  void reset_texcoords(int Width, int Height);

  float _old_radius;
  int _old_nodes;
  int _old_segments;

  // Original curve, all resamplings are done with this
  // initial knot to minimize precision errors
  Tube<Vector3> *original;
  Tube<Vector3> *knot;

public:
  static void initClass(void);
  SoKnot(void);

  SoSFFloat radius;
  SoSFInt32 nodes;
  SoSFInt32 segments;

  void setKnot(Tube<Vector3>* t, const float Tol);
  Tube<Vector3>* getKnot();
  void reset();

  virtual void GLRender(SoGLRenderAction * action);
/*
  virtual void rayPick(SoRayPickAction * action);
*/

  virtual void getPrimitiveCount(SoGetPrimitiveCountAction * action);

protected:
  virtual ~SoKnot();

  virtual void generatePrimitives(SoAction * action) ;
  virtual void computeBBox(SoAction * action,
			   SbBox3f & box,
			   SbVec3f & center);

};

#endif // !COIN_SOKNOT_H
