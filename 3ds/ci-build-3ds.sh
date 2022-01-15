#!/bin/bash

source /etc/profile.d/devkit-env.sh
cd 3ds
make -j8 || exit 1

# Bundle and prepare for release
mkdir 3ds
mkdir 3ds
mkdir cia
mv OpenSupaplex.3dsx 3ds/OpenSupaplex.3dsx
mv OpenSupaplex.cia cia/OpenSupaplex.cia
zip -r OpenSupaplex-3ds.zip 3ds cia INSTALL.TXT
