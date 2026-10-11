#!/usr/bin/env bash
set -euo pipefail

root_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "${root_dir}"

if ! command -v python3 >/dev/null 2>&1; then
    printf 'python3 is required to serve the web build.\n' >&2
    exit 1
fi

if [[ ! -f build/web/flystate.html ]]; then
    printf 'build/web/flystate.html was not found. Run ./scripts/build-web.sh first.\n' >&2
    exit 1
fi

printf 'Serving http://localhost:8000/flystate.html\n'
python3 -m http.server 8000 --directory build/web
