from Blender import *
from Blender.Mathutils import *

editmode = Window.EditMode()    # are we in edit mode?  If so ...
if editmode: Window.EditMode(0) # leave edit mode before getting the mesh

# define vertices and faces for a pyramid
coords=[ [-1,-1,-1], [1,-1,-1], [1,1,-1], [-1,1,-1], [0,0,1] ]  
faces= [ [3,2,1,0], [0,1,4], [1,2,4], [2,3,4], [3,0,4] ]
edges = [ [0,1], [1,2], [2,3], [3,0] ]

#me = Mesh.New('myMesh')   # create a new mesh

#me.verts.extend(coords)   # add vertices to mesh
#me.edges.extend(edges)
# me.faces.extend(faces)    # add faces to the mesh (also adds edges)

#me.vertexColors = 1# enable vertex colors 
#me.faces[1].col[0].r = 255# make each vertex a different color
#me.faces[1].col[1].g = 255
#me.faces[1].col[2].b = 255

#scn = Scene.GetCurrent()   # link object to current scene
#ob = scn.objects.new(me, 'myObj')

if editmode: Window.EditMode(1)  # optional, just being nice
	
#Redraw()
	
bezlist = [ [ Vector([0,0,0]), Vector([1,1,0]), Vector([2,0,0]) ] ]

def bezList2Curve(bezier_vecs):
  '''
  Take a list or vector triples and converts them into a bezier curve object
  '''
  
  def bezFromVecs(vecs):
    '''
    Bezier triple from 3 vecs, shortcut functon
    '''
    bt= BezTriple.New(vecs[0].x, vecs[0].y, vecs[0].z,vecs[1].x, vecs[1].y, vecs[1].z,vecs[2].x, vecs[2].y, vecs[2].z)
    
    bt.handleTypes= (BezTriple.HandleTypes.FREE, BezTriple.HandleTypes.FREE)
    
    return bt
  
  # Create the curve data with one point
  cu= Curve.New()
  # We must add with a point to start with
  cu.appendNurb(bezFromVecs(bezier_vecs[0]))
  # Get the first curve just added in the CurveData
  cu_nurb= cu[0]
  
  # skip first vec triple because it was used to init the curve
  i = 1
  while i<len(bezier_vecs):
    bt_vec_triple = bezier_vecs[i]
    bt = bezFromVecs(bt_vec_triple)
    cu_nurb.append(bt)
    i+=1
  
  # Add the Curve into the scene
  scn= Scene.GetCurrent()
  ob = scn.objects.new(cu)
  return ob

bezList2Curve(bezlist)