#!/bin/bash

# compile the project
make clean
make

if [[ $1 -eq 0 ]]; then
	ln -s Test/*.bin .
	time ./lychrel
elif [[$1 -eq 1]]; then
	rm *.bin
	ln -s Final/*.bin
	time lychrel > RESULTS
	diff -s RESULTS Final/Output.txt
else
	echo needs to be 0 or 1
fi

