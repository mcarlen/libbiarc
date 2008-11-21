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
AnnealFlag=1
LogFreq=1
Seed=0

knot=$1
log_dir=$2
prog=`basename $anneal`

check $knotinfo
check $anneal
check $knot

if ! [ -d $log_dir ]; then
  echo "Create log dir : $log_dir"
  mkdir -p $log_dir/sub
fi

i=0
Tlist="0.001 0.01 0.1"
Clist="1e-8 1e-7 1e-6 1e-5"
for T in $Tlist; do
  for C in $Clist; do
    li=`printf "%04d" $i`
    echo "Process T=$T C=$C"
    gensub anneal$li "$anneal $knot ${log_dir}/log$li $T $C $AnnealFlag $LogFreq 1e-12 $Seed" ${log_dir}/sub/anneal$li.sub
    qsub ${log_dir}/sub/anneal$li.sub
    i=$(( i+1 ))
  done
done
