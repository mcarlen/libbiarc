echo source setenv.sh
script=$(readlink -f "$0")
currdir=$(dirname "$script")

export LIBBIARC="$currdir"
export LD_LIBRARY_PATH="$currdir/lib/:$LD_LIBRARY_PATH"
export PATH="$currdir/bin/:$PATH"
