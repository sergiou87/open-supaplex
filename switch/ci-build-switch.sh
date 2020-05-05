#!/bin/bash

source /etc/profile.d/devkit-env.sh
cd switch
make -j8

# Bundle and prepare for release
mkdir -p switch/OpenSupaplex
mv OpenSupaplex.nro switch/OpenSupaplex/OpenSupaplex.nro
wget https://cdn.chills.co.za/supaplex.zip
unzip supaplex.zip -d switch/OpenSupaplex/
rm switch/OpenSupaplex/*.exe
zip -r OpenSupaplex-switch.zip switch
