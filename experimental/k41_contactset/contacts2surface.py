#!/usr/bin/python

import sys
from vector import vector

contacts = []

def tovec(v):
  a0,a1,a2,b0,b1,b2,c0,c1,c2 = map(float, v.split())
  return [ vector([a0,a1,a2]), vector([c0,c1,c2]) ]

def invertIfNecessary(c0, c1):
  d0 = (c1[0] - c0[0]).length()
  d1 = (c1[1] - c0[0]).length()
  if d1 < d0:
    return [ c1[1], c1[0] ]
  else:
    return c1

f = open(sys.argv[1])

for i in f.readlines():
  contacts.append(tovec(i))

""" construct the surface """
idx = 1
last_contact = []
def triangulate(contact0, contact1, N = 10):
  global idx
  global last_contact

  if last_contact:
    contact0 = invertIfNecessary(last_contact, contact0)
  contact1 = invertIfNecessary(contact0, contact1)
  last_contact = contact1

  d0 = contact0[1]-contact0[0]
  d1 = contact1[1]-contact1[0]
  for i in xrange(N-1):
    s0 = float(i)/float(N-1)
    s1 = float(i+1)/float(N-1)
    print "v", contact0[0] + d0*s0
    print "v", contact0[0] + d0*s1
    print "v", contact1[0] + d1*s0
    print "v", contact1[0] + d1*s1
    print "f", idx, idx+2, idx+1
    print "f", idx+2, idx+3, idx+1
    idx = idx + 4

""" write obj file """
for i in xrange(len(contacts)-1):
  triangulate(contacts[i],contacts[i+1],20)

