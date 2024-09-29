#!/bin/bash
docker run --rm -v "$(pwd)":/src -w /src sergiou87/ps3dev-docker-open-supaplex:7.2 "$@"
