#!/bin/zsh

if g++ -std=c++20 -Wall -Wextra $1
then
    echo -e "\e[32m[OK] $1\e[m"
else
    echo -e "\e[31m[NG] $1\e[m"
fi

