"""
  Use this script to generate a *thick* wireframe with spheres
  as joints. The input file (currently /tmp/wire.txt) is the
  output file from the tool pkf2mesh!

  Adjust SRadius for the cylinder size. The sphere size is
  6*SRadius, change it as well if needed.
"""

from Blender import *

subdiv = 2
scale = 16.102
radius = .01

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
for i in f.readlines():
  x,y,z = map(float,i.split())
  curvex.append((count,scale*x))
  curvey.append((count,scale*y))
  curvez.append((count,scale*z))
  count = count + 1

Window.RedrawAll()