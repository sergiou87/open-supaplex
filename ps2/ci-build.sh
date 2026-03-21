#!/bin/sh

set -eu

cd "$(dirname "$0")"

bundle_dir=OpenSupaplex

make clean
make -j8

rm -rf "$bundle_dir" OpenSupaplex-ps2.zip
mkdir "$bundle_dir"
cp OpenSupaplex.elf "$bundle_dir"/
cp -R ../resources/* "$bundle_dir"/

zip -r OpenSupaplex-ps2.zip "$bundle_dir"