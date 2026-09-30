#!/bin/sh

set -eu

repo_root=${1:-.}

validate_level_dir() {
    dir=$1

    for file in "$dir"/LEVEL.L*; do
        [ -e "$file" ] || continue

        base_name=$(basename "$file")
        file_size=$(wc -c < "$file" | tr -d '[:space:]')

        case "$base_name" in
            LEVEL.LST|LEVEL.L[0-9][0-9])
                expected_size=3108
                ;;
            *)
                echo "Unexpected level file: $file" >&2
                exit 1
                ;;
        esac

        if [ "$file_size" -ne "$expected_size" ]; then
            echo "Invalid size for $file: expected $expected_size bytes, got $file_size" >&2
            exit 1
        fi
    done
}

validate_level_dir "$repo_root/resources"
validate_level_dir "$repo_root/tests/OpenSupaplex-tests"