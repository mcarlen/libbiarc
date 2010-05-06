#!/bin/bash

function usage {
  echo "Usage : $0 [open] file.pkf out.html"
  exit 0
}

Dim=""
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
  Dim=4
else
  echo "We're in 3D!"
fi

knotfile=$1
knot1=`basename $1`
knot=${knot1%.pkf}

# Tools needed
source ~/.bashrc
plot=$LIBBIARC/experimental/pngmanip/plot$Dim
curv=$LIBBIARC/tools/curvature$Dim
if [ "$Dim" != "4" ]; then
  tors=$LIBBIARC/tools/torsion
  check $tors
fi
info=$LIBBIARC/tools/info$Dim
cset=$LIBBIARC/tools/contactset$Dim

check $plot
check $curv
check $info

# Get knot info
msg=($($info $OPEN $1 | sed '1,5d'))
name=$knot
if [ "$Dim" == "4" ]; then
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
$curv $knotfile >$knot.curvature 2>/dev/null
gnuplot <<EOF
set term png
set output "$knot-curvature.png"
set title "Curvature/rho for $knot"
plot [0:$L] [0:1.01] "$knot.curvature" using 3:4 with steps notitle, "$knot.curvature" using 3:5 with steps notitle
set output "$knot-rho.png
set title "rho for $knot"
plot [0:$L] [.999:1.0001] "$knot.curvature" using 3:5 with steps notitle
EOF

# generate torsion plots
if [ "$Dim" != "4" ]; then
echo "Generate torsion plots"
$tors $knotfile >$knot.torsion 2>/dev/null
gnuplot <<EOF
set term png
set output "$knot-torsion.png"
set title "Torsion (angle between biarcs) for $knot"
plot [0:$L] "$knot.torsion" using 3:5 with steps notitle
EOF
fi

echo "pp/pt/tt plots"
for i in pp pt tt; do
  $plot $OPEN -plot $i $1 >/dev/null
  mv out.png $knot-$i.png
done

# generate contact set
echo "Generate contact set"
dummy=""
if [ "$Dim" == "4" ]; then
  dummy="0"
fi
# $cset $OPEN $knotfile 0.01 5 $dummy >$knot.ssigma 2>/dev/null
$cset $OPEN $knotfile 0.3 5 $dummy >$knot.ssigma 2>/dev/null
gnuplot <<EOF
set term png
set output "$knot-cset.png"
set title "Contact set for $knot"
set size square
plot [0:1] [0:1] "$knot.ssigma" notitle
EOF

echo "Render 3D pt"
cp $knot-pt.png /tmp/tex.png
$plot $OPEN -hm -plot pt $1 >/dev/null
python $LIBBIARC/experimental/pngmanip/interpdiag.py out.png /tmp/hm.png
blender -b $LIBBIARC/experimental/blender/plot.blend -f 1 -F PNG -o /tmp/ >/dev/null
mv /tmp/0001.png ./$knot-3dpt.png

if [ "$Dim" == "" ]; then
RENDER_CLOSED=-closed
if [ "$OPEN" == "-open" ]; then 
  RENDER_CLOSED=""
fi
echo "Render knot pix"
$LIBBIARC/tools/inertiatensor $knotfile | sed -n 8,10p >$knot.inertia
~/work/pkfrender/pkfrender -whitebg $RENDER_CLOSED -N=600 -R=`python -c "print $D/2."` -whitebg -quality=1 -S=32 -matrix `cat $knot.inertia | sed -n 1p` $knotfile >/dev/null
convert output.tif -resize 400x -fuzz 7% -trim -gravity center -extent 110%x $knot-1.png
~/work/pkfrender/pkfrender -whitebg $RENDER_CLOSED -N=600 -R=`python -c "print $D/2."` -whitebg -quality=1 -S=32 -matrix `cat $knot.inertia | sed -n 2p` $knotfile >/dev/null
convert output.tif -resize 400x -fuzz 7% -trim -gravity center -extent 120%x $knot-2.png
~/work/pkfrender/pkfrender -whitebg $RENDER_CLOSED -N=600 -R=`python -c "print $D/2."` -whitebg -quality=1 -S=32 -matrix `cat $knot.inertia | sed -n 3p` $knotfile >/dev/null
convert output.tif -resize 400x -fuzz 7% -trim -gravity center -extent 120%x $knot-3.png
fi

echo "Dump html to $2"
cat <<EOF >$2
<html>
  <h2>$name (<a href='$knotfile'>PKF file</a>)</h2>
  <TABLE>
  <TR>
    <TD><b>N=$N<br/>L=$L<br/>D=$D<br/>Rope=$rope<br/></b><br/><TD>
EOF
if [ "$Dim" == "" ]; then
cat <<EOF >>$2
    <TD><img src='$knot-1.png' /></TD>
    <TD><img src='$knot-2.png' /></TD>
    <TD><img src='$knot-3.png' /></TD>
EOF
fi
cat <<EOF >>$2
  </TR>
  </TABLE>
  <TABLE>
  <TR>
    <TD>D/(2*radius_of_curvature)</TD>
    <TD>Torsion (~angle between biarcs)</TD>
    <TD>D/(2*rho)</TD>
  </TR>
  <TR>
    <TD><IMG SRC='$knot-curvature.png'></TD>
    <TD><IMG SRC='$knot-torsion.png'></TD>
    <TD><IMG SRC='$knot-rho.png'></TD>
  </TR>
  </TABLE>
  <TABLE>
    <TR><TD>Contact set</TD><TD>3D pt plot</TD></TR>
    <TR><TD><IMG SRC='$knot-cset.png'></TD>
        <TD><IMG SRC='$knot-3dpt.png'></TD></TR>
  </TABLE>
  <TABLE>
  <TR>
    <TD>2pp/D</TD>
    <TD>D/2pt</TD>
    <TD>D/2tt</TD>
  </TR>
  <TR>
    <TD><IMG SRC='$knot-pp.png'></TD>
    <TD><IMG SRC='$knot-pt.png'></TD>
    <TD><IMG SRC='$knot-tt.png'></TD>
  </TR>
  </TABLE>
</html>
EOF

# Clean-up
rm -f $knot.curvature $knot.ssigma $knot.inertia out.png output.tif
