#!/bin/bash

source /etc/profile.d/devkit-env.sh
cd 3ds
make -j8 NEW3DS=1 || exit 1

# Bundle and prepare for release
mkdir OpenSupaplex
mkdir 3ds
mkdir cias
mv OpenSupaplex.3dsx 3ds/OpenSupaplex.3dsx
mv OpenSupaplex.cia cias/OpenSupaplex.cia
cp -R ../resources/* OpenSupaplex/
rm -rf OpenSupaplex/audio-hq # we don't need high quality audio here
rm -rf OpenSupaplex/audio-lq # we don't need low quality audio here
zip -r OpenSupaplex-new-3ds.zip OpenSupaplex 3ds cias INSTALL.TXT
