#!/bin/bash

source /etc/profile.d/devkit-env.sh
cd wii
make -j8 || exit 1

# Bundle and prepare for release
mkdir -p apps/OpenSupaplex
mv OpenSupaplex.dol apps/OpenSupaplex/BOOT.DOL
cp -R ../resources/* apps/OpenSupaplex/
cp assets/* apps/OpenSupaplex/
zip -r OpenSupaplex-wii.zip apps
