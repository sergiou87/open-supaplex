#!/bin/bash

source /home/riscos/gccsdk-params
cd riscos
make -j8 OpenSupaplex-riscos.zip || exit 1
