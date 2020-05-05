#!/bin/bash

export VITASDK=/usr/local/vitasdk
export PATH=$VITASDK/bin:$PATH # add vitasdk tool to $PATH
cd vita

# Download files from the original game
mkdir -p resources
wget https://cdn.chills.co.za/supaplex.zip
unzip supaplex.zip -d resources/
rm resources/*.exe

# Build
rm -rf build # debug only, remove me
mkdir build
cd build
cmake ..
make -j8
