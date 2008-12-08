#!/bin/bash

# Write the trefoil harmonics 3 by 3 (since there are 3 significant
# coefficients per 3 harmonics to a pkf curve

if ! [ "$#" == "1" ]; then
  echo "Usage : $0 coeff_file"
  exit 0
fi

i=0
while read line; do
  echo $line
#  echo -n >tmp.coeff
  echo "0 0 0" >tmp.coeff
  for i in `seq $i`; do
    echo "0 0 0 0 0 0" >>tmp.coeff
  done
  echo $line >>tmp.coeff
  echo $i
  cat tmp.coeff
#  ./coeff2pkf 3 200 tmp.coeff harmonics`printf "%04d" $i`.pkf
  ./coeff2pkf 4 200 tmp.coeff harmonics`printf "%04d" $i`.pkf
  if ! [[ $? ]]; then
    echo "[Err] Stopped."
    exit 1
  fi
  i=$(( i+1 ))
done <$1
rm tmp.coeff
