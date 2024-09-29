#!/bin/bash
docker run --rm -v "$(pwd)":/src -w /src sergiou87/vita-docker-open-supaplex:7.2 "$@"
