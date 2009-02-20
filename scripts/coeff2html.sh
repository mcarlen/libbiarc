#!/bin/bash

function usage {
  echo "Usage : $0 3/4/5/n N file.coeff out.html"
  exit 0
}

if [ "$#" != "4" ]; then
  usage
fi

function check {
  if ! [ -e $1 ]; then
    echo "$1 not found."
    exit 1
  fi
}

coeff=$3
knotfile=`basename ${3%.coeff}.pkf`
res=$2
style=$1

knot1=`basename $3`
knot=${knot1%.coeff}

# Tools needed
source ~/.bashrc
plot=$LIBBIARC/experimental/pngmanip/plot$D
curv=$LIBBIARC/tools/curvature$D
tors=$LIBBIARC/tools/torsion$D
info=$LIBBIARC/tools/info$D
coeff2pkf=$LIBBIARC/fourierknots/coeff2pkf
fcurv=$LIBBIARC/fourierknots/fcurvature
angles=$LIBBIARC/tools/angle_principal_normal_contacts

check $coeff
check $plot
check $curv
check $tors
check $info
check $coeff2pkf
check $fcurv
check $angles

# How many coefficients?
Ncoeffs=$(cat $coeff | wc -w)
if [ "$style" == "3" ]; then
  Ncoeffs=$(( Ncoeffs - 3))
fi

# Produce pkf file
$coeff2pkf $style $type $res $coeff $knotfile
check $knotfile

# Get knot info
msg=($($info $knotfile | sed '1,5d'))
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
$fcurv $style $res $coeff > $knot.fcurvature
$tors $knotfile >$knot.torsion
$angles $knotfile 0.001 >$knot.angles
maxfcurv=$(cut -d" " -f2 $knot.fcurvature | sort -n | tail -n 1)

echo "N=$N,L=$L"
gnuplot <<EOF
set term png
set output "$knot-torsion-curvature.png"
set title "Curvature/rho/arclengthparam and angle between biarcs for $knot"
set xlabel "arclength"
set ylabel "angle betw biarcs / 180 - rad/(2*D)"
plot [0:$L] [0:1] "$knot.curvature" using 3:4 with steps notitle, "$knot.curvature" using 3:5 with steps notitle, "$knot.curvature" using (\$2/(2*$N)*$L):(\$3/$L) with lines notitle, "$knot.torsion" using 3:(\$5/180) with steps notitle
set output "$knot-rho.png
set title "rho for $knot"
plot [0:$L] [.999:1] "$knot.curvature" using 3:5 with steps notitle
set output "$knot-fcurvature.png"
set title "Fourier curvature for $knot"
set xlabel "t"
set ylabel "D/(2.*fourier''(t))"
plot [0:1] [0:1] "$knot.fcurvature" using 1:($D/2.*\$2) with lines notitle
set output "$knot-torsion.png"
set output "$knot-angles.png"
set title "Angles between principal normal and contact chords"
set xlabel "arclength"
set ylabel "angle (rad)"
plot "$knot.angles" using 1:((\$2*2)-1)*pi/2 notitle
EOF

echo "pp/pt/tt plots"
for i in pp pt tt; do
  $plot $OPEN -plot $i $knotfile >/dev/null
  mv out.png $knot-$i.png
done

# Composite tt and pp plot into one image
convert -size 500x500 xc: -draw 'polygon 499,499 0,499, 0,0' mask.png
composite $knot-tt.png $knot-pp.png mask.png $knot-pp-tt.png

echo "Dump html to $4"
cat <<EOF >$4
<html>
  <h2>$name</h2>
  <b>Coeffs=$Ncoeffs,N=$res<br/>L=$L<br/>D=$D<br/>Rope=$rope<br/></b>
  <TABLE><TR>
    <TD>D/(2*radius_of_curvature) <IMG SRC='$knot-torsion-curvature.png'></TD>
<!--    <TD>D/(2*rad_fourier_curv) <IMG SRC='$knot-fcurvature.png'></TD>
    <TD>Angle between biarcs (~Torsion) <IMG SRC='$knot-torsion.png'></TD> -->
<!--  <TD>2pp/D <IMG SRC='$knot-pp.png'></TD> -->
    <TD>D/2pt <IMG SRC='$knot-pt.png'></TD>
<!--    <TD>D/2tt <IMG SRC='$knot-tt.png'></TD> -->
    <TD>PP (bottom) and TT (top) plot <IMG SRC='$knot-pp-tt.png'></TD>
    <TD>D/(2*rho) <IMG SRC='$knot-rho.png'></TD>
    <TD>Angles <IMG SRC='$knot-angles.png'></TD>
  </TR></TABLE>
</html>
EOF

# Clean-up
# rm -f $knot.curvature $knot.fcurvature $knotfile $knot.torsion $knot-pp.png $knot-tt.png mask.png
