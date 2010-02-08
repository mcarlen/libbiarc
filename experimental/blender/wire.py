"""
  Use this script to generate a *thick* wireframe with spheres
  as joints. The input file (currently /tmp/wire.txt) is the
  output file from the tool pkf2mesh!

  Adjust SRadius for the cylinder size. The sphere size is
  6*SRadius, change it as well if needed.
"""

from Blender import *
from Blender.Mathutils import Vector
from os import popen2, listdir
from math import sin,cos,pi, sqrt
from sys import exit

struts="/tmp/wire.txt"

SScale  = 10
SRadius = 0.045

file = open(struts)
list = []
count=0

# Read in number of nodes along the curve
N, stuff      = file.readline().split('#', 1)
N = int(N)
SSegs, stuff  = file.readline().split('#', 1)
SSegs = int(SSegs)

for l in file.readlines():
  x0,y0,z0 = l.strip().split()
  list += [[float(x0)*SScale,float(y0)*SScale,float(z0)*SScale,1.0]]

assert(len(list)==(SSegs+1)*(N+1))

scene = Scene.GetCurrent()

def dojoint(coords, subdiv=2,radius=1):
  me = Mesh.Primitives.Icosphere(subdiv, radius)
  ob = Scene.GetCurrent().objects.new(me,'Mesh')
  ob.loc = coords
  return ob

def docyl(S,R,Pts):
  coords = []
  p0 = Vector(Pts[0][0],Pts[0][1],Pts[0][2])
  p1 = Vector(Pts[1][0],Pts[1][1],Pts[1][2])
  t = Vector(Pts[1][0]-Pts[0][0], Pts[1][1]-Pts[0][1], Pts[1][2]-Pts[0][2])
  t.normalize()
  if t*Vector(1,0,0) < 0.8:
    n = Vector(1,0,0)-(Vector(1,0,0)*t)*t
  else:
    n = Vector(0,1,0)-(Vector(0,1,0)*t)*t
  n.normalize()

  b = Vector(t[1]*n[2]-t[2]*n[1],t[2]*n[0]-t[0]*n[2],t[0]*n[1]-t[1]*n[0])
  b.normalize()

  for i in xrange(S):
    angle = float(i)*2.*pi/float(S)
    vec = cos(angle)*R*n + sin(angle)*R*b

    c0 = p0 + vec
    c1 = p1 + vec
    coords += [[ c0[0],c0[1],c0[2] ]]
    coords += [[ c1[0],c1[1],c1[2] ]]

  faces = []
  for i in xrange(S):
    faces += [[ 2*i, (2*i+2)%(2*S), (2*i+3)%(2*S),(2*i+1)%(2*S) ]]

  # midpoints
  coords += [[ p0[0],p0[1],p0[2] ]]
  coords += [[ p1[0],p1[1],p1[2] ]]

  cid0 = len(coords)-2
  cid1 = len(coords)-1
  for i in xrange(S):
    idx = []
    idx2 = []
    idx.append(2*i+0)
    idx.append(cid0)
    idx.append(2*((i+1)%S))
    idx2.append(2*i+1)
    idx2.append(2*((i+1)%S)+1)
    idx2.append(cid1)
    faces += [ idx ]
    faces += [ idx2 ]
  print faces

  me = Mesh.New('Strut')
  me.verts.extend(coords)
  me.faces.extend(faces)

  # Add new object to scene and return it
  return Scene.GetCurrent().objects.new(me)

# Do rings only
for i in xrange(0,len(list)-SSegs-1,SSegs+1):
  for j in xrange(SSegs):
    l = [ list[i+j], list[i+j+1] ]
    ob1 = docyl(10*SSegs,SRadius*1.5,l)

    # put joints
    ob2 = dojoint(list[i+j][0:3], 4, SRadius*4)

    # boolean unite them
    # mods = ob1.modifiers
    # mod = mods.append(Modifier.Types.BOOLEAN)
    # mod[Modifier.Settings.OBJECT] = ob2
    # mod[Modifier.Settings.OPERATION] = 1

    # connect rings
    l = [ list[i+j], list[(i+j+SSegs+1)%(N*(SSegs+2))] ]
    docyl(10*SSegs,SRadius*1.2,l)

  # print "done ", i
  # break

# put spheres as joints
"""
for i in xrange(len(list)):
  if i!=0 and i%(SSegs+1)==0:
    continue
  me = Mesh.Primitives.Icosphere(2, SRadius*6)
  ob = Object.New('Mesh')
  ob.loc = list[i][0:3]
  ob.link(me)
  sc = Scene.GetCurrent()
  sc.link(ob)
"""

Window.RedrawAll()