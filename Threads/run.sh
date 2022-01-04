#!/bin/bash

# Use this bash script to compile a program that includes the posix thread library
# bash run.sh
gcc -pthread -o divisors divisors.c
./divisors
