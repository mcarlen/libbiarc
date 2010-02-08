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

filename="/home/carlen/work/libbiarc/tools/bill_vertices.txt"

res=3
rad=0.1

file = open(filename)

def doSphere(coords, subdiv=2,radius=1):
  me = Mesh.Primitives.Icosphere(subdiv, radius)
  ob = Scene.GetCurrent().objects.new(me,'Mesh')
  ob.loc = coords
  return ob

for l in file.readlines():
  doSphere(map(float,l.strip().split()), res, rad)

file.close()

Window.RedrawAll()
