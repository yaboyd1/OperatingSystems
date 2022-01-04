#!/bin/bash

# Compare how long the program takes as the number of threads increases
# bash increaseThreads.sh
gcc -pthread -o divisors divisors.c
./divisors 1
./divisors 2
./divisors 4
./divisors 8
./divisors 16
./divisors 32
./divisors 64
