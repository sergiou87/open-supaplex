#!/bin/bash
docker run --rm -v "$(pwd)":/src -w /src sergiou87/3ds-docker-open-supaplex:7.1 "$@"
