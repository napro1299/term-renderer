#!/bin/bash

CC=
out=
run=false

print_usage() {
  echo "Usage: [-r] [-c CC]"
}

while getopts 'rc:o:' flag; do
  case $flag in
    r) 
      run=true
      ;;
    c)
      if [[ $OPTARG == clang || $OPTARG == gcc ]]; then
        CC=$OPTARG
      else
        echo "Invalid argument '${OPTARG}'"
        exit 1
      fi
      ;;
    o)
      out=$OPTARG
      ;;
    *) 
      print_usage
      exit 1
      ;;    
    esac
done

$CC tr-example.c -o $out -lm

if [ $run == true ]; then
  printf '=%.0s' $(seq 1 "$(tput cols)")
  echo
  ./$out
fi