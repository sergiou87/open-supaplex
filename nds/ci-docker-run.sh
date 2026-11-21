#!/bin/bash
docker run --rm -v "$(pwd)":/src -w /src sergiou87/nds-docker-open-supaplex:7.2 "$@"
