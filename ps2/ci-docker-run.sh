#!/bin/sh

repo_root=$(cd "$(dirname "$0")/.." && pwd)
image_name=sergiou87/ps2dev-docker-open-supaplex:7.2

if ! docker pull "$image_name"; then
	docker build -f "$repo_root/ci/Dockerfile-ps2" -t "$image_name" "$repo_root/ci"
fi

docker run --rm -v "$repo_root":/src -w /src "$image_name" "$@"