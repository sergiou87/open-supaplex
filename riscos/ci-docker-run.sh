#!/bin/bash
docker run --rm -v "$(pwd)":/src -w /src sergiou87/riscos-docker-open-supaplex:7.2 "$@"
