#!/bin/sh

set -eu

: "${PS2DEV:?PS2DEV must be set}"
: "${PS2SDK:?PS2SDK must be set}"

test -d "$PS2DEV/gsKit/include"
test -d "$PS2DEV/gsKit/lib"
test -d "$PS2SDK/ports/include"
test -d "$PS2SDK/ports/lib"
test -f "$PS2SDK/samples/Makefile.pref"
test -f "$PS2SDK/samples/Makefile.eeglobal_cpp"