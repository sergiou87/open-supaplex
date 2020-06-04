#!/bin/bash

cd psp
make -j8 || exit 1

# Bundle and prepare for release
mkdir OpenSupaplex
mv EBOOT.PBP OpenSupaplex/EBOOT.PBP
cp -R ../resources/* OpenSupaplex/
rm -rf OpenSupaplex/audio-hq # we don't need high quality audio here
zip -r OpenSupaplex-psp.zip OpenSupaplex
