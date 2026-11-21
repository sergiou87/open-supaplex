#!/bin/bash

source /etc/profile.d/devkit-env.sh
cd switch
make -j8 || exit 1

# Bundle and prepare for release
mkdir -p switch/OpenSupaplex
mv OpenSupaplex.nro switch/OpenSupaplex/OpenSupaplex.nro
cp -R ../resources/* switch/OpenSupaplex/
zip -r OpenSupaplex-switch.zip switch
