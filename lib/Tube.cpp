/*!
  \class Tube Tube.h include/Tube.h
  \ingroup BiarcLibGroup
  \brief The Tube class contains the mesh of a tube around a
  curve in \f$\mathcal{R}^3\f$.

  This class is used to store the points and normals of a
  tubular mesh around a space-curve.

  The usage of Tube objects are shown in the following example :

  \code

  #include "../include/Tube.h"
  
  main(int argc, char **argv) {
  
    // Read curve data from a file
    Tube t("curve.pkf");

    // Close the curve
    t.link();

    // Number of nodes on the curve
    int N = t.nodes();

    // We want 10 segments on the cross section
    int S = 10;

    // The radius is set to 1.0
    float R = 1.0;

    // We match up the mesh with this tolerance value
    float Tol = 1e-3;

    // Generate mesh points and normals
    t.makeMesh(N,S,R,Tol);

    ...

    // Write the output to standart out
    cout << t << endl;

    ...

    return 0;
  }
  \endcode
  
  \sa Curve, Biarc
  
*/
 
//
// documentation of inlined methods
//
 
/*!
  \fn ostream &Tube::operator<<(ostream &out, Tube &t) {

  Overloaded left shift operator. Writes all the mesh points
  to the stream \a out. For a closed curve the points on the
  circle of the first and the last node are repeated to keep
  the connectivity. Sometimes a permutation is done to diminish
  twist in the mesh. Therefore to keep the connectivity of all
  faces in the mesh, these points occur twice. As well as the
  first and the last point on the circular cross section.
  If we have S segments, there will be written S+1 points
  to the stream.
*/

#include "../include/Tube.h"

// Template thing
#ifndef __TUBE_SRC__
#define __TUBE_SRC__

/*!
  Initialize Tube object. Number of segments and the radius
  are set to 0.0. The mesh point table and the mesh normal
  table are set to NULL.
*/
template <class Vector>
void Tube<Vector>::init() {
  _Segments = 0;
  _Radius = 0.0;
  _MeshPoints = NULL;
  _MeshNormals = NULL;
}

/* TODO : curve to tube and tube to curve correct conversion */

/*!
  Default constructor. Creates an empty curve with no mesh points.
*/
template <class Vector>
Tube<Vector>::Tube() {
  init();
}

/*!
  Constructs a new Tube object, reading the curve data
  from a stream \a in. Header is automatically set to
  the default values and is NOT expected to on the stream!

  \sa makeMesh()
*/
template <class Vector>
Tube<Vector>::Tube(istream &in) : Curve<Vector>(in) {
  init();
}

/*!
  Constructor copies a Curve object \a curve. No interpolation
  and no mesh generation is done. To do that makeMesh() must be
  called.

  \sa makeMesh()
*/
template <class Vector>
Tube<Vector>::Tube(const Curve<Vector> &curve) : Curve<Vector>(curve) {
  init();
}

/*!
  Constructor copies a Tube object \a tube. Only the curve
  data points are copied and the curve is closed if necessary.
  No interpolation and no mesh generation is done in the new
  Tube instance. To do that makeMesh() must be called.

  Maybe this will be changed in the future to copy
  all the mesh points if the \a tube object has mesh points.

  \sa makeMesh()
*/
template <class Vector>
Tube<Vector>::Tube(const Tube<Vector> &tube) : Curve<Vector>(tube) {
  init();
  *this = tube;
}

/*!
  Assign operator. Copies the curve data and
  calls link() if Tube \a c is closed.

FIXME:doc
*/
template <class Vector>
Tube<Vector> &Tube<Vector>::operator= (const Tube<Vector> &c) {
  if (this->nodes()!=0) this->flush_all();

  this->_Biarcs.insert(this->_Biarcs.begin(),c._Biarcs.begin(),c._Biarcs.end());
  if (c.isClosed())
    this->link();

  // Fix the biarcs' curve pointers!!
  for (biarc_it it=this->begin();it!=this->end();it++)
    it->setCurve(this);
  return *this;

/*
  int N = c.nodes();
  typename vector<Biarc<Vector> >::iterator current = c.begin();
  for (int i=0;i<N;i++) {
    append(*current);
    current++;
  }

  if (c.isClosed())
    this->link();

  return *this;
*/
}

/*!
  Destructor. Clears the memory used by the mesh points and normals.
*/
template <class Vector>
Tube<Vector>::~Tube() {
  clear_tube();
}

/*!
  Deletes the mesh points and the mesh normals.
*/
template <class Vector>
void Tube<Vector>::clear_tube() {
  if (_MeshPoints!=NULL) {
    delete[] _MeshPoints;
    _MeshPoints = NULL;
  }
  if (_MeshNormals!=NULL) {
    delete[] _MeshNormals;
    _MeshNormals = NULL;
  }
 }

/*!
  Returns the number of segments on the tubular cross section.

  \sa Curve::nodes(),radius()  
*/
template <class Vector>
int Tube<Vector>::segments() {
  return _Segments;
}

/*!
  Returns the current radius of the tubular curve.

  \sa Curve::nodes(),segments()
*/
template <class Vector>
float Tube<Vector>::radius() {
  return _Radius;
}

/*!
  Returns mesh point number \a i. There are a total
  of \f$(segments+1)\cdot (nodes+1)\f$ mesh points. 
*/
template <class Vector>
Vector & Tube<Vector>::meshPoint(int i) {
  return _MeshPoints[i];
}

/*!
  Returns mesh normal number \a i. There are a total
  of \f$(segments+1)\cdot (nodes+1)\f$ mesh normals. 
*/
template <class Vector>
Vector & Tube<Vector>::meshNormal(int i) {
  return _MeshNormals[i];
}

/*!
  Puts the position of the minimum and the maximum corner
  of a box containing the whole tubular object into the vectors
  \a BBox_Min and \a BBox_Max.
*/
template <class Vector>
void Tube<Vector>::getBoundingBox(Vector & BBox_Min, Vector & BBox_Max) {

  // Return knot dimensions (computed in : makeMesh())
  BBox_Min = _BBox_Min;
  BBox_Max = _BBox_Max;

}

/*!
  Returns the center of mass of the tubular mesh.
*/
template <class Vector>
Vector & Tube<Vector>::getCenter() {

  // Return center of mass of the tube !!
  return _Center;
}

// this guy is private
/*!
  Computes the bounding box of the Tube instance. The
  of the bounding box touches the tube on its extremal extensions.

  This function precomputes the center and the bounding box!
  For caching purpose.
*/
template <class Vector>
void Tube<Vector>::computeBoundingBox() {

  int N = this->nodes();
  int S = this->_Segments;

  // Compute bounding box for the mesh
  // initialise with first point
  _BBox_Min = _MeshPoints[0];
  _BBox_Max = _MeshPoints[0];
  _Center = _MeshPoints[0];


  for (int i=1;i<N;i++) {
    for (int j=0;j<S;j++) {
      int idx = i*(S+1)+j;
      _BBox_Min = Vector3(_MeshPoints[idx][0]<_BBox_Min[0]?
			  _MeshPoints[idx][0]:_BBox_Min[0],
			  _MeshPoints[idx][1]<_BBox_Min[1]?
			  _MeshPoints[idx][1]:_BBox_Min[1],
			  _MeshPoints[idx][2]<_BBox_Min[2]?
			  _MeshPoints[idx][2]:_BBox_Min[2]);
      _BBox_Max = Vector3(_MeshPoints[idx][0]>_BBox_Max[0]?
			  _MeshPoints[idx][0]:_BBox_Max[0],
			  _MeshPoints[idx][1]>_BBox_Max[1]?
			  _MeshPoints[idx][1]:_BBox_Max[1],
			  _MeshPoints[idx][2]>_BBox_Max[2]?
			  _MeshPoints[idx][2]:_BBox_Max[2]);
      
      _Center += _MeshPoints[idx];
    }
  }

  _Center /= (float)(N*S+1);

}

/*!
  Inflates or deflates the existing mesh according to
  the new radius given. All the cross sectional circles
  are changed to have radius \a NewRadius. This function
  drops an error message if there are no valid mesh points.

  The bounding box values are updated at the end of the
  process.
*/
template <class Vector>
void Tube<Vector>::scaleTubeRadius(float NewRadius) {

  if (_MeshPoints==NULL) {
    cerr << "Tube::scaleTubeRadius() : No mesh points\n";
    return;
  }

  // Move the points in _MeshPoints s.t. the tube has radius NewRadius
  // this is done by scaling the points on each circle by OldRadius/NewRadius

  float ScaleFactor = NewRadius/this->_Radius;

  Vector Tmp, Point;

  biarc_it current = this->begin();
  int count = 0;
  int N = this->_Closed?this->nodes()+1:this->nodes();

  // for each point on the curve, scale the radius of corresponding circle
  while (count<N) {
    if (current==this->end()) current = this->begin();
    Point = current->getPoint();

    // Circle loop
    for (int j=0;j<=_Segments;j++) {
      Tmp = _MeshPoints[count*(_Segments+1)+j];
      _MeshPoints[count*(_Segments+1)+j] = (Tmp - Point)*ScaleFactor + Point;
    }

    count++;
    current++;

  }

  _Radius = NewRadius;

  // update the bounding box
  computeBoundingBox();

}

/*!
  Creates a tubular mesh with radius \a R around the
  curve with N nodes and S segments on each cross
  sectional circle. If the actully stored curve has
  not exactly N nodes, the Curve::resample() routine
  is called automatically.

  The parameter \a Tol is the precision used to math up
  the mesh of a closed curve. \a Tol is the difference
  in radians of the angle between the first and the last
  normal of the sequence of curve frames.

  Use \a Tol = -1 (default) for no torsion adjustment and open
  curves.

  Computes also the mesh normals at each point where
  the normal is an average of the neighbouring face
  normals. (The normals are used for Gouraud shading
  in visualisation programs).

  At the end the center of mass and the bounding box
  are computed.

  The interpolation in case of a resampling is done
  with Gamma = 0.5.
*/

template <class Vector>
void Tube<Vector>::makeMesh(int N, int S, float R, float Tol) {

  // clean up old mesh points/normals
  clear_tube();

  int TwistFlag = 0;

  if (N<2) {
    cerr << "Warning : Tube::makeMesh() : Number of nodes must be >1!\n"
	 << "                             Let's set nodes to 2!";
    N=2;
  }
  
  if (S<3) {
    cerr << "Warning : Tube::makeMesh() : Number of segments msut be >2!\n"
	 << "                             Let's set segments to 3!\n";

    S=3;
  }

  _Segments = S;
  _Radius = R;

  if (N!=this->nodes()) {
    this->make_default();
    this->resample(N);
  }

  // In the following we have
  // S = number of circle segments
  // N = number of nodes on the curve
  // R = radius of the tube

  /*
   * Caution : first and last point are twice in the list
   * keeps the connectivity of the first and last circle (if closed tube)
   * and easier for openInventor mesh handling
   */

  int ToClose, Nloc;
  if ((Tol < 0.0) || (!this->_Closed)) {
    this->unlink();
    ToClose = 0;
    Nloc = this->nodes();
  }
  else {
    this->unlink();
    ToClose = 1;
    TwistFlag = 1;
    Nloc = this->nodes()+1;
  }

  _MeshPoints  = new Vector3[Nloc*(S+1)];
  _MeshNormals = new Vector3[Nloc*(S+1)];

  /*
   * Variable stuff
   * The first Vector stores all the normals for the Torsion smooth stuff
   * Frame Tangents, Normals, Binormals
   *
   */

  Vector3* Normals = new Vector3[Nloc];
  Vector3* Points = new Vector3[this->nodes()];
  Vector3* Tangents= new Vector3[this->nodes()];
  Vector3 Binormal, Theta;
  Vector3 ClosestDirection;
  Matrix3 Frame, tmp;
  float b, a, denom, tn, tb, Theta0, Theta1, angleDiff = 100000.0;

  // To keep track of the first and last circle points connectivity
  int PermutationIndex = 0 ;

  // Frame Twist Parameter
  float TwistSpeed = 0.0;
  int Stop = 1;
  float direction, AngularSpeedScale = 1.0;
  
  biarc_it current = this->begin();
  for (int i=0;i<this->nodes();i++) {
    *(Points+i)   = current->getPoint();
    *(Tangents+i) = current->getTangent();
    //(*(Tangents+i)).normalize(); // should already be done in data
    current++;
  }

  /*
   * Generate a smoothly changing Frame along the center line
   * Normals (according to Frame) generation (without torsion c=0)
   * Using Cayleigh rot matrix
   */

  int iterations = 0;

  // Init condition for frame convention=(normal, binormal, tangent)
  // FIXME : If Points[1]-Point[0] is parallel to Tangents[0], than we have a problem !!!
  if (Tangents[0].cross(Points[1]-Points[0]).norm()>10e-8)
    Normals[0] = Tangents[0].cross(Points[1]-Points[0]);
  else
    Normals[0] = Tangents[0].cross(Vector3(0,1,0));
  
  // check cross product problem due to zero norm
  // FIXME : Tangents[0] could be || to 0,1,0 !!!
  if (Normals[0].norm()<0.0001) Normals[0]=Tangents[0].cross(Vector3(0,0,1));
  Normals[0].normalize();
  Binormal = Tangents[0].cross(Normals[0]);
  Binormal.normalize();

  Vector3 rot_point;
  float dist;

#ifdef OSCAR // Oscar
  do {
#endif

    Frame = Matrix3(Normals[0],Binormal,Tangents[0]);

/* Start Frenet Frame code */
#ifdef FRENET // Frenet Frame

    cout << "Frenet Framing\n" << flush;
    this->link();
    Vector3 p0,p1,p2;

    current = this->begin();
    for (int i=0;i<Nloc;i++) {

      p0 = (current->getPoint()-current->getPrevious()->getPoint());
      p1 = (current->getNext()->getPoint()-current->getPrevious()->getPoint());
      p2 = p1.cross(p0);

      Normals[i] = (current->getTangent().cross(p2));
      Normals[i].normalize();

      current++;

    }

    this->unlink();

#endif // Frenet Frame
/* end Frenet Frame code */

/* Start Parallel Transport Frame ODE code */
#ifdef PARALLEL // Parallel Transport Frame ODE

    cout << "Parallel Transport ODE Framing\n" << flush;
    this->link();
    Vector3 p0,p1,p2,tt;
    Vector3 *tmpNormals = new Vector3[Nloc];

    current = this->begin();
    for (int i=0;i<Nloc;i++) {
      p0 = (current->getPoint()-current->getPrevious().getPoint());
      p1 = (current->getNext().getPoint()-current->getPrevious().getPoint());
      p2 = p1.cross(p0);
      tt = current->getTangent();
      tmpNormals[i] = (tt.cross(p2));
      tmpNormals[i].normalize();
      current++;
    }

    for (int i=1;i<Nloc;i++) {
      Normals[i] = Normals[i-1] + (Tangents[i-1].cross(tmpNormals[i-1])).cross(tmpNormals[i-1])*1.0/(float)Nloc;

      Normals[i] = Normals[i] - Normals[i].dot(Tangents[i])*Tangents[i];

      Normals[i].normalize();

    }

    if (!ToClose)
    this->unlink();

#endif // Parallel Transport Frame ODE
/* end Parallel Transport Frame ODE code */


/* start test code */
#ifdef WRITHEFRAME // Writhe Framing

    cout << "Let's get to work\n" << flush;

    this->link();

    for (int i=0;i<Nloc-1;i++) {

      Biarc<Vector3>* bi_tmp;
      Biarc<Vector3>* b_i = this->accessBiarc(i);
      Biarc<Vector3>* b_j = b_i->getNext();
      Vector3 t_plus  = Tangents[i];
      Vector3 t_minus = -Tangents[i];

      Curve<Vector3> tmp;

      Vector3 e_ij;
      while (b_j!=b_i) {
	e_ij = (b_i->getPoint()-b_j->getPoint());
	e_ij.normalize();
	tmp.append(e_ij,Vector3(0,0,0));
	b_j = b_j->getNext();
      }

      // Rot curve
      Vector3 down_dir;
      for (int k=0;k<tmp.nodes();k++) down_dir += tmp[k]->getPoint();
      down_dir/=((float)tmp.nodes());

      down_dir = down_dir - down_dir.dot(t_plus)*t_plus;

      Matrix3 align_for_proj(t_plus,down_dir,t_plus.cross(down_dir));

      tmp.rotate(align_for_proj);

      // stereo proj
      for (int k=0;k<tmp.nodes();k++) {
	Vector3 pppp = tmp[k]->getPoint();
	tmp[k]->setPoint(Vector3(pppp[0]/(1.0-pppp[2]),
				 pppp[1]/(1.0-pppp[2]),
				 0.0));
      }
      
      float MassArea = 0.0, x_bar = 0.0, y_bar = 0.0;
      // we have to 0,0 our 2d curve
      Vector3 uga = tmp[tmp.nodes()-1]->getPoint()-tmp[0]->getPoint();
      uga.normalize();
      float winkel = acos(uga.dot(Vector3(1.0,0.0,0.0)));

      float cosa = cos(winkel);
      float sina = sin(winkel);

      for (int k=1;k<tmp.nodes();k++) {
	Vector3 mass_ding = tmp[k]->getPoint()-tmp[0]->getPoint();

	// drehen
	float temp1 = mass_ding[0], temp2 = mass_ding[1];
	mass_ding[0] = (-sina*temp1 + cosa*temp2);
	mass_ding[1] = (cosa*temp1 + sina*temp2);
	mass_ding[2] = 0.0;

	MassArea += mass_ding[1];
	x_bar += mass_ding[0]*mass_ding[1];
	y_bar += mass_ding[1]*mass_ding[1];
      }

      x_bar /= MassArea;
      y_bar /= (2*MassArea);
      
      float gaga = x_bar*x_bar+y_bar*y_bar+1.0;
      Vector3 CM(2.0*x_bar/gaga,
		 2.0*y_bar/gaga,
		 (x_bar*x_bar+y_bar*y_bar-1.0)/gaga);


      Normals[i] = CM - CM.dot(t_plus)*t_plus;
      
    }

this->unlink();
    cout << "New part is done.\n" << flush;

#endif // if Writhe Framing end
/* end test code */

#ifdef OSCAR // Oscar
    /*
     * Propagating the initial Normal[0] along the curve
     * the result is a sequence of smoothly chaning normal vectors
     * at each curve node
     */
    for (int i=1;i<Nloc;i++) {

      b = (1.0 - (Frame[2].dot(Tangents[i%this->nodes()])));
      a = (2.0 - b)/(2.0 + 0.5*TwistSpeed*TwistSpeed);
      if (a < 1e-5) cerr << "generateMesh() : possible div by 0.\n";
      denom = a*(1.0+0.25*TwistSpeed*TwistSpeed);

      tn = Tangents[i%this->nodes()].dot(Frame[0]);
      tb = Tangents[i%this->nodes()].dot(Frame[1]);

      Theta0 = (0.5*TwistSpeed*tn - tb)/denom;
      Theta1 = (0.5*TwistSpeed*tb + tn)/denom;

      Theta = Vector3(Theta0,Theta1,TwistSpeed);

      // Get next local frame
//      Frame = (tmp.cay(Theta)*Frame);
      Frame = (Frame*tmp.cay(Theta));
      
      Normals[i] = Frame[0];
      Normals[i].normalize();  

    }

    /*
     * Only If the tube is closed
     *
     * Compute the angle between the first normal Normals[0]
     * and the direction from the center of the
     * circle to the closest point on the circle
     * given by the last normal Normals[N]
     * 
     * O = circle center
     * Search for closest point to O+Normals[0]
     * on the circle defined by Normals[N]
     *
     * This is the ClosestDirection vector. The angle between
     * ClosestDirection and Normals[0] must -> zero
     * by arranging the torsion speed !
     *
     * at the end the points to be connected on circle
     * N-1 and circle 0 must be identified
     *
     */

    if (!TwistFlag)
      Stop = 0;
    else {
      ClosestDirection = Normals[Nloc-1];
      dist = (ClosestDirection-Normals[0]).norm();
/* XXX Disable Permutation stuff (which minimizes internal twist)
       but this is specially for visualization and texturing painfull!
      for (int j=0;j<S;j++) {
	// trigonometric orientation of the point on the circle !!!
	rot_point = Normals[Nloc-1].rotPtAroundAxis(2.0*M_PI/(float)S*(float)j,
						    Tangents[0]);
	if ((rot_point-Normals[0]).norm()<dist) {
	  if (iterations==0) {
	    ClosestDirection = rot_point;
	    PermutationIndex = j;
	  }
	  else if (iterations>0 && j==PermutationIndex)
	    ClosestDirection = rot_point;
	}
      }
*/
// Set the PermutationIndex to 0
PermutationIndex = 0;

#ifdef __debug__
      cout << "Angle diff for iteration " << iterations << " : " << angleDiff
	   << " and permutation index " << PermutationIndex << endl;
#endif

      // angle difference between first normal and closest last direction
      if (Normals[0].dot(ClosestDirection) > 1-Tol) Stop=0;

      else {
	angleDiff = acos(Normals[0].dot(ClosestDirection));
	// get rotation handedness to correct the mesh 
	direction = (ClosestDirection.cross(Normals[0])).dot(Tangents[0]) ;
	AngularSpeedScale = angleDiff/Nloc; // maybe /10.0

	if (direction>0)
	  TwistSpeed+=AngularSpeedScale;
	else
	  TwistSpeed-=AngularSpeedScale;

	iterations++;
	if (iterations > 10) {
	  cerr << "Warning : 10 iterations. Stopped with angle difference: "
	       << angleDiff << " rad." << endl;
	  Stop = 0;
	}
      }
    }
  } while (Stop);

#ifdef __debug__
  cout << "Mesh adjustment iterations : " << iterations-1 << endl;
#endif

#endif // if Oscar

  // Generate all the mesh points and normals
  Vector3 n, dummy;

  // open tube case
  if (!TwistFlag) {
    for (int i=0;i<Nloc;i++) {
      for (int j=0;j<=S;j++) {
	dummy = Normals[i].rotPtAroundAxis(-2.0*M_PI/(float)S*(float)(j%S),
					   Tangents[i]);
	dummy.normalize();

	_MeshNormals[i*(S+1)+j] = dummy;
	_MeshPoints[i*(S+1)+j] = (R*dummy) + Points[i];
      }
    }
  }
  // closed tube case
  else {
    for (int i=0;i<Nloc;i++) {
      for (int j=0;j<=S;j++) {
	dummy = Normals[i].rotPtAroundAxis(-2.0*M_PI/(float)S*(float)(j%S),
					   Tangents[i]);
	dummy.normalize();

	_MeshNormals[i*(S+1)+j] = dummy;
	_MeshPoints[i*(S+1)+j] = (R*dummy) + Points[i];

	if (i==0)
	  _MeshPoints[this->nodes()*(S+1)+j] = (R*dummy) + Points[0];
      }
    }

#ifdef OSCAR // Oscar
    /*
     * Permutation of the points on the last/first circle
     * so that they match properly according to the PremutationIndex
     * found higher up
     */

    Vector3 perm[S], permN[S];

    for (int i=0;i<S;i++) {
      perm[i]  = _MeshPoints[(i+PermutationIndex)%S];
      permN[i] = _MeshNormals[(i+PermutationIndex)%S];
    
    }

    for (int i=0;i<(S+1);i++) {
      _MeshPoints[this->nodes()*(S+1)+i]  = perm[i%S];
      _MeshNormals[this->nodes()*(S+1)+i] = permN[i%S];
    }
#endif // if Oscar
  }

  if (ToClose)
    this->link();

  computeBoundingBox();

  // Clean up the mess
  if (Normals)  { delete[] Normals;  Normals = NULL; }
  if (Points)   { delete[] Points;   Points = NULL; }
  if (Tangents) { delete[] Tangents; Tangents = NULL; }

}

#ifdef RENDERMAN

/*!
  TODO : doc for Pixie init func
*/
template <class Vector>
void Tube<Vector>::renderman_init() {

  // NU = this->nodes()+1;//this->isClosed()?this->nodes():this->nodes()+1;
  NU = this->isClosed()?this->nodes()+1:this->nodes();
  NV = this->_Segments+1;

  MeshPoints = new RtPoint[NV*NU];
  MeshNormals = new RtPoint[NV*NU];

  for (int i=0;i<NU;i++) {
    for (int j=0;j<NV;j++) {
      
      MeshPoints[j*NU+i][0] = meshPoint(i*NV+j)[0];
      MeshPoints[j*NU+i][1] = meshPoint(i*NV+j)[1];
      MeshPoints[j*NU+i][2] = meshPoint(i*NV+j)[2];
      
      MeshNormals[j*NU+i][0] = meshNormal(i*NV+j)[0];
      MeshNormals[j*NU+i][1] = meshNormal(i*NV+j)[1];
      MeshNormals[j*NU+i][2] = meshNormal(i*NV+j)[2];
    }
  }  
};

/*!
  TODO : doc for Pixie draw func
*/
template <class Vector>
void Tube<Vector>::renderman_draw() {
  RiPatchMesh(RI_BILINEAR,
              NU,RI_NONPERIODIC,
	      NV,RI_NONPERIODIC,
	      RI_P,(RtPointer)MeshPoints,
	      RI_N,(RtPointer)MeshNormals,
	      RI_NULL);
};

template <class Vector>
void Tube<Vector>::exportRIBFile(char* filename, int W, int H,
                                 const Vector &p, const Vector &a, float angle,
                                 const Vector &light_dir) {
  if (!MeshPoints) renderman_init();
  RiBegin(filename);
  RiOrientation(RI_RH);
  RiShadingRate(1.0);
  RiPixelSamples(4,4);
  RiFormat(W,H,1.0);
  RiCropWindow(0.0,1.0,0.0,1.0);
  RtFloat fov = 50;

  RtString strtmp = ".:%PIXIEHOME%/displays";
  RiOption( "searchpath","string display",&strtmp,RI_NULL);

  RiProjection("perspective","fov",&fov,RI_NULL);
  RiDisplay("output.tif",RI_FILE,RI_RGB,RI_NULL);
  RtPoint from = {0,0,0}, to = {-light_dir[0],-light_dir[1],-light_dir[2]};
  RtFloat intensity = 1;
  RtColor light_color = { 1,1,1 };
  // Setup Lights
  RiLightSource("distantlight","from",&from,"to",&to);//,
//                "intensity",&intensity,"lightcolor",&light_color);

  // Setup Camera
  RiRotate(-angle/M_PI*180.,-a[0],-a[1],a[2]);
  RiTranslate(-p[0],-p[1],p[2]);
  
  RiWorldBegin();
  // FIXME : colors ? other shaders ?
  // Inventor and RenderMan do not have the sym coord system
  RiScale(1,1,-1);
  RiSurface("plastic",RI_NULL);
  renderman_draw();
  RiWorldEnd();
  RiEnd();
}

#endif // RENDERMAN

#endif // __TUBE_SRC__
