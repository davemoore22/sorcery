# Compiling Sorcery

## Introduction

Compilation notes for Sorcery (**Updated September 2026**).

Sorcery uses a **single source tree** which builds on both Linux and Windows.

The primary development platform is Ubuntu/Debian Linux. Windows builds are supported using **MSYS2 UCRT64**, GCC, CMake and Ninja. Other Windows toolchains are not currently tested or supported by the project, although contributions documenting them are welcome.

The project targets **C++26** and uses C++ reflection, so **GCC 16.1 or later is required**. The current development toolchain uses GCC 16.2.

No separate Linux or Windows source tree is required. Platform-specific differences are handled by CMake and the source code.

---

## TL;DR

The common CMake workflow on both platforms is:

```sh
cmake -S . -B build
cmake --build build --parallel
```

To clean:

```sh
cmake --build build --target clean
```

To reconfigure:

```sh
cmake -S . -B build
```

The executable and runtime data are placed in:

```text
build/dist/
```

On Linux:

```sh
./build/dist/sorcery
```

On Windows, from an MSYS2 UCRT64 shell:

```sh
./build/dist/sorcery.exe
```

---

# Development Environment

Sorcery is currently developed and tested primarily with:

- GCC 16.2
- CMake 4.x
- C++26 with GCC reflection support
- Ubuntu 26.04
- Windows 10 using MSYS2 UCRT64

Linux remains the primary development platform.

The Windows build is intended to provide a supported native Windows executable using the same source tree. The supported Windows toolchain is MSYS2 UCRT64 with GCC, CMake and Ninja.

Other Windows build environments, including MSVC, clang-cl and other MinGW distributions, are currently untested.

---

# Git LFS

Sorcery stores its artwork using Git LFS.

Git LFS must be installed before cloning the repository, otherwise placeholder files may be checked out instead of the artwork contained in `gfx/`.

## Linux

```sh
sudo apt install git-lfs
git lfs install
```

## Windows / MSYS2

Install Git LFS from the MSYS2 shell if it is not already installed:

```sh
pacman -S git-lfs
git lfs install
```

After cloning, it is useful to verify that the LFS files have been obtained:

```sh
git lfs pull
```

---

# Supported Platforms

## Linux

The native Linux build is developed on Ubuntu 26.04 and should also be suitable for current Debian/Ubuntu-derived systems with a sufficiently recent GCC toolchain.

Linux uses the system-provided development libraries for SDL2, OpenGL, GLEW, FreeType, FFmpeg, JsonCpp, GLM, libuuid and related system dependencies.

## Windows

The supported Windows development environment is:

- Windows 10 or later
- MSYS2
- UCRT64 environment
- GCC 16.1 or later
- CMake
- Ninja

All Windows compilation commands should be run from the **MSYS2 UCRT64 shell**, not from the plain MSYS shell.

The project uses the MinGW-w64 UCRT runtime and produces a native Windows executable.

---

# System Dependencies

Sorcery uses the following installed libraries and software:

- SDL2
- OpenGL
- GLEW
- FreeType
- FFmpeg
- JsonCpp
- GLM

Linux additionally uses:

- POSIX threads
- libuuid
- libdw

Development/documentation tools include:

- Git
- Git LFS
- CMake
- Doxygen
- pkg-config / pkgconf

Other third-party source dependencies are downloaded automatically by CMake using `FetchContent`.

---

# Linux Dependencies

On Ubuntu 26.04, the required packages can be installed with:

```sh
sudo apt install \
    build-essential \
    cmake \
    git \
    git-lfs \
    pkg-config \
    libsdl2-dev \
    libgl-dev \
    libjsoncpp-dev \
    libfreetype6-dev \
    libglm-dev \
    libavcodec-dev \
    libavdevice-dev \
    libavfilter-dev \
    libavformat-dev \
    libswscale-dev \
    libswresample-dev \
    libavutil-dev \
    libglew-dev \
    uuid-dev \
    libdw-dev \
    doxygen
```

GCC 16.1 or later is required because Sorcery uses C++26 reflection.

At the time of writing, GCC 16.2 may need to be installed separately from the standard Ubuntu package set.

If GCC is installed outside the normal system paths, ensure that CMake can find the compiler and that the matching `libstdc++` is available at runtime.

The current Linux development setup uses a custom GCC installation beneath `/opt`. If your GCC installation is elsewhere, the Linux `BUILD_RPATH` configuration may need to be adjusted accordingly.

---

# Windows / MSYS2 Dependencies

Install MSYS2 and open the **UCRT64** shell.

Update the MSYS2 installation first:

```sh
pacman -Syu
```

Restart the UCRT64 shell if MSYS2 requests it, then complete the update if necessary:

```sh
pacman -Syu
```

Install the compiler and build tools:

```sh
pacman -S \
    mingw-w64-ucrt-x86_64-gcc \
    mingw-w64-ucrt-x86_64-cmake \
    mingw-w64-ucrt-x86_64-ninja \
    mingw-w64-ucrt-x86_64-pkgconf \
    git \
    git-lfs
```

Install the native Windows development libraries:

```sh
pacman -S \
    mingw-w64-ucrt-x86_64-SDL2 \
    mingw-w64-ucrt-x86_64-freetype \
    mingw-w64-ucrt-x86_64-glm \
    mingw-w64-ucrt-x86_64-ffmpeg \
    mingw-w64-ucrt-x86_64-glew \
    mingw-w64-ucrt-x86_64-jsoncpp
```

Verify that the UCRT64 compiler is being used:

```sh
g++ --version
g++ -dumpmachine
```

The target should be:

```text
x86_64-w64-mingw32
```

The project has been tested with GCC 16.2 from MSYS2 UCRT64.

---

# External Libraries

Third-party source dependencies are downloaded and configured automatically using CMake `FetchContent`.

The project currently fetches the following external libraries and headers.

## Dear ImGui

<https://github.com/ocornut/imgui.git>

Sorcery builds the required Dear ImGui core, SDL2, OpenGL 3 and FreeType source files as the `dear_imgui` static library.

## imgui_toggle

<https://github.com/cmdwtf/imgui_toggle.git>

This is built as the `dear_imgui_toggle` static library.

The following dependencies are header-only or exposed through CMake interface targets.

## ImSpinner

<https://github.com/dalerank/imspinner.git>

## imgui_sugar

<https://github.com/mnesarco/imgui_sugar.git>

## cereal

<https://github.com/USCiLab/cereal.git>

## SimpleIni

<https://github.com/brofield/simpleini.git>

## stb

<https://github.com/nothings/stb.git>

Pinned commit:

```text
31c1ad37456438565541f4919958214b6e762fb4
```

---

# FetchContent Notes

Dependencies downloaded through `FetchContent` are normally placed beneath:

```text
build/_deps/
```

These downloaded source trees are build artefacts and are not stored in the Sorcery repository.

The first CMake configuration therefore requires an internet connection. Subsequent configurations and builds normally use the already populated copies.

Deleting the build directory also removes all downloaded `FetchContent` dependencies. They will be downloaded again during the next CMake configuration.

Dependency revisions are pinned to specific tags or commit hashes where appropriate to improve reproducibility.

---

# Building

The same source tree is used on both Linux and Windows.

Clone the repository:

```sh
git clone https://github.com/davemoore22/sorcery.git
cd sorcery
```

If necessary:

```sh
git lfs pull
```

## Linux

Configure:

```sh
cmake -S . -B build
```

Compile:

```sh
cmake --build build --parallel
```

or, to specify a job count:

```sh
cmake --build build -j<cores>
```

Run:

```sh
./build/dist/sorcery
```

## Windows / MSYS2 UCRT64

Open an **MSYS2 UCRT64 shell** and change to the Sorcery source directory.

Configure with Ninja:

```sh
cmake -S . -B build -G Ninja
```

Compile:

```sh
cmake --build build --parallel
```

Run from the UCRT64 shell:

```sh
./build/dist/sorcery.exe
```

Running the development build from the UCRT64 shell allows Windows runtime DLL dependencies provided by MSYS2 to be found through the UCRT64 environment.

Creation of a self-contained Windows distribution, including the required runtime DLLs, is a separate packaging step.

---

# Clean Builds

To clean the current build without deleting the configuration:

```sh
cmake --build build --target clean
```

For a completely fresh configuration:

## Linux

```sh
rm -rf build
cmake -S . -B build
cmake --build build --parallel
```

## Windows / MSYS2 UCRT64

```sh
rm -rf build
cmake -S . -B build -G Ninja
cmake --build build --parallel
```

Deleting `build/` also removes all `FetchContent` dependencies beneath `build/_deps/`.

---

# Build Output

The build places the executable and runtime files beneath:

```text
build/dist/
```

The generated static libraries are placed beneath:

```text
build/lib/
```

The static archives are build artefacts used by the linker and are not required at runtime.

The runtime distribution directory contains:

```text
build/dist/
    sorcery          # Linux
    sorcery.exe      # Windows
    cfg/
    dat/
    doc/
    gfx/
    sav/
    sfx/
    vfx/
```

Only the executable appropriate to the current platform will normally be present.

The `sav/characters` and `sav/states` directories are included as part of `sav/`.
The `dat/fonts` directory are included as part of `dat`

---

# Runtime Libraries

## Linux

Linux normally resolves shared libraries from the system installation.

A binary copied to another Debian/Ubuntu machine therefore requires compatible runtime versions of its dynamically linked libraries.

For development and source builds, the required packages are documented above.

The runtime dependencies of a Linux executable may be inspected with:

```sh
ldd build/dist/sorcery
```

## Windows

Windows builds depend on MinGW/MSYS2 runtime DLLs and DLLs belonging to libraries such as SDL2, FFmpeg, FreeType and GLEW.

During development, the simplest method is to run Sorcery from the MSYS2 UCRT64 shell.

A release Windows distribution should contain the required DLLs beside `sorcery.exe`. Runtime DLL bundling is considered part of packaging rather than compilation.

---

# Graphics Requirements

Sorcery uses OpenGL 3.3 Core Profile.

Some of the current graphics resources are stored in large texture atlases. Systems with very limited graphics memory, particularly virtual machines, may fail to allocate these textures even when their reported maximum texture dimensions are sufficient.

For example, the current creature atlas is approximately 8000 × 8800 pixels and requires roughly 269 MiB of GPU memory when expanded as RGBA.

For this reason, virtual machines configured with only 256 MiB of graphics memory may not be able to run the current build successfully.

This is a runtime graphics limitation rather than a compilation requirement.

---

# Structure Notes

Sorcery is organised into a number of static libraries, including:

- `sorcery_core`
- `sorcery_engine`
- `sorcery_display`
- `sorcery_drawables`
- `sorcery_frontend`
- `sorcery_modules`
- `sorcery_resources`
- `sorcery_training`
- `sorcery_types`
- `dear_imgui`
- `dear_imgui_toggle`

Header-only dependencies such as ImSpinner and imgui_sugar are represented by CMake `INTERFACE` library targets.

The static libraries are linked together to produce the final Sorcery executable.

---

# Compilation Database

CMake automatically generates `compile_commands.json` by enabling:

```text
CMAKE_EXPORT_COMPILE_COMMANDS
```

The resulting compilation database is placed in the build directory and may be used by clangd, clang-tidy, Include-What-You-Use and other compatible development tools.

No additional compilation database generator such as Bear is required.

---

# Code Formatting

The project includes a top-level `.clang-format` file for automatic source-code formatting.

clang-tidy and other static-analysis tools may use the generated `compile_commands.json` file.

---

# Include-What-You-Use

Include-What-You-Use (<https://include-what-you-use.org/>) is supported on Linux.

For example, to build a special version beneath `build-iwyu/` and save the results to `iwyu.txt`:

```sh
cmake -S . -B build-iwyu -DSORCERY_ENABLE_IWYU=ON
cmake --build build-iwyu --parallel 2>&1 | tee iwyu.txt
```

This assumes IWYU has already been compiled or installed locally.

---

# VS Code Notes

The following Visual Studio Code configuration has been tested on the primary Linux development system:

- Visual Studio Code 1.127
- C/C++ extension 1.29.3
- CMake Tools extension 1.23.52

At the time of writing, version 1.32.2 of the C/C++ extension causes debugging problems on the primary development system. Version 1.29.3 is therefore currently recommended there.

Minor IntelliSense issues may also occur with some C++26/reflection or constant-expression code.

These notes describe the tested development setup rather than a hard project requirement.

---

# Debugging

GDB and CMake Tools debugging are supported.

The executable is built directly into:

```text
build/dist/
```

The debugger working directory should also be:

```text
build/dist/
```

This is important because Sorcery loads its runtime configuration, data, graphics, save files, sound effects and video effects relative to its working directory.

## Linux example

A `.vscode/launch.json` configuration may use settings equivalent to:

```json
{
    "name": "Sorcery",
    "type": "cppdbg",
    "request": "launch",
    "program": "${workspaceFolder}/build/dist/sorcery",
    "cwd": "${workspaceFolder}/build/dist",
    "MIMode": "gdb"
}
```

## Windows / MSYS2 example

The corresponding executable is:

```text
${workspaceFolder}/build/dist/sorcery.exe
```

and the working directory remains:

```text
${workspaceFolder}/build/dist
```

When debugging the Windows build, use the GDB supplied by the MSYS2 UCRT64 toolchain.

For CMake Tools Debug, no custom executable path should normally be necessary because `CMAKE_RUNTIME_OUTPUT_DIRECTORY` points to `build/dist/`.

---

# Post-build

The build automatically creates the runtime distribution directory and copies the required Sorcery data directories into it.

The CMake project is intended to keep the Linux and Windows build processes as similar as possible. Platform-specific system libraries and runtime handling are selected by CMake, while the same application source is compiled on both platforms.

Linux remains the primary development platform. Windows support is provided through the documented MSYS2 UCRT64 configuration.
