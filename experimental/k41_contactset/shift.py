#!/usr/bin/python
import sys
f = open(sys.argv[1])
for i in f.readlines():
  x,y = map(float,i.split())
  x = x + .25
  y = y + .25
  if x>1:
    x = x - 1
  if y>1:
    y = y - 1
  if y>x:
    print x,y
  else:
    print y,x
f.close()
