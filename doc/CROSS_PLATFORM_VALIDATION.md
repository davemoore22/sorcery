# Cross-platform validation — 27 September 2026

Tested source: `ee25df25b076b4c772ba10c2577db49bbae772d6`, originally on
`codex/macos-support` and now named `m-series_mac_support` on the Mac.
No source changes were needed for the Windows or Linux builds.
The earlier native macOS results are recorded in [MACOS.md](MACOS.md).

## Windows

Verified on Windows 11 Home x64, build 26200, AMD Ryzen 7 7700, using an
isolated MSYS2 UCRT64 installation:

- GCC 16.2.0, target `x86_64-w64-mingw32`; CMake 4.4.3; Ninja 1.13.2.
- SDL2 2.32.10, FreeType 2.14.3, GLM 1.0.3, GLEW 2.3.1,
  JsonCpp 1.9.8 and FFmpeg 9.0.2.
- Full Debug compilation, linkage and runtime-data assembly succeeded.
- Both CTest checks passed: `enum_resource_compatibility` and
  `preserve_saves_on_rebuild`.

The build explicitly retained the original C++26 reflection implementation:

```sh
cmake -S . -B build/windows -G Ninja \
  -DCMAKE_BUILD_TYPE=Debug \
  -DSORCERY_PORTABLE_ENUMS=OFF \
  -DSORCERY_BUILD_TESTS=ON
cmake --build build/windows --parallel 8
ctest --test-dir build/windows --output-on-failure
```

Desktop smoke testing observed the title screen, animated background, new-game
confirmation, castle, edge of town and rendered dungeon. Mouse menu navigation
worked. The application closed normally. Runtime output reported OpenGL 3.3
on an NVIDIA GeForce RTX 4070 Ti SUPER, complete framebuffer and successful
shader compilation/linking.

Executable SHA-256:
`62e920917afbdf0f6b3690f3ba1283e86b9cb6feb76d1d3d7a6304537f41c078`.

The unchanged Windows dependency-copy script emitted CMake CMP0207 path
normalization warnings and listed unresolved Windows components:
`AzureAttestManager.dll`, `AzureAttestNormal.dll`, `HvsiFileTrust.dll`,
`PdmUtilities.dll`, and `wpaxholder.dll`. These did not prevent the observed
launch, but this test does not certify a clean-machine redistribution package.

Keyboard movement, quicksave/quickload, audio audibility, controllers,
fullscreen/resizing and extended gameplay remain unverified on Windows.
Remote keyboard forwarding was unreliable during the smoke test.

## Linux

Verified on Ubuntu 26.04.1 x86_64, kernel 7.0.0-30, Intel Core i5-3210M:

- Ubuntu GCC 16.0.1 experimental snapshot dated 20260322
  (`r16-8246-g569ace1fa50`); CMake 4.2.3; Ninja 1.13.2.
- SDL2 2.32.10, FreeType 2.14.2, GLM 1.0.1, GLEW 2.2.0,
  JsonCpp 1.9.6 and FFmpeg 8.0.1 development packages.
- Full Debug compilation, linkage and runtime-data assembly succeeded.
- Both CTest checks passed: `enum_resource_compatibility` and
  `preserve_saves_on_rebuild`.

The build retained the original C++26 reflection implementation:

```sh
cmake -S . -B build/linux -G Ninja \
  -DCMAKE_C_COMPILER=gcc-16 -DCMAKE_CXX_COMPILER=g++-16 \
  -DCMAKE_BUILD_TYPE=Debug \
  -DSORCERY_PORTABLE_ENUMS=OFF \
  -DSORCERY_BUILD_TESTS=ON
cmake --build build/linux --parallel 2
ctest --test-dir build/linux --output-on-failure
```

This records the actual Ubuntu compiler snapshot tested, not a released
GCC 16.1/16.2 toolchain. The full build and checks passed with that snapshot.

The executable launched in the existing Ubuntu desktop session. Its rendered
title screen and menu were observed through RustDesk, and the game was left
open for the user's hands-on testing. Dungeon gameplay, input, save/load,
audio audibility, controllers, fullscreen/resizing and extended play remain
unverified on Linux.

Executable SHA-256:
`f44a99a5313111297e2ee6e479af84337acde093f027f29dbdac5bbcbe5c62ec`.

## Transfer provenance

The source archive contained all 334 tracked files, including 14 materialized
Git LFS artwork files checked against their LFS SHA-256 identifiers. Its
SHA-256 was verified on both destination machines:
`0dba596139169f2287dc7cf0a38b927f4b7946ca157fb702f48c47b54c5e30a3`.

A separate Git bundle contains the branch and baseline history; it does not
include LFS objects. The archive supplies the actual artwork. Local logs,
transfer manifests and setup scripts are retained under `build/transfer/`
(ignored by Git). No upstream push or pull request was made.
