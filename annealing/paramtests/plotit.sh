#!/bin/bash

if [ "$#" != "2" ]; then
  echo Usage : $0 directory outfile
  exit 0
fi

direc=$1
outfile=$2

if ! [ -d $direc ]; then
  echo "[Err] $direc not found."
  exit 1
fi

string=""
for i in $direc/log0*; do
  string+=" \"$i\" with steps notitle,"
done

# gnuplot
gnuplot <<EOF
set term png
set output "$outfile"
plot $string 0
EOF

