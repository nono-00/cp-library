#!/bin/zsh

test_directory="../test"
compile_check="./.compile_check.sh"

find $test_directory | grep .cpp | xargs -n 1 -I{} -P 8 zsh -c "$compile_check {}"
rm -f a.out
