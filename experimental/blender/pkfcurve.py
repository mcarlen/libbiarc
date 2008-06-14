from Blender import *
# from pkf import *
# FIXME workaround for loading the pkf helper
execfile('/home/carlen/blender/pkf.py')

file = open("/tmp/curve.pkf")
pkfread(file)

editmode = Window.EditMode()    # are we in edit mode?  If so ...
if editmode: Window.EditMode(0) # leave edit mode before getting the mesh

# The Blender Beveling tool has some scaling
# problems if the first point is not in 0,0,0
def shiftcoord(c,d):
  return [c[0]-d[0],c[1]-d[1],c[2]-d[2],1]

curve = Curve.New('PkfCurve')
curve.setFlag(3)
curve.appendNurb(shiftcoord(coords[0],coords[0]))
nurb = curve[0]
nurb.setType(0)
for i in xrange(1,len(coords)):
  nurb.append(shiftcoord(coords[i],coords[0]))

scn = Scene.GetCurrent()
ob = scn.objects.new(curve, 'PkfCurve') #PkfName)

if editmode: Window.EditMode(1)

Redraw()