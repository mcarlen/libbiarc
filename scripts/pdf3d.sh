#!/bin/bash

TEST=true

# U3D needs this :
# U3D_SOURCE=PATH_TO/U3D/Source/
# U3D_LIBDIR=${U3D_SOURCE}Build/U3D/
# PATH=$PATH:$U3D_LIBDIR
# LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$U3D_LIBDIR

if [ "$#" != "2" ]; then
  echo "Usage : $0 file.pkf out.pdf"
  exit 0
fi

function check_return {
  if [ $1 -ne 0 ]; then
    echo "Exit $1"
    exit $1
  fi
}

function check {
  if ! [ -e $1 ]; then
    echo "$1 not found."
    exit 1
  fi
}

pkf2idtf=
idtf2u3d=/home/carlen/coding/pdf3d/U3D/Source/Build/U3D/IDTFConverter.out
infile=$1
job=${2%.pdf}

# check pkf2idtf
check $idtf2u3d
check $infile

if $TEST; then
  idtf=trefoil.idtf
else
  idtf=$(mktmep)
  echo "pkf -> idtf"
  $pkf2idtf $infile $idtf
  check_return $?
fi

# u3d=$(mktmep)
u3d=bla.u3d
echo "idtf -> u3d"
echo $idtf2u3d -en 1 -rzf 0 -pq 500 -input "$idtf" -output "$u3d" # >/dev/null 2>&1
$idtf2u3d -en 1 -rzf 0 -pq 500 -input "$idtf" -output "$u3d" # >/dev/null 2>&1
check_return $?

# latex file
echo "latex passes and write to $job.pdf"
FINISHED=/bin/false
check="Not finished"
while [ "$check" ]; do
  check=$(cat <<EOF | pdflatex -jobname $job | grep "@@ Rerun to get object references right! @@"
\documentclass[a4paper]{article}
\usepackage[3D]{movie15}
\usepackage[colorlinks=true]{hyperref} 
\begin{document}
\title{Embedding 3D stuff in a PDF}
\author{Mathias Carlen - LCVMM - EPFL \\ \href{http://lcvmwww.epfl.ch}{http://lcvmwww.epfl.ch}}
\date{\today}
\maketitle
\begin{center}
\includemovie[
	poster,
	toolbar,
	label=$u3d,
	text=($u3d),
%	3Djscript=turntable.js,
	3Daac=60, 3Droll=0, 3Dc2c=0 1 0, 3Droo=3, 3Dcoo=0 0 0,
	3Dlights=Cube,
]{\linewidth}{\linewidth}{$u3d}
\label{xyz} Bla \end{center}
\end{document}
EOF)

done

# rm -f $idtf $u3d $job.aux $job.log
rm -f $u3d $job.aux $job.log $job.out
