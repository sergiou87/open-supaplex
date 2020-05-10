#!/bin/bash

wget https://github.com/devkitPro/pacman/releases/latest/download/devkitpro-pacman.deb
sudo dpkg -i devkitpro-pacman.deb
sudo dkp-pacman -S switch-dev switch-sdl2 switch-sdl2_mixer switch-pkg-config --noconfirm
