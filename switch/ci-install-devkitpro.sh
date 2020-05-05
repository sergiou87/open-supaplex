#!/bin/bash

wget https://github.com/devkitPro/pacman/releases/latest/download/devkitpro-pacman.deb
sudo dpkg -i devkitpro-pacman.deb
sudo dkp-pacman -S switch-dev switch-sdl2 --noconfirm
mkdir -p switch/OpenSupaplex
mv OpenSupaplex.nro switch/OpenSupaplex/OpenSupaplex.nro
wget https://cdn.chills.co.za/supaplex.zip
unzip supaplex.zip -d switch/OpenSupaplex/
rm switch/OpenSupaplex/*.exe
zip -r OpenSupaplex-switch.zip switch
