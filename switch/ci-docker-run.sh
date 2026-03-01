#!/bin/bash
docker run --rm -v "$(pwd)":/src -w /src sergiou87/switch-docker-open-supaplex:7.1 "$@"
