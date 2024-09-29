#!/bin/bash

source /etc/profile.d/devkit-env.sh
cd nds
make -j8 || exit 1
