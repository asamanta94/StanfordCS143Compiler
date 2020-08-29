#!/bin/bash

./lexer $1 | ./parser | ./semant | ./cgen # > test.s
