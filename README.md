# Raylib Game Jam Template

This is a C++20 raylib starter project for Linux, Windows, and WebAssembly.

## Setup

```sh
./scripts/setup-dependencies.sh
```

The script clones raylib and Jolt Physics into `3rdparty/` without using CMake FetchContent. Jolt is optional and is disabled by default.

## Build

Desktop builds use the CMake presets:

```sh
cmake --preset linux
cmake --build --preset linux

# On a machine with Visual Studio 2026 and CMake 4.2 or newer:
cmake --preset windows
cmake --build --preset windows
```

For WebAssembly, activate Emscripten and run:

```sh
./scripts/build-web.sh
```

Equivalently, configure and build the web preset separately:

```sh
emcmake cmake --preset web
cmake --build --preset web
```

Serve the result locally with Python, then open <http://localhost:8000/raylib_jam.html>:

```sh
./scripts/serve-web.sh
```

Rebuild after changing the web shell:

```sh
cmake --build --preset web
```

This avoids requiring `npx` or an additional package download. If using `npx serve` manually, run it from `build/web` and open `/raylib_jam.html` rather than `/`.

The web output is `build/web/raylib_jam.html`; serve `build/web/` through a local HTTP server so the browser can load the generated files. `src/game.cpp` contains the desktop/Web split and a minimal centered gray web page shell. Browser console logging remains available through `printf`/Emscripten's console forwarding.
That said, use raylib's `TraceLog` for logging.

Each configured build generates `compile_commands.json` inside its platform build directory and updates `build/compile_commands.json` to point at the active database. This matches clangd's `--compile-commands-dir=build` convention. Configure the preset you intend to use at least once before starting clangd.

To enable Jolt, configure with `-DUSE_JOLT=ON` after running the dependency setup script.
