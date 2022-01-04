#!/bin/bash

# Compare how long the program takes as the number increases
# bash increaseThreads.sh
gcc -pthread -o divisors divisors.c
./divisors 4 10
./divisors 4 100
./divisors 4 1000
./divisors 4 10000
./divisors 4 100000
./divisors 4 1000000
