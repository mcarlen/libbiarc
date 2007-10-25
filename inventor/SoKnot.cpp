/*!
  \class SoKnot SoKnot.h
  \brief The SoKnot class is for rendering tubular curves in the
  Coin/OpenInventor graphics library.

  \ingroup InventorGroup

  For details about the Coin3D API and some more precise explanations
  about the structure of Coin see
  
  http://www.coin3d.org/doc/

  The SoKnot class is a ready made node for a Coin3D/OpenInventor
  program that generates a tubular shape and can then be inserted
  in a scenegraph. The shape is rendered with the current material.
  The paramters for rendering a knot are the radius, the number of
  nodes on the centerline and the number of segments on the
  cross-section.

  put example how to use this ...
*/

#include "SoKnot.h"

#include <Inventor/SoPickedPoint.h>
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/actions/SoGetPrimitiveCountAction.h>
#include <Inventor/actions/SoRayPickAction.h>
#include <Inventor/bundles/SoMaterialBundle.h>
#include <Inventor/SoPrimitiveVertex.h>
#include <Inventor/elements/SoGLTextureEnabledElement.h>
#include <Inventor/elements/SoGLTexture3EnabledElement.h>
#include <Inventor/elements/SoTextureCoordinateElement.h>
#include <Inventor/elements/SoMaterialBindingElement.h>
//#include <Inventor/misc/SoGL.h>
//#include <Inventor/misc/SoGenerate.h>
// #include <Inventor/misc/SoPick.h>
//#include <Inventor/misc/SoState.h>

#include <GL/gl.h>


/*!
  \var SoSFFloat SoKnot::radius;

  Tube radius of the SoKnot instance.
*/

/*!
  \var SoSFInt32 SoKnot::nodes;

  Number of nodes (number of data points) along the curve.
*/

/*!
  \var SoSFInt32 SoKnot::segments;

  Number of segments on the circular cross section of
  the tube.
*/

// *************************************************************************

SO_NODE_SOURCE(SoKnot);

/*!
  Constructor.
*/
SoKnot::SoKnot(void) {

  SO_NODE_CONSTRUCTOR(SoKnot);

  SO_NODE_ADD_FIELD(radius, (1.0f));
  SO_NODE_ADD_FIELD(segments, (6));
  SO_NODE_ADD_FIELD(nodes, (10));

  knot = NULL;
  original = NULL;
  sogl_tube_texcoords = NULL;
}

/*!
  Destructor.
*/
SoKnot::~SoKnot() {
  if (sogl_tube_texcoords!=NULL) delete[] sogl_tube_texcoords;
  if (original != NULL) { delete original; original = NULL; }
  if (knot != NULL) { delete knot; knot = NULL; }
}

/*!
  Static function that initializes the SoKnot class
  in Coin.
*/
void SoKnot::initClass(void) {
  SO_NODE_INIT_CLASS(SoKnot, SoShape, "SoShape");
}

/*!
  Initializes the SoKnot instance, given a Tube
  object \a t and a tolerance \a Tol for the mesh generation routine.
  
  The original Tube \a t will not be changed in any way. Every time
  an action on the Tube object is necessary, the work is carried out
  on a copy, internally of the SoKnot instance.

  \sa getKnot()
*/
void SoKnot::setKnot(Tube<Vector3>* t, const float Tol) {

  this->original = t;

  if (knot==NULL)
    knot = new Tube<Vector3>;
  else {
    delete knot;
    knot = new Tube<Vector3>;
  }

  *knot = *original;

  knot->makeMesh(t->nodes(),
		 t->segments(),		 
		 t->radius(),
		 Tol);

  _old_segments = t->segments();
  _old_nodes = t->nodes();
  _old_radius = t->radius();


/*
  sogl_tube_texcoords = NULL;

  // Fixme reinit the new texcoords
  reset_texcoords(t->segments(),t->nodes());
*/
}

/*!
  Returns a pointer to the current Tube object
  stored internally in the SoKnot object.

  \sa setKnot()
*/
Tube<Vector3>* SoKnot::getKnot() {
  return this->knot;
}

/*!
  Action method for the SoGLRenderAction.
  This is called during rendering traversals.
  Reimplemented from SoShape.

  Note : This comes from the Coin documentation
*/
void SoKnot::GLRender(SoGLRenderAction * action) {

  /* fixme : do that stuff for MBINDING==PER_VERTEX in order to mak
     curvature,torsion stuff working with this
     so far nothing is implemented in that direction
  */

  if (!this->shouldGLRender(action)) return;

  SoState * state = action->getState();

  SoMaterialBundle mb(action);
  mb.sendFirst();

  SbBool doTextures = FALSE;
  SbBool do3DTextures = FALSE;
  if (SoGLTextureEnabledElement::get(state)) doTextures = TRUE;
  /*
  else if (SoGLTexture3EnabledElement::get(state)) do3DTextures = TRUE;
  */

  // urspruenglich :
  //SbBool sendNormals = !mb.isColorOnly() || 
  //  (SoTextureCoordinateElement::getType(state) == SoTextureCoordinateElement::FUNCTION);

  SbBool sendNormals = !mb.isColorOnly();  

  //float complexity = SbClamp(this->getComplexityValue(action), 0.0f, 1.0f);

  unsigned int flags = 0;
  if (sendNormals) flags |= SOGL_NEED_NORMALS;
  if (doTextures) flags |= SOGL_NEED_TEXCOORDS;
  else if (do3DTextures) flags |= SOGL_NEED_3DTEXCOORDS;
  if (SoMaterialBindingElement::get(state)==SoMaterialBindingElement::PER_PART)
    flags |= SOGL_MATERIAL_PER_PART;

  float Tol = knot->isClosed()?1e-3:-1;

  if (sogl_tube_texcoords==NULL && doTextures) {
 //    cout << "reset_tex ... " << flush;
    reset_texcoords(knot->segments(),knot->nodes());
 //    cout << "done\n" << flush;
  }

  sogl_render_tube(this->radius.getValue(),
		   this->segments.getValue(),
		   this->nodes.getValue(),
		   Tol,
		   &mb,
		   flags, state);

}

/*!
Implemented by SoShape subclasses to let the SoShape
superclass know the exact size and weighted center point
of the shape's bounding box.

The bounding box and center point should be calculated
and returned in the local coordinate system.

The method implements action behavior for shape nodes
for SoGetBoundingBoxAction. It is invoked from
SoShape::getBoundingBox(). (Subclasses should not
override SoNode::getBoundingBox().)

The box parameter sent in is guaranteed to be an
empty box, while center is undefined upon function
entry.

Implements SoShape. 
*/
void
SoKnot::computeBBox(SoAction * action, SbBox3f & box, SbVec3f & center)
{
  
  center = (const float*)&(knot->getCenter());
  Vector3 BBox_Min, BBox_Max;
  this->knot->getBoundingBox(BBox_Min,BBox_Max);
  
  box.setBounds(SbVec3f((const float*)&BBox_Min),
		SbVec3f((const float*)&BBox_Max));
  
}

/*
  void
  SoKnot::rayPick(SoRayPickAction *action)
  {
  // todo : zersch versta
  if (!shouldRayPick(action)) return;
  
  sopick_pick_sphere(this->radius.getValue(),
  action);
  }
*/

/*!
  Action method for the SoGetPrimitiveCountAction.

  Calculates the number of triangle, line segment
  and point primitives for the node and adds these
  to the counters of the action.

  Nodes influencing how geometry nodes calculates
  their primitive count also overrides this method
  to change the relevant state variables.

  Reimplemented from SoShape.
*/
void SoKnot::getPrimitiveCount(SoGetPrimitiveCountAction * action) {

  if (!this->shouldPrimitiveCount(action)) return;
  action->addNumTriangles(2 * this->segments.getValue() *
                          this->nodes.getValue());

}

/*!
  The method implements action behavior for shape
  nodes for SoCallbackAction. It is invoked from
  SoShape::callback(). (Subclasses should not
  override SoNode::callback().)

  The subclass implementations uses the convenience
  methods SoShape::beginShape(), SoShape::shapeVertex(),
  and SoShape::endShape(), with SoDetail instances, to
  pass the primitives making up the shape back to the caller.

  Implements SoShape.
*/
void SoKnot::generatePrimitives(SoAction * action) {

  /* coin code 

  static void generate_cube(const float width,
                            const float height,
                            const float depth,
                            const unsigned int flags,
                            SoShape * const shape,
                            SoAction * const action) {
    SbVec3f varray[8];
    sogenerate_generate_cube_vertices(varray,
                           width * 0.5f,
                           height * 0.5f,
                           depth * 0.5f);
  */

  SoPrimitiveVertex vertex;
  vertex.setMaterialIndex(0);
  
  this->beginShape(action, SoShape::QUADS);
  
  int S = this->segments.getValue();
  int N = this->nodes.getValue();

  if (!knot->isClosed())
    N-=1;

  for (int i=0;i<S;i++) { // number of segments on the circle
    for (int j=0;j<N;j++) { // number of nodes on the curve

      // corner 1
      vertex.setNormal((const GLfloat*)&(knot->meshNormal(j*(S+1)+i)));
      vertex.setPoint((const GLfloat*)&(knot->meshPoint(j*(S+1)+i)));
      this->shapeVertex(&vertex);

      // corner 2
      vertex.setNormal((const GLfloat*)&(knot->meshNormal((j+1)*(S+1)+i)));
      vertex.setPoint((const GLfloat*)&(knot->meshPoint((j+1)*(S+1)+i)));
      this->shapeVertex(&vertex);

      // corner 3
      vertex.setNormal((const GLfloat*)&(knot->meshNormal((j+1)*(S+1)+i+1)));
      vertex.setPoint((const GLfloat*)&(knot->meshPoint((j+1)*(S+1)+i+1)));
      this->shapeVertex(&vertex);

      // corner 4
      vertex.setNormal((const GLfloat*)&(knot->meshNormal(j*(S+1)+i+1)));
      vertex.setPoint((const GLfloat*)&(knot->meshPoint(j*(S+1)+i+1)));
      this->shapeVertex(&vertex);

    }
  }
  this->endShape();

}


/*!
  Private function. Reinitializes the texture coordinates given
  a Width and a Height.
*/
void SoKnot::reset_texcoords(int Segs, int Nodes) {

  if (sogl_tube_texcoords!=NULL) {
    //cout << "del texcoords" << flush;
    delete[] sogl_tube_texcoords;
    sogl_tube_texcoords = NULL;
    //cout << " ok.\n" << flush;
  }

  if (sogl_tube_texcoords==NULL) {
    //cout << "new texcoords " << flush;
    sogl_tube_texcoords = new float[2*(Segs+1)*(Nodes+1)];
    //cout << " ok.\n" <<flush;
  }

  for (int i=0;i<=Segs;i++) {
    for (int j=0;j<=Nodes;j++) {
      sogl_tube_texcoords[2*(j*(Segs+1)+i)]   = (float)j/(float)Nodes;
      sogl_tube_texcoords[2*(j*(Segs+1)+i)+1] = (float)i/(float)Segs;
    }
  }

  for (int i=0;i<=Segs;i++) {
    for (int j=0;j<=Nodes;j++) {
      // cout << " (" << sogl_tube_texcoords[2*(j*(Segs+1)+i)] << ',' << sogl_tube_texcoords[2*(j*(Segs+1)+i)+1] << ")";
    }
    // cout << endl;
  }
}


// Private openGL rendering method
/*!
  Private openGL function. Renders a knot shape, given
  a radius, segments, nodes, tolerance, material, state
  and a rendering Flag.
*/
void SoKnot::sogl_render_tube(const float radius,
			      const int segments,
			      const int nodes,
			      const float Tol,
			      SoMaterialBundle * const material,
			      const unsigned int flagsin,
			      SoState * state)
{

  // if segments or nodes changed, update the mesh
  if (_old_radius!=radius) {
    if (segments!=_old_segments || nodes!=_old_nodes) {

      knot->clear_tube();
      knot->flush_all();

      *knot = *original;
      if (original->isClosed())
	knot->link();

      knot->makeMesh(nodes,segments,radius,Tol);

      _old_segments = segments;
      _old_nodes = nodes;
    }
    else {
      knot->scaleTubeRadius(radius);
    }
    _old_radius = radius;
  }
  else {
    if (segments!=_old_segments || nodes!=_old_nodes) {

      knot->clear_tube();
      knot->flush_all();

      *knot = *original;

      if (flagsin & SOGL_NEED_TEXCOORDS)
        reset_texcoords(segments, nodes);

      if (original->isClosed()) knot->link();

      knot->makeMesh(nodes,segments,radius,Tol);

      _old_segments = segments;
      _old_nodes = nodes;

    }
  }

  // code in the sample inventor cube render thing
  //const SbBool * unitenabled = NULL;
  //int maxunit = 0;
  //const cc_glglue * glue = NULL;
  
  int flags = flagsin;

  /*
  if (state) {
    unitenabled = 
      SoMultiTextureEnabledElement::getEnabledUnits(state, maxunit);
    if (unitenabled) {
      glue = sogl_glue_instance(state);
      flags |= SOGL_NEED_MULTITEXCOORDS;
    }
    else maxunit = -1;
  }
*/

  int S = segments;
  int N = knot->isClosed()?nodes:nodes-1;

  //  int u;

  glBegin(GL_QUADS);

  for (int j=0;j<N;j++) { // number of nodes on the curve

    for (int i=0;i<S;i++) { // number of segments on the circle

      if (flags & SOGL_MATERIAL_PER_PART)
        material->send(j, TRUE);

      // 4 corners of the quad
      // corner 1
      if (flags & SOGL_NEED_NORMALS)
	glNormal3fv((const GLfloat*)&(knot->meshNormal(j*(S+1)+i)));
      if (flags & SOGL_NEED_TEXCOORDS) {
	glTexCoord2fv(&sogl_tube_texcoords[2*(j*(S+1)+i)]);
      }
      /*
      else if (flags & SOGL_NEED_3DTEXCOORDS) {
	//glTexCoord3fv(sogl_cube_3dtexcoords[*iptr]);
	}*/
      /*
      if (flags & SOGL_NEED_MULTITEXCOORDS) {
	for (u = 1; u <= maxunit; u++) {
	  if (unitenabled[u]) {
	    //cc_glglue_glMultiTexCoord2fv(glue, (GLenum) (GL_TEXTURE0 + u),
	    //                               &sogl_cube_texcoords[j<<1]);
	  }
	}
      }
      */
      glVertex3fv((const GLfloat*)&(knot->meshPoint(j*(S+1)+i)));
      
      // corner 2
      if (flags & SOGL_NEED_NORMALS)
	glNormal3fv((const GLfloat*)&(knot->meshNormal((j+1)*(S+1)+i)));
      if (flags & SOGL_NEED_TEXCOORDS) {
	glTexCoord2fv(&sogl_tube_texcoords[2*((j+1)*(S+1)+i)]);
      }
      glVertex3fv((const GLfloat*)&(knot->meshPoint((j+1)*(S+1)+i)));

      // corner 3
      if (flags & SOGL_NEED_NORMALS)
	glNormal3fv((const GLfloat*)&(knot->meshNormal((j+1)*(S+1)+i+1)));
      if (flags & SOGL_NEED_TEXCOORDS) {
	glTexCoord2fv(&sogl_tube_texcoords[2*((j+1)*(S+1)+(i+1))]);
      }
      glVertex3fv((const GLfloat*)&(knot->meshPoint((j+1)*(S+1)+i+1)));

      // corner 4
      if (flags & SOGL_NEED_NORMALS)
	glNormal3fv((const GLfloat*)&(knot->meshNormal(j*(S+1)+i+1)));
      if (flags & SOGL_NEED_TEXCOORDS) {
	glTexCoord2fv(&sogl_tube_texcoords[2*(j*(S+1)+i+1)]);
      }
      glVertex3fv((const GLfloat*)&(knot->meshPoint(j*(S+1)+i+1)));

    }
  }
  glEnd();

  /* state stuff that i'm not familiar with so far 
  if (state) {
    // always encourage auto caching for cubes
    SoGLCacheContextElement::shouldAutoCache(state, SoGLCacheContextElement::DO_AUTO_CACHE);
  }
*/
}

