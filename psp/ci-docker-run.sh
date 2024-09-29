#!/bin/bash
docker run --rm -v "$(pwd)":/src -w /src sergiou87/pspdev-docker-open-supaplex:7.1 "$@"
