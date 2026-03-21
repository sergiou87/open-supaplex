#!/bin/sh

repo_root=$(cd "$(dirname "$0")/.." && pwd)
image_name=opensupaplex-ps2-ci

docker build -f "$repo_root/ci/Dockerfile-ps2" -t "$image_name" "$repo_root/ci"
docker run --rm -v "$repo_root":/src -w /src "$image_name" "$@"