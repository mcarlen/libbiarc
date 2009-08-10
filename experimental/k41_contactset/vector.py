#!/usr/bin/python

import math
from math import sqrt, sin, cos, acos, asin

class vector:
   def __init__(self,*lst):
      self.data =[]
      if type(lst[0]) != type(1) and type(lst[0]) != type(1.0):
         lst = lst[0]
      for x in lst:
         self.data.append(x)
      self.dim = len(lst)

   def __repr__(self):
      # return "<" + ",".join(map(lambda x: ("%f" % x), self.data)) + ">" + (" (%f)" % self.length())
      return " ".join(map(lambda x: ("%f" % x), self.data))

   def __len__(self):
      return self.dim

   def __setitem__(self,i, value):
      self.data[i] = value

   def __getitem__(self,i):
      return self.data[i]

   def __add__(self,other):
      if len(other)!=self.dim:
         raise ValueError("Dimension mismatch")
      v = []
      for i in range(len(other)):
         v.append(self[i]+other[i])
      return vector(v)

   def __neg__(self):
      return self.smul(-1)

   def __sub__(self,other):
      return self + other.smul(-1)

   def __rmul__(self,other):
      return self.smul(other)

   def __lmul__(self,other):
      return self.smul(other)

   def __mul__(self,other):
      return self.smul(other)

   def __div__(self,other):
      return self.sdiv(other)

   def sdiv(self,r):
      return self.smul(1.0/r)

   def smul(self,r):
      v = []
      for x in self.data:
         v.append(r*x)
      return vector(v)

   def dot(self,other):
      sum = 0.0
      if len(other)!=self.dim:
         raise ValueError("Dimension mismatch")
      for i in range(len(other)):
         sum += self[i]*other[i]
      return sum

   def length(self):
        return math.sqrt(self.dot(self))

   def norm2(self):
           return self.dot(self)

   def normalize(self):
      l = self.length()
      if l == 0.0:
         return self
      for i in range(self.dim):
         self.data[i]=self.data[i] / l
      return self

   def as_lst(self):
      return self.data

def cross(x,y):
   assert len(x) == 3 and len(y)==3
   return vector([x[1]*y[2]-x[2]*y[1],x[2]*y[0]-x[0]*y[2],x[0]*y[1]-x[1]*y[0]])

