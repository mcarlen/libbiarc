#!/bin/bash

function usage {
  echo "Usage : $0 [open] file.pkf out.html"
  exit 0
}

D=""
OPEN=""
if [ "$#" == "3" ]; then
  if [ "$1" != "open" ]; then
    usage
  fi 
  echo "Consider an OPEN curve!"
  OPEN=-open
  shift
fi

if [ "$#" != "2" ]; then
  usage
fi

function check {
  if ! [ -e $1 ]; then
    echo "$1 not found."
    exit 1
  fi
}

# Do we have 3 or 4 dimensions?
check $1
arr=( $(grep "NODE" $1 | sed -n '1p') )
if [ "${#arr[*]}" == "9" ]; then
  echo "We're in 4D!"
  D=4
else
  echo "We're in 3D!"
fi

knotfile=$1
knot1=`basename $1`
knot=${knot1%.pkf}

# Tools needed
source ~/.bashrc
plot=$LIBBIARC/experimental/pngmanip/plot$D
curv=$LIBBIARC/tools/curvature$D
info=$LIBBIARC/tools/info$D

check $plot
check $curv
check $info

# Get knot info
msg=($($info $1 | sed '1,5d'))
name=$knot
if [ "$D" == "4" ]; then
  N=${msg[5]}
  L=${msg[9]}
  D=${msg[21]}
  rope=${msg[25]}
else
  N=${msg[5]}
  L=${msg[9]}
  D=${msg[20]}
  rope=${msg[24]}
fi

# generate curvature plots
echo "Generate curvature plots"
$curv $knotfile >$knot.curvature
gnuplot <<EOF
set term png
set output "$knot-curvature.png"
set title "Curvature/rho for $knot"
plot [0:$L] [0:1] "$knot.curvature" using 3:4 with steps notitle, "$knot.curvature" using 3:5 with steps notitle
set output "$knot-rho.png
set title "rho for $knot"
plot [0:$L] [.999:1] "$knot.curvature" using 3:5 with steps notitle
EOF

echo "pp/pt/tt plots"
for i in pp pt tt; do
  $plot $OPEN -plot $i $1 >/dev/null
  mv out.png $knot-$i.png
done

echo "Dump html to $2"
cat <<EOF >$2
<html>
  <h2>$name</h2>
  <b>N=$N<br/>L=$L<br/>D=$D<br/>Rope=$rope<br/></b>
  <TABLE><TR>
    <TD>D/(2*radius_of_curvature) <IMG SRC='$knot-curvature.png'></TD>
    <TD>2pp/D <IMG SRC='$knot-pp.png'></TD>
    <TD>D/2pt <IMG SRC='$knot-pt.png'></TD>
    <TD>D/2tt <IMG SRC='$knot-tt.png'></TD>
    <TD>D/(2*rho) <IMG SRC='$knot-rho.png'></TD>
  </TR></TABLE>
</html>
EOF

# Clean-up
rm -f $knot.curvature
