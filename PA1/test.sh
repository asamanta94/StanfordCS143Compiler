#!/bin/bash

make lexer
rm -rf test.txt
rm -rf test2.txt
./lexer $1 > test.txt
/afs/ir.stanford.edu/class/cs143/bin/lexer $1 > test2.txt
OUT=$(diff test.txt test2.txt)
if [[ -z $OUT ]]; then
	echo "Test Passed"
else
	echo "Test Failed"
fi
