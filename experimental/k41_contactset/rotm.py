#!/usr/bin/python

from math import pi,sin,cos
import sys

def rot45(v):
  vf = map(float,v.split())
  return [cos(-pi/4.)*vf[0] - sin(-pi/4.)*vf[1],
          sin(-pi/4.)*vf[0] + cos(-pi/4.)*vf[1]]

p = []
f = open(sys.argv[1])
for i in f.readlines():
  p += [ rot45(i) ]

for i in p:
  print i[0],i[1]
