from Blender import *
from os import popen2, listdir

# Parameters
T_N = Draw.Create(100)
T_S = Draw.Create(12)
T_R = Draw.Create(0.2)
T_File = Draw.Create("/tmp/pkf")

# Global Variables
gMesh = 0
gFrameCounter = 0

# Events
EVENT_NOEVENT = 1
EVENT_DRAW    = 2
EVENT_EXIT    = 3
EVENT_FILE    = 4

def draw():
  global T_N, T_S, T_R, T_File
  global EVENT_NOEVENT,EVENT_DRAW,EVENT_EXIT

  ########## Titles
  BGL.glClear(BGL.GL_COLOR_BUFFER_BIT)
  BGL.glRasterPos2d(8, 103)
  Draw.Text("PKF Mesh Loader Script")

  ######### Parameters GUI Buttons
  BGL.glRasterPos2d(8, 83)
  Draw.Text("Parameters:")
  T_N = Draw.Number("Points: ", EVENT_NOEVENT, 10, 75, 210, 18,
                    T_N.val, 3, 200, "Number of points along the curve");
  T_S = Draw.Number("Segments: ", EVENT_NOEVENT, 10, 55, 210, 18,
                    T_S.val, 3, 36, "Number of cross section segments");
  T_R = Draw.Slider("Radius: ", EVENT_NOEVENT, 10, 35, 210, 18,
                    T_R.val, 0.001, 1, 1, "Radius of the tube");
  T_File = Draw.String("PKF Directory: ", EVENT_NOEVENT, 10, 95, 210, 18, T_File.val, 200)

  ######### Draw and Exit Buttons
  Draw.Button("Draw",EVENT_DRAW , 10, 10, 80, 18)
  Draw.Button("Exit",EVENT_EXIT , 140, 10, 80, 18)

def event(evt, val):
  if ((evt == Draw.QKEY or evt == Draw.ESCKEY) and not val):
    Draw.Exit()

def getfile(name):
  T_File.val = name
  Draw.Redraw()

def bevent(evt):
  global T_N, T_S, T_R
  global EVENT_NOEVENT,EVENT_DRAW,EVENT_EXIT

  ######### Manages GUI events
  if (evt == EVENT_EXIT):
    Draw.Exit()
  elif (evt == EVENT_DRAW):
    for f in listdir(T_File.val):
      if f[-4:]=='.pkf':
        print 'Load ',f
        mkpkfmesh(T_File.val+'/'+f,T_N.val,T_S.val,T_R.val)
    if gMesh.key.ipo == None:
      gMesh.key.ipo = Ipo.New('Key','KeyIpo')
    ipo = gMesh.key.ipo
    count = 0
    for key in ipo.curveConsts:
      ipo.addCurve(key)
      icurve = ipo[key]
      icurve.interpolation = IpoCurve.InterpTypes['CONST']
      icurve.append((count*1,0))
      icurve.append(((count+1)*1,1))
      icurve.append(((count+2)*1,0))
      count+=1
  elif (evt == EVENT_FILE):
	Window.FileSelector(getfile)

Draw.Register(draw, event, bevent)

def mkpkfmesh(pkffile,N,S,R):
  global gFrameCounter, gMesh

  # Here we use the biarc client included in the libbiarc tools directory
  # to recover a mesh
  cli = popen2("biarccli %s" % pkffile)
  if cli[1].readline().strip()!='OK':
    return
  cli[0].write('closed\n')
  cli[0].flush()
  cli[1].readline()
  cli[0].write("mesh:%i:%i:%f\n" % (N,S,R) )
  cli[0].flush()

  # Skip first dot '.'
  cli[1].readline()

  coords = []
  for i in xrange(N):
    for j in xrange(S+1):
      x,y,z = map(lambda v: float(v),cli[1].readline().strip().split())
      if j==S: continue
      coords += [ [x,y,z] ]

  cli[0].write('exit\n')
  del cli

  def idx(i,j):
    return (i)*S+j

  faces = []
  if gFrameCounter==0:
    for i in xrange(N-2):
      for j in xrange(S):
        faces += [[idx(i,j),idx(i,(j+1)%S),
                   idx(i+1,(j+1)%S),idx(i+1,j)]]

    # Glue the Ends
    def distance(a,b):
      x,y,z=a[0]-b[0],a[1]-b[1],a[2]-b[2]
      return (x*x+y*y+z*z)

    # Find permutation index
    pidx = 0
    cdist = distance(coords[0],coords[idx(N-1,0)])
    for i in xrange(1,S):
      d = distance(coords[i],coords[idx(N-1,0)])
      if d<cdist:
        cdist = d
        pidx = i

    for j in xrange(S):
      faces += [[ idx(N-2,j),idx(N-2,(j+1)%S),
                  idx(0,(j+1+pidx)%S),idx(0,(j+pidx)%S) ]]

  if gFrameCounter==0:
    gMesh = Mesh.New('pkfMesh')
    gMesh.verts.extend(coords[:S*(N-1)])
    gMesh.faces.extend(faces)
    gMesh.insertKey()

    scn = Scene.GetCurrent()   # link object to current scene
    ob = scn.objects.new(gMesh, 'pkfMesh')
  else:
    for i in xrange(S*(N-1)):
      gMesh.verts[i] = Mesh.MVert(*coords[i])
    gMesh.insertKey()

  gFrameCounter+=1

  Redraw()