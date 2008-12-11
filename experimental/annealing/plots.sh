for f in bla*.bla; do
  png="$(basename $f .bla).png"
  echo "Preparing $png"
gnuplot <<EOF
  set size square
  set output "$png"
  set term png
  plot [-5:5] [-5:5] "$f"
EOF

done
