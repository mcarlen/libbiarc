#!/bin/bash

# Where are the matlab scripts?
source config

if ! [ "$#" = "1" ]; then
  echo "Usage : $0 pkffile"
  exit
fi

PKF=$1

if ! [ "$MDIR" = "" ]; then

pushd . >/dev/null
cp $PKF /tmp
cd /tmp
/usr/bin/perl $MDIR/arccurve/PKF2matlab.pl $PKF xxx >/dev/null 2>&1
echo "addpath('$MDIR/arccurve/pkf2bezier');" >> xxx.m
echo "addpath('$MDIR/arccurve/v3');" >> xxx.m
echo -n "Jana  "
ddd=$(matlab -nojvm -nosplash -nodesktop << EOF
format long;
xxx;
[B0,B1,B2]=make_biarccurve_tan4(x,t,1);
tic
[length,total_length,length_added,min_r,r,sizecritC,sizedistC,critC,distC,lower_bound_thickness,upper_bound_thickness,upper_bound_ropelength,lower_bound_ropelength,max_error]=ropelength_v3(B0,B1,B2,1);
duration=toc;
[N one]=size(lower_bound_thickness);
fid=fopen('xxx2','w');
fprintf(fid,'%24.24f ',(lower_bound_thickness(N)));
fprintf(fid,'%6.6f\n',duration);
fclose(fid);
EOF)
cat xxx2
rm -f xxx.m xxx2 `basename $PKF`
popd >/dev/null

fi

./thick $PKF
