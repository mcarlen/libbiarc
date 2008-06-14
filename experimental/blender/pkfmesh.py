# Given a pkf file, generate a mesh for blender
# XXX There are several double coordinates. Remove them
# with the "Rem Double" in Blender Edit Mode!
# Then enable Gouraud shading with "Set Smooth"
# and a subsurf modifier smoothens a coarse mesh even more!

from Blender import *
from os import popen2

pkffile = "/home/carlen/work/libbiarc/knots/k8.18.pkf"

# Here we use the biarc client included in the libbiarc tools directory
# to recover a mesh
cli = popen2("biarccli %s" % pkffile)
cli[0].write('closed\n')
cli[0].flush()
cli[1].readline()
cli[0].write('mesh:100:12:0.0083\n')
cli[0].flush()

# Skip first dot '.'
cli[1].readline()

coords = []
for i in xrange(101):
  for j in xrange(13):
    x,y,z = map(lambda v: float(v),cli[1].readline().strip().split())
    coords += [[x,y,z]]

cli[0].write('exit\n')
del cli

def idx(i,j):
  return (i)*13+j

faces = []
for i in xrange(100):
  for j in xrange(12):
    faces += [[idx(i,j),idx(i+1,j),idx(i+1,j+1),idx(i,j+1)]]

editmode = Window.EditMode()
if editmode: Window.EditMode(0)

me = Mesh.New('myMesh')

me.verts.extend(coords)
me.faces.extend(faces)

scn = Scene.GetCurrent()   # link object to current scene
ob = scn.objects.new(me, 'PKFMesh')

if editmode: Window.EditMode(1)  # optional, just being nice
	
Redraw()
