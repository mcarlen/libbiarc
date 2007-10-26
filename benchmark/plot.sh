#!/bin/bash

# for i in 100 200 500 1000 2000 3000 5000 10000 20000 50000 80000 100000 140000 200000 250000 500000 600000 700000 800000 900000 1000000; do
rm -f bench[123].txt bla.txt
for i in `seq 100 500 100000`; do
  echo $i >> bench1.txt
  ./bench $i 500 >> bench2.txt
  ./benchben $i 500 >> bench3.txt
done

paste bench[123].txt > bla.txt

gnuplot <<EOF
set terminal png
set output "out.png"
set ylabel "time (s)"
set xlabel "No access"
plot "bla.txt" using 1:2 title "libbiarc" with lines , "bla.txt" using 1:3 title "knotlib" with lines
EOF

rm -f bench[123].txt bla.txt
