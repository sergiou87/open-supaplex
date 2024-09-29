#!/bin/bash
docker run --rm -v "$(pwd)":/src -w /src sergiou87/wii-docker-open-supaplex:7.2 "$@"
