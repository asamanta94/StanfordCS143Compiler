#!/bin/bash

./lexer $1 | ./parser | ./semant | /usr/class/cs143/bin/cgen