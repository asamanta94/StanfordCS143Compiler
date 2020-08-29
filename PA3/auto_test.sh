#!/bin/bash

rm -rf good_result test_good_result bad_result test_bad_result

mkdir good_result test_good_result bad_result test_bad_result

echo "Diffing good cases."

FILES=./tests/good/*

for f in $FILES; do
	echo $(basename $f)
	./semant.sh $f > test_good_result/$(basename $f).txt
	./sample_semant.sh $f > good_result/$(basename $f).txt
	diff good_result/$(basename $f).txt test_good_result/$(basename $f).txt
done

echo "Diffing bad cases."

FILES2=./tests/bad/*

for f in $FILES2; do
	echo $(basename $f)
	./semant.sh $f > test_bad_result/$(basename $f).txt
	./sample_semant.sh $f > bad_result/$(basename $f).txt
	diff bad_result/$(basename $f).txt test_bad_result/$(basename $f).txt
done