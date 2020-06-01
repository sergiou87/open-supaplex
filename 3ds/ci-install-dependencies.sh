#!/bin/bash

sudo apt-get update && sudo apt-get -y install zip
sudo dkp-pacman -Sy && sudo dkp-pacman -S dkp-libs/3ds-sdl --noconfirm # not needed once the devkitpro/devkitarm image is updated
