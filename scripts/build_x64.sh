#!/bin/bash
build_type=Release  # Debug, Release
abi=x64

cd ../
mkdir -p bin_$abi

# rm -rf build_$abi && mkdir build_$abi
cd build_$abi
# make clean

cmake -G "Unix Makefiles" \
    -DCMAKE_CXX_STANDARD=17 \
    -DCMAKE_BUILD_TYPE=$build_type \
    ../

make -j8
cp test ../bin_$abi/

cd ../scripts
echo "----------------------"
bash run.sh
