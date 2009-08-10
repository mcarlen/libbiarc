#!/usr/bin/python

from math import sqrt, pi, sin

class LinearInterpolator:
  def __init__(self, x, y):
    self.y = y[:]
    self.x = x[:]
    self.L = sum( [ sqrt((y[i]-y[i-1])**2+(x[i]-x[i-1])**2) for i in xrange(1,len(x)) ] )
    # not closed here
    # self.L += sqrt((y[0]+1.-y[-1])**2 + (x[0]+1.-x[-1])**2)
    
  def __repr__(self):
    str = ""
    for x,y in zip(self.x,self.y):
      str += "%f %f\n" % (x,y)
    return str

  def __call__(self, s):
    if s < 0 or s > 1:
      print 's out of bounds s=', s
      return (0,0)
    i = 0
    while s > self.x[i] and i < len(x):
      i = i + 1
    return (s - x[i-1])/(x[i] - x[i-1])*(y[i] - y[i-1])

  def seglength(self, s):
    if s < 0 or s > self.L:
      print 's out of bounds s=', s, 'L=',self.L
      return (0,0)
    length = 0
    i = 0
    while i+1 < len(x) and length < s:
      i = i + 1
      length += sqrt( (y[i]-y[i-1])**2 + (x[i]-x[i-1])**2 )
    ds = 0
    if i == len(self.x)-1 and length < s:
      ds = sqrt( (y[0]+1-y[-1])**2 + (x[0]+1-x[-1])**2 )
      length += ds
    else:
      ds = sqrt( (y[i]-y[i-1])**2 + (x[i]-x[i-1])**2 )
    frac = (s - length + ds)/ds
    return (x[i-1] + frac*(x[i]-x[i-1]), y[i-1] + frac*(y[i]-y[i-1]))

  def sample(self, N = 100):
    return map(self.seglength, [ float(i)*self.L/float(N-1) for i in xrange(N) ])

__TEST__ = False # True

N = 20
x = []
y = []

from sys import argv,exit
if len(argv) != 3:
  print "prog file.txt Nsamples"
  exit(0)

f = open(argv[1])
N = int(argv[2])
y0old = -1
for l in f.readlines():
  x0, y0 = map(lambda x: float(x), l.split())
#  if x0 > y0:
#    y0 = y0 + 1
#  if y0 > y0old:
  x += [ x0 ]
  y += [ y0 ]
  y0old = y0
f.close()

inter = LinearInterpolator(x,y)
sampling = inter.sample(N)

for s in sampling:
  if s[1] > 1.:
    print s[0],s[1]-1, 0
  else:
    print s[0],s[1], 0

