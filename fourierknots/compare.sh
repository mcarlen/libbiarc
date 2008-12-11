#!/bin/bash

if ! [ "$#" == "2" ]; then
  echo "Usage : $0 n/3/4 coeffile"
  exit 0
fi

style=$1
coeffs=$2

#RES=184 #83 184 512
#RES=200
RES=512

$LIBBIARC/scripts/coeff2html.sh $style $RES $coeffs out.html
firefox out.html
exit 0

$LIBBIARC/fourierknots/coeff2pkf $style $RES $coeffs out.pkf && $LIBBIARC/scripts/htmlize.sh out.pkf out.html

#best=16.37197224639253
#rope=$(grep Rope out.html | perl -e '$a=<>; $a=~/Rope=(\d*\.\d*)/; print $1')
#python -c "print $rope, $rope-$best, ($rope-$best)/$best"

#$LIBBIARC/fourierknots/fourier_anneal

Nc=$(cat $coeffs | wc -w)
if [ "$style" == "4" ]; then
  Nc=$(( Nc-3 ))
fi
sedstr="s/N=/Coeffs=$Nc, N=/"

perl -pi -e "s/N=/Coeffs=$Nc, N=/" out.html
firefox out.html
