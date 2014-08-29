echo source setenv.sh
SCRIPTDIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
echo SCRIPTDIR="$SCRIPTDIR"

export LIBBIARC="$SCRIPTDIR"
export LD_LIBRARY_PATH="$SCRIPTDIR/lib/:$LD_LIBRARY_PATH"
export PATH="$SCRIPTDIR/bin/:$PATH"
