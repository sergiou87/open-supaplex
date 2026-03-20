#!/bin/bash

set -euo pipefail

: "${PS2DEV:?PS2DEV must be set}"
: "${PS2SDK:?PS2SDK must be set}"

if ! command -v ps2-packer >/dev/null 2>&1; then
    echo "ps2-packer not found in PATH" >&2
    exit 1
fi

test -d "$PS2DEV/gsKit/include"
test -d "$PS2DEV/gsKit/lib"
test -d "$PS2SDK/ports/include"
test -d "$PS2SDK/ports/lib"
test -f "$PS2SDK/samples/Makefile.pref"
test -f "$PS2SDK/samples/Makefile.eeglobal_cpp"