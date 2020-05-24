#!/bin/bash

make -j8 || exit

TARGET_DIR=OpenSupaplex-windows-x64

cp -R ../resources ./${TARGET_DIR}
cp OpenSupaplex ./${TARGET_DIR}/OpenSupaplex

for F in `ldd OpenSupaplex.exe | grep mingw64 | cut -d' ' -f3`
do
    cp $F ./${TARGET_DIR}/
done

zip -r ${TARGET_DIR}.zip ${TARGET_DIR}
