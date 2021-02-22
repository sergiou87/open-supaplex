#!/bin/bash

source /etc/profile.d/devkit-env.sh
cd 3ds
make -j8 || exit 1

# Bundle and prepare for release
mkdir OpenSupaplex
mkdir 3ds
mkdir cias
mv OpenSupaplex.3dsx 3ds/OpenSupaplex.3dsx
mv OpenSupaplex.cia cias/OpenSupaplex.cia
cp -R ../resources/* OpenSupaplex/
zip -r OpenSupaplex-3ds.zip OpenSupaplex 3ds cias INSTALL.TXT
