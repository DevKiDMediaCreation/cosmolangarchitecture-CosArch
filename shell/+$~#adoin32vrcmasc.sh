#!/bin/bash

mkdir -p build
cmake -S . -B build/
cmake --build build/
./build/cosarch ./test.cos
# echo $?

echo Running Program
../data/out
echo $?