#!/usr/bin/env bash
set -euo pipefail

root_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "${root_dir}"

if ! command -v emcmake >/dev/null 2>&1; then
    printf 'emcmake was not found. Activate the Emscripten environment first.\n' >&2
    exit 1
fi

emcmake cmake --preset web
cmake --build --preset web
