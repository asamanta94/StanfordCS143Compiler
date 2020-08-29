#!/bin/bash

test_files=("single_feature_test.cl" "multiple_feature_test.cl" "sample_while_loop.cl" "cond_test.cl" "good.cl")

test() {
	parser_out=$(/afs/ir.stanford.edu/class/cs143/bin/lexer $1 | ./parser 2>/dev/null)
	sample_parser_out=$(/afs/ir.stanford.edu/class/cs143/bin/lexer $1 | /afs/ir.stanford.edu/class/cs143/bin/parser 2>/dev/null)

	if [[ $parser_out == $sample_parser_out ]]; then
        	echo -e "\e[32mTest $1 Passed"
    	else
        	echo -e "\e[31mTest $1 Failed"
    	fi
}

test_run() {
	for cl_file in ${test_files[@]}; do
		test $cl_file
    	done
}

test_run
