#!/bin/bash

# 引数の数をチェック
if [ $# -ne 1 ]; then
    echo "Usage: $0 <input_file>"
    exit 1
fi

CUR_DIR=$(pwd)
BASE_DIR=$(cd $(dirname $0); pwd)
cd ${CUR_DIR}

input_file=${CUR_DIR}/$1

if [ ! -f $input_file ]; then
    echo "Error: File '$input_file' does not exist."
    exit 1
fi

python ${BASE_DIR}/expand.py ${input_file} | grep -v 'assert\|pragma once\|//' | tr -s '\n' '\n' > ${CUR_DIR}/".expand"

echo "assert, pragma once, comment was removed from '$input_file'."

