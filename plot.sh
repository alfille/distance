#!/bin/sh

# part of distance -- finding average distance in an N-cube
# by Paul H Alfille 2021
# see http://github.com/alfille/distance

# This script uses gnuplot to display the results
# Either piped directly in
# or with the csv file as the only command line argument

if [ $# -eq 0 ]
  then
  # No argument, so make a temp file and copy stdin into it
  datafile=$(mktemp "${TMPDIR:-/tmp/}$(basename $0).XXXXXXXXXXXX")
  cat > $datafile
  else
  # Use the command line argument as the CSV filename
  datafile="$1"
fi

# Actual gnuplot command
gnuplot -p -e "\
    set datafile separator ',';\
    set title 'Random segments in a unit N-cube';\
    set xlabel 'dimension';\
    set ylabel 'average distance';\
    plot for [p=2:*] '${datafile}' using 1:p with lines title columnhead(p).'-norm';\
    "
