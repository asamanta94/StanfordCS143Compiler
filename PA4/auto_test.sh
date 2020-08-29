#!/bin/bash

rm -rf result test_result

mkdir result test_result

FILES=./tests/*

for f in $FILES; do
	echo $(basename $f)
	./cgen.sh $f > test_result/$(basename $f).txt
	./sample_cgen.sh $f > result/$(basename $f).txt
	diff result/$(basename $f).txt test_result/$(basename $f).txt
done