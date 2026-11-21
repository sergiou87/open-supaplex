#!/bin/bash

apt-get update && apt-get install make git-core cmake python
rm -rf vdpm # debug only, remove me
git clone https://github.com/vitasdk/vdpm
cd vdpm
export VITASDK=/usr/local/vitasdk
export PATH=$VITASDK/bin:$PATH # add vitasdk tool to $PATH
./bootstrap-vitasdk.sh
./install-all.sh
