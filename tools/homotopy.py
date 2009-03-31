#!/usr/bin/python

import sys

if len(sys.argv) != 3:
  print "Usage : "+sys.argv[0]+" contacts homotopy_val[0:1]"
  sys.exit(1)

val  = float(sys.argv[2])
file = open(sys.argv[1],"r")

# original curve
points = []
for line in file.readlines():
  p = map(lambda(x): float(x), line.strip().split())
  points += [ [ (p[i] + (p[i+3] - p[i])*val) for i in range(3) ] ]
file.close()

file = open("out.pkf", "w")
print >>file, "PKF 0.2\nBIARC_KNOT homotopy curve\nETICL 11\nETIC homotopy.py\nEND"
print >>file, "CITEL 0\nEND\nHISTL 0\nEND\nNCMP 1";
print >>file, "COMP", len(points)
for p in points:
	print >>file, "NODE", p[0], p[1], p[2], " 1 0 0"
print >>file, "END"
file.close()

