#!/bin/bash

# Create 4 processes and test how the CPU handles them
echo "Executing ./cpu A & ./cpu B & ./cpu C & ./cpu D"
gcc cpu.c -o cpu
./cpu A & ./cpu B & ./cpu C & ./cpu D
