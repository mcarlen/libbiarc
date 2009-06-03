#!/bin/bash

# Time vs. CPUs
gnuplot<<EOF
set term postscript
set output "time_vs_cpus.ps"
set xlabel "#CPUs"
set ylabel "Time (s)"
set size square
plot "time_vs_cpus.txt" notitle
EOF
ps2pdf time_vs_cpus.ps

# Time vs. nodes
gnuplot<<EOF
set term postscript
set output "time_vs_nodes.ps"
set xlabel "number of knot data points"
set ylabel "Time (s)"
set size square
plot "time_vs_nodes.txt" title "Parallel", "time_vs_nodes_single.txt" title "Single"
EOF
ps2pdf time_vs_nodes.ps

# Time vs. knot (k.X.1)
gnuplot<<EOF
set term postscript
set output "time_vs_knots.ps"
set xlabel "number of crossings of first knot in class"
set ylabel "Time (s)"
set size square
set xrange [-1:10]
plot "time_vs_knots.txt" using 1:3 title "Parallel", "time_vs_knots.txt" using 1:2 title "Single"
EOF
ps2pdf time_vs_knots.ps

# Time vs. batch size
gnuplot<<EOF
set term postscript
set output "time_vs_batch.ps"
set xlabel "Batch size"
set ylabel "Time (s)"
set size square
plot "time_vs_batch.txt"
EOF
ps2pdf time_vs_batch.ps

