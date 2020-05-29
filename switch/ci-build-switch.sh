#!/bin/bash

source /etc/profile.d/devkit-env.sh
cd switch
make -j8

# Bundle and prepare for release
mkdir -p switch/OpenSupaplex
mv OpenSupaplex.nro switch/OpenSupaplex/OpenSupaplex.nro
cp -R ../resources/* switch/OpenSupaplex/
rm -rf switch/OpenSupaplex/audio-lq # we don't need low quality audio here
zip -r OpenSupaplex-switch.zip switch
