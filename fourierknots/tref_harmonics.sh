#!/bin/bash

# Write the trefoil harmonics 3 by 3 (since there are 3 significant
# coefficients per 3 harmonics to a pkf curve

i=0
while read line; do
  for i in `seq $i`; do
    echo "0 0 0" >tmp.coeff
  done
  echo $line >>tmp.coeff
  ./coeff2pkf tmp.coeff
  mv bla.pkf harmonics$i.pkf
  i=$(( i+1 ))
done
rm tmp.coeff
