#!/bin/bash

if [ "$1" == "C" ]; then
    ./fptree $2 f_itemset.dat
    ./comp $2 f_itemset.dat 2 $3
elif [ "$1" == "D" ]; then
    ./decomp $2 $3
else
    echo "Invalid Argument"
fi