#!/bin/bash
cd ../bin_x64

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:./:../3rd
# gdb 
./test 55 183 154
