#!/usr/bin/env bash
set -euo pipefail

root_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
thirdparty_dir="${root_dir}/3rdparty"
mkdir -p "${thirdparty_dir}"

clone_if_missing() {
    local url="$1"
    local destination="$2"

    if [[ -d "${destination}/.git" ]]; then
        printf 'Already present: %s\n' "${destination}"
    elif [[ -e "${destination}" ]]; then
        printf 'Refusing to overwrite non-git path: %s\n' "${destination}" >&2
        exit 1
    else
        git clone --depth 1 "${url}" "${destination}"
    fi
}

clone_if_missing "https://github.com/raysan5/raylib.git" "${thirdparty_dir}/raylib"
clone_if_missing "https://github.com/jrouwe/JoltPhysics.git" "${thirdparty_dir}/JoltPhysics"

printf '\nDependencies are ready in %s\n' "${thirdparty_dir}"
