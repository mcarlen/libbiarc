#!/bin/bash

if [ "$#" != "2" ]; then
  echo "Usage : $0 knot log_dir"
  exit 0
fi

source common.sh

knotinfo=$LIBBIARC/tools/info
# this modified annealing prog prints elapsed time and thickness/energy
# on std out
anneal=$LIBBIARC/annealing/paramtests/anneal_param
plotit=`pwd`/plotit.sh
AnnealFlag=1
LogFreq=1
Seed=0

knot=$1
log_dir=$2
prog=`basename $anneal`

check $knotinfo
check $anneal
check $plotit
check $postproc
check $knot

if ! [ -d $log_dir ]; then
  echo "Create log dir : $log_dir"
  mkdir -p $log_dir/sub
fi

i=0
# Tlist="0.01 0.1 1"
# Clist="1e-6 1e-5 1e-4 1e-3"
Tlist="1 2"
Clist="3 4"
for T in $Tlist; do
  for C in $Clist; do
    li=`printf "%04d" $i`
    echo "Process T=$T C=$C"
    gensub anneal$li "$anneal $knot ${log_dir}/log$li $T $C $AnnealFlag $LogFreq 1e-12 $Seed >/dev/null 2>&1" ${log_dir}/sub/anneal$li.sub
    echo qsub ${log_dir}/sub/anneal$li.sub
    i=$(( i+1 ))
  done
done
