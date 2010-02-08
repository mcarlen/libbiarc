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

subdiv = 2
scale = 16.102
radius = .01
SS = 12
SR = .001

cylmesh = 0

def docyl(S,R,P0,P1):
  coords = []
  p0 = Vector(P0[0],P0[1],P0[2])
  p1 = Vector(P1[0],P1[1],P1[2])
  t = Vector(P1[0]-P0[0], P1[1]-P0[1], P1[2]-P0[2])
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
    faces += [[ 2*i,(2*i+1)%(2*S),(2*i+3)%(2*S),(2*i+2)%(2*S) ]]

  me = Mesh.New('Strut')
  me.verts.extend(coords)
  me.faces.extend(faces)
  return Scene.GetCurrent().objects.new(me)

me = Mesh.Primitives.Icosphere(subdiv, scale*radius)
ob = Scene.GetCurrent().objects.new(me,'FollowPoint')

ipo = Ipo.New("Object", "FollowIpo")
ob.setIpo(ipo)

curvex = ipo.addCurve('LocX')
curvex.interpolation = IpoCurve.InterpTypes['CONST']
curvey = ipo.addCurve('LocY')
curvey.interpolation = IpoCurve.InterpTypes['CONST']
curvez = ipo.addCurve('LocZ')
curvez.interpolation = IpoCurve.InterpTypes['CONST']

f = open("/tmp/coords")
count = 0
x1,y1,z1 = 0,0,0
for i in f.readlines():
  x0,y0,z0, x1,y1,z1 = map(float,i.split())
  curvex.append((count,scale*x0))
  curvey.append((count,scale*y0))
  curvez.append((count,scale*z0))
  
  cylobj = docyl(SS,scale*SR,[x0*scale,y0*scale,z0*scale],[x1*scale,y1*scale,z1*scale])
  cylipo = Ipo.New("Object", "CylIpo")
  cylobj.setIpo(cylipo)
  icurve = cylipo.addCurve('Layer')
  icurve.interpolation = IpoCurve.InterpTypes['CONST']
  icurve.append((count-1,2))
  icurve.append(((count),1))
  icurve.append(((count+1),2))
  
  count = count + 1

me2 = Mesh.Primitives.Icosphere(subdiv, scale*radius)
ob2 = Scene.GetCurrent().objects.new(me,'FixedPoint')
ob2.setLocation(x1*scale,y1*scale,z1*scale)

Window.RedrawAll()

