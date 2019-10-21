#!/bin/bash

if [ "$1" != "quick" ]; then
../setup.sh $1 $2
fi
gcc test.c $lib -lud_string_array -lud_file -lud_protodeep_core -o executable
./executable
