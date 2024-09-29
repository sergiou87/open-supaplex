#!/bin/bash

export VITASDK=/usr/local/vitasdk
export PATH=$VITASDK/bin:$PATH # add vitasdk tool to $PATH
cd vita

# Build
rm -rf build # debug only, remove me
mkdir build
cd build
cmake ..
make -j8
