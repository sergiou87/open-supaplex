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

make -j8 || exit

mv OpenSupaplex.exe OpenSupaplex-tmp.exe
cp -R ../resources ./OpenSupaplex
cp OpenSupaplex-tmp.exe ./OpenSupaplex/OpenSupaplex.exe

ldd OpenSupaplex-tmp.exe

for F in `ldd OpenSupaplex-tmp.exe | grep ${MINGW_PREFFIX} | cut -d' ' -f3`
do
    cp $F ./OpenSupaplex/
done

zip -r OpenSupaplex-windows-${ARCH_SUFFIX}.zip ./OpenSupaplex
