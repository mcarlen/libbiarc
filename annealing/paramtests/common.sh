#!/bin/bash

function check {
  if ! [ -e $1 ]; then
    echo "[Err] $1 not found."
    exit 1
  fi
}

function gensub {
echo "qsub file : $3"
if [ "$#" != "3" ]; then
  echo "[Err] gensub : jobname commandstring filename"
  exit 1
fi
cat <<EOF >$3
#!/bin/bash
#$ -S /bin/bash
#$ -cwd
#$ -N $1
#$ -e $3.err
#$ -o $3.out
$2
EOF
}

