#!/bin/sh
if [ $# -eq 0 ]
  then
  datafile=$(mktemp "${TMPDIR:-/tmp/}$(basename $0).XXXXXXXXXXXX")
  cat > $datafile
  else
  datafile="$1"
fi
#echo $datafile
#cat $datafile
cat "${1:-/dev/stdin}" | gnuplot -p -e "set datafile separator ','; set key autotitle columnhead ; plot for [c=2:*] '${datafile}' using 1:c with lines"
