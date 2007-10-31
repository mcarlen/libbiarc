#!/bin/bash

./curvature $1 > tmp.tmp
gnuplot <<EOF
set terminal png
set output "out.png"
set xrange [0:1]
set yrange [0:1]
set xlabel "Curvature"
set ylabel "Thickness/2/radius_curvature"
plot "tmp.tmp" using 3:4 notitle with steps
EOF
