#!/bin/sh
# Serves the WASM build over HTTP for browser testing (wasm/ must be built first).
# Usage: $0 [port]

PORT="${1:-8931}"
DIR="$(cd "$(dirname "$0")/wasm" && pwd)"

if [ ! -f "$DIR/opensupaplex.js" ]; then
	echo "wasm/opensupaplex.js not found - run 'make' in wasm/ first" >&2
	exit 1
fi

echo "Serving $DIR at http://127.0.0.1:$PORT/"
echo "  game:        http://127.0.0.1:$PORT/opensupaplex.html"
echo "Press Ctrl+C to stop."

exec python3 -m http.server "$PORT" --bind 127.0.0.1 --directory "$DIR"
