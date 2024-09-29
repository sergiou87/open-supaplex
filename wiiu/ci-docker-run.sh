#!/bin/bash
docker run --rm -v "$(pwd)":/src -w /src sergiou87/wiiu-docker-open-supaplex:7.2 "$@"
