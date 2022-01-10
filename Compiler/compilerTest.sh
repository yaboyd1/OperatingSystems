#!/bin/bash

# Testing our simple compiler
gcc compiler.c -o compiler
./compiler helloWorld.c goodbyeWorld.c
rm compiler helloWorld goodbyeWorld