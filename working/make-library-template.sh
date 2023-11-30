#!/bin/zsh

if [ $# -ne 1 ]
then
    echo "Error: Please enter the name of the library you want to create."
    exit 1
fi

template_directory=".template"
library_name=$1

mkdir $library_name
cp $template_directory/library.hpp $library_name/$library_name.hpp
cp $template_directory/library-test.cpp $library_name/$library_name-test.cpp

echo "Make $library_name.hpp and $library_name-test.cpp"
