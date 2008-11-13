#!/bin/bash

if [ "$#" != "9" ]; then
  echo "Usage : $0 knot T0 Ts T1 C0 Cs C1 time_in_s outfile.png"
  echo "example : k3.1.pkf 0.01 .005 0.02 0.001 .0005 0.002 10 out.png"
  exit 0
fi

knotinfo=$LIBBIARC/tools/info
# this modified annealing prog prints elapsed time and thickness/energy
# on std out
anneal=$LIBBIARC/annealing/paramtests/anneal_param
plotit=`pwd`/plotit.sh
postproc=`pwd`/postproc.sh
AnnealFlag=1
LogFreq=1
Seed=0
log_dir=/tmp/log_dir

knot=$1
T0=$2
Ts=$3
T1=$4
C0=$5
Cs=$6
C1=$7
tf=$8
outfile=$9
prog=`basename $anneal`

function check {
  if ! [ -e $1 ]; then
    echo "[Err] $1 not found."
    exit 1
  fi
}

check $knotinfo
check $anneal
check $plotit
check $postproc
check $knot

if ! [ -d $log_dir ]; then
  echo "Create log dir : $log_dir"
  mkdir -p $log_dir
fi

i=0
for T in `seq $T0 $Ts $T1`; do
  for C in `seq $C0 $Cs $C1`; do
    echo "Process T=$T C=$C"
    $anneal $knot $log_dir/ $T $C $AnnealFlag $LogFreq 1e-12 $Seed >/dev/null 2>&1 &
    sleep $tf
    if [[ `pidof $prog` ]]; then
      $(kill -HUP `pidof $prog` >/dev/null 2>&1)
    fi
    mv $log_dir/log $log_dir/log`printf "%04d" $i`
    i=$(( i+1 ))
  done
done

$plotit $log_dir $outfile

rm -rf $log_dir
