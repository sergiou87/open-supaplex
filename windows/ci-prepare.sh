#!/bin/bash

if [ $# -ne 1 ] || ([ $1 != "x86_64" ] && [ $1 != "i686" ]); then
    echo "Missing architecture parameter (x86_64 or i686). Usages:"
    echo "$0 x86_64 # for 64bit build"
    echo "$0 i686 # for 32bit build"
fi

ARCH_SUFFIX=$1
MINGW_PREFFIX="mingw32"

if [ ${ARCH_SUFFIX} == "x86_64" ]; then
    MINGW_PREFFIX="mingw64"
fi

echo "Arch: ${ARCH_SUFFIX}"
echo "Mingw: ${MINGW_PREFFIX}"

pacman --noconfirm -S \
    ${MINGW_PREFFIX}/mingw-w64-${ARCH_SUFFIX}-SDL2 \
    ${MINGW_PREFFIX}/mingw-w64-${ARCH_SUFFIX}-SDL2_mixer \
    msys/zip \
    msys/make \
    ${MINGW_PREFFIX}/mingw-w64-${ARCH_SUFFIX}-gcc \
    ${MINGW_PREFFIX}/mingw-w64-${ARCH_SUFFIX}-pkg-config
