for i in `seq 0 9`; do
for j in `seq 0 9`; do
  echo $i, $j
  ./trefoil 1.5$i `python -c "from math import pi; print 3.2${j}*pi/3."` 160 && curvature trefoil.pkf > /tmp/xxx
  cp trefoil.pkf trefoil_${i}_${j}.pkf
  gnuplot << EOF
set terminal png
set output "graph_${i}_${j}.png"
set xrange [0:1]
set yrange [0:1.01]
plot "/tmp/xxx" using 3:4 with steps notitle, "/tmp/xxx" using 3:5 with steps notitle
EOF
done
done
