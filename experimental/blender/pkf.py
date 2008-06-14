PkfName  = 'void'
KnotType = 'void'
NCMP = 0
COMP = 0
coords = []
tangents = []
edges = []

def pkfread(file):
  global PkfName,KnotType,NCMP,COMP
  global coords,tangents,edges

  if file.readline().strip()!='PKF 0.2':
    print "Not PKF Version 0.2"
    exit(1)
  KnotType, PkfName = file.readline().strip().split()
    
  # Read ETIC,HIST and CITE
  for i in xrange(3):
    token,val = file.readline().strip().split(" ",1)
    if val>0:
      while file.readline().strip()!='END':
        continue
    else:
      file.readline()
  
  token,NCMP = file.readline().strip().split()
  if token!='NCMP':
    print "Expected NCMP! Got : "+token
    exit(1)

  for i in xrange(int(NCMP)):
    token,COMP = file.readline().strip().split()
    if token!='COMP':
      print "Expected COMP! Got : "+token
      exit(1)
    if KnotType == 'BIARC_KNOT':
      for i in xrange(int(COMP)):
        token,x,y,z,tx,ty,tz = file.readline().strip().split()
        coords.append([10.*float(x),10.*float(y),10.*float(z),1.0])
        tangents.append([float(tx),float(ty),float(tz)])
        if i>0:
          edges.append([i,i-1])
    else:
      for i in xrange(int(COMP)):
        token,x,y,z = file.readline().strip().split()
        coords.append([float(x),float(y),float(z),1.0])
        if i>0:
          edges.append([i,i-1])
      
    token = file.readline().strip()
    if token!='END':
      print "Expected END : Got "+token
      exit(1)