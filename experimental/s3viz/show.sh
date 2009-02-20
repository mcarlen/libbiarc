#!/bin/bash

if ! [ "$#" == 1 ]; then
  echo "Usage : $0 file4.pkf"
  exit 0
fi

./s3viz $1

files=""
if [ -e up.pkf ]; then
  files+=up.pkf" "
fi
if [ -e down.pkf ]; then
  files+=down.pkf
fi

if [[ $files == "" ]]; then
  echo "No files found."
  exit 1
fi

$LIBBIARC/inventor/main -N=200 -iv-scene spheres.iv $files
# rm -f up.pkf down.pkf
