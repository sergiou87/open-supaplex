#!/bin/bash

source /etc/profile.d/devkit-env.sh
cd wiiu
make -j8 || exit 1

# Bundle and prepare for release
mkdir -p wiiu/apps/OpenSupaplex
mv OpenSupaplex.rpx wiiu/apps/OpenSupaplex/OpenSupaplex.rpx
cp -R ../resources/* wiiu/apps/OpenSupaplex/
cp assets/* wiiu/apps/OpenSupaplex/
zip -r OpenSupaplex-wiiu.zip wiiu
