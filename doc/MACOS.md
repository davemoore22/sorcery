# Native macOS development build

This branch adds an experimental Apple Silicon build using Apple Clang and
C++23. Linux/Windows continue to use the existing GCC/C++26 reflection path.

## Build and run

Install Xcode (or a sufficiently recent Command Line Tools release) and Homebrew.
The verified compiler is Apple Clang 21; older toolchains have not been tested.

```sh
brew install cmake ninja pkgconf sdl2 freetype glm glew jsoncpp ffmpeg
cmake -S . -B build/macos -G Ninja \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_PREFIX_PATH="$(brew --prefix)" \
  -DSORCERY_BUILD_TESTS=ON
cmake --build build/macos --parallel 8
ctest --test-dir build/macos --output-on-failure
./build/macos/dist/sorcery
```

Run the configure command from the repository root. The built executable can
subsequently be launched by absolute path from any working directory. Keep the
`cfg`, `dat`, `doc`, `gfx`, `sav`, `sfx`, and `vfx` directories beside it.

The runtime is under `build/macos/dist`. New saves belong to that directory;
source-tree saves are unchanged. Rebuilding seeds missing saves without
replacing existing progress. The upstream build still refreshes configuration
files from `cfg` during linking, so local runtime option changes can reset on a
rebuild. Back up `build/macos/dist/sav` before removing the build directory.

## Port details

- macOS defaults `SORCERY_PORTABLE_ENUMS=ON`, using pinned magic_enum 0.9.7
  behind the existing enum lookup API. Current reflected enums fit its default
  range of -128 through 127 and have no duplicate values. Future larger values
  or aliases need explicit review; this fallback is not full C++26 reflection.
- Darwin executable lookup uses `_NSGetExecutablePath` and canonical paths.
  Font scanning uses the same executable-relative resource store.
- SDL requests a forward-compatible OpenGL 4.1 core context on macOS, retaining
  the game's GLSL 330 shaders.
- GCC-only libraries/options and Linux runtime paths are excluded from the
  Apple Clang build. UUID functions come from Darwin's libSystem.
- Standard C++ declarations, explicit string map keys, out-of-line destructors,
  and string conversion for path formatting address Clang/libc++ differences.

## Validation — 27 September 2026

Verified locally on Apple M2 Pro (arm64), macOS 27.2 build 26B5091g,
Apple Clang 21.0.0 (clang-2100.3.34.2), CMake 4.3.3, SDL 2.32.10, and FFmpeg 8.0.1:

- Full Debug compilation and native arm64 executable linkage.
- Enum names, negative values, sparse values, invalid strings, and integer
  overflow boundaries used by resource/save conversion.
- Startup from `/tmp`, resource loading, title screen, animated background,
  mouse navigation, new game, castle, and edge of town.
- Dungeon rendering, keyboard movement/turning, and automap.
- F9 wrote a quicksave; after turning, F10 restored the saved facing direction.
- OpenGL reported `4.1 Metal - 91.7`, renderer `Apple M2 Pro`, successful shader
  compilation/linking, complete framebuffer, and 2x Retina drawable scaling.

For native UI automation, a local `build/macos/Sorcery Smoke Test.app` wrapper
was assembled around an identical copy of the executable, with symlinks to
`dist` runtime directories. It is a development-only test artifact, not a
redistributable application bundle or an automatically refreshed build target.

Not yet validated: Intel Macs, older macOS/Xcode versions, Release builds,
controller input, fullscreen/resizing, long gameplay sessions, audible music
quality, or cross-platform save interchange. Linux/Windows were not rebuilt in
this session. Existing warnings include duplicate static libraries and a stale
SDK search path inherited from an installed dependency; neither prevented
linking or startup.

The executable currently depends on Homebrew libraries. Distribution still
needs a proper resource/save layout, bundled dependencies, signing/notarization,
and testing on a clean Mac.
