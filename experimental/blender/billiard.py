from Blender import *
from Blender.Mathutils import Vector
from os import popen2, listdir
from math import sin,cos,pi, sqrt

struts="/tmp/bill"

SScale  = 1
SRadius = 0.001
SSegs   = 72

file = open(struts)
list = []
count=0
for l in file.readlines():
  x0,y0,z0 = l.strip().split()
  list += [[float(x0)*SScale,float(y0)*SScale,float(z0)*SScale,1.0]]

scene = Scene.GetCurrent()

def docyl(S,R,Pts):
  coords = []
  N = len(Pts)
  for i in xrange(N):

    prev = Vector(Pts[(i-1)%N][0],Pts[(i-1)%N][1],Pts[(i-1)%N][2])
    cur  = Vector(Pts[i][0],Pts[i][1],Pts[i][2])
    next = Vector(Pts[(i+1)%N][0],Pts[(i+1)%N][1],Pts[(i+1)%N][2])

    t = next - prev
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

      c0 = cur + vec
      coords += [[ c0[0],c0[1],c0[2] ]]

  faces = []
  for j in xrange(N):
    for i in xrange(S):
      faces += [[ S*j + i, S*j + (i + 1)%S, S*((j+1)%N) + (i+1)%S, S*((j+1)%N) + i ]]

  me = Mesh.New('Billiard')
  me.verts.extend(coords)
  me.faces.extend(faces)
  Scene.GetCurrent().objects.new(me)

docyl(SSegs,SRadius,l)

Redraw()
