=====
Introduction
=====

Compilation notes for Sorcery on Linux.

Updated 11th August 2026

My development environment is Ubuntu 26.04 LTS.

=====
Development Environment
=====

Sorcery is currently developed using:

* GCC 16.2
* CMake 4 or later (or thereabouts)

The project targets C++26 (it makes use of reflection), and thus requires at
minimum GCC16.1. As set up it uses GCC16.2, which is not yet released as a
package. Please note that you may need to adjust the following line in
CMakeLists.txt as necessary to pick up a compatible version of libstdc++.

```
set_target_properties(${PROJECT_NAME} PROPERTIES
 BUILD_RPATH "$ORIGIN/lib;/opt/gcc-16.2/lib64"
)
```

=====
Git LFS
=====

Sorcery stores its artwork using Git LFS.

Before cloning the repository, install Git LFS and enable it for the current
user:

```
sudo apt install git-lfs
git lfs install
```

Failure to do so may result in Git LFS placeholder files being checked out
instead of the artwork contained within gfx/.

As of 4th July 2026, the former sdl/imgui branch has replaced the master branch.

The older SFML-based version of Sorcery has been retained as an archived branch.
It is no longer maintained and may be removed from the active branch list in a
future revision.

======
VSCode Notes
======

The following VS Code configuration has been tested:

* Visual Studio Code 1.127
* C/C++ extension 1.29.3
* CMake Tools extension 1.23.52

At the time of writing, version 1.32.2 of the C/C++ extension causes debugging
problems on my system. Version 1.29.3 is therefore currently recommended. You may also find minor intellisense issues with the use of certain constant expressions in std::format calls throughout the program.

=====
Compilation Database
=====

CMake automatically generates compile_commands.json by enabling:

```
CMAKE_EXPORT_COMPILE_COMMANDS
```

The resulting compilation database is placed in the build directory and may be
used by clangd, clang-tidy and other compatible development tools.

No additional compilation database generator, such as Bear, is required.

======
Code Formatting
======

The project includes a top-level .clang-format file for automatic source-code
formatting.

clang-tidy and other static-analysis tools may use the generated
compile_commands.json file.

=====
System Dependencies
=====

Sorcery uses the following libraries supplied by the operating system:

* SDL2
* OpenGL
* GLEW
* FreeType
* FFmpeg
* jsoncpp
* GLM
* POSIX threads
* libuuid
* libdw
* Doxygen

On Ubuntu, the required packages can be installed with:

```
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

=====
External Libraries
=====

Third-party source dependencies are downloaded and configured automatically
using CMake FetchContent.

The project currently fetches the following libraries:

## Dear ImGui

Repository:

```
https://github.com/ocornut/imgui.git
```

Sorcery builds the required Dear ImGui core (v1.92.8), SDL2, OpenGL 3 and
FreeType source files as the dear_imgui static library.

## imgui_toggle

Repository:

```
https://github.com/cmdwtf/imgui_toggle.git
```

## ImSpinner

Repository:

```

<https://github.com/dalerank/imspinner.git>

```

ImSpinner is header-only and is exposed to the project through an INTERFACE
library target.

## imgui_sugar

Repository:

```
<https://github.com/mnesarco/imgui_sugar.git>

```

imgui_sugar is header-only and is exposed to the project through an INTERFACE
library target.

## cereal

Repository:

```

<https://github.com/USCiLab/cereal.git>

```

## SimpleIni

Repository:

```

<https://github.com/brofield/simpleini.git>

```

## stb

Repository:

```

<https://github.com/nothings/stb.git>

```

Pinned commit:

```

31c1ad37456438565541f4919958214b6e762fb4

```

=====
FetchContent Notes
=====

Dependencies downloaded through FetchContent are normally placed beneath:

```

build/_deps/

```

These downloaded source trees are build artefacts and are not stored in the
Sorcery repository.

The first CMake configuration therefore requires an internet connection.
Subsequent configurations and builds normally use the already populated copies.

Deleting the build directory also removes all downloaded FetchContent dependencies. They will be downloaded again during the next CMake
configuration.

Dependency revisions are pinned to specific tags or commit hashes to ensure
reproducible builds.

=====
Structure Notes
=====

Sorcery is organised into the following static libraries:

* sorcery_core
* sorcery_engine
* sorcery_frontend
* sorcery_gui
* sorcery_modules
* sorcery_resources
* sorcery_types
* dear_imgui
* sorcery_imgui_toggle

Header-only dependencies such as ImSpinner and imgui_sugar are represented by
CMake INTERFACE library targets.

The static libraries are linked together to produce the final Sorcery
executable.

Static archive files generated during compilation are build artefacts. They are
placed beneath:

```

build/lib/

```

They are required by the linker during development but are not runtime
dependencies and are not copied into the distribution directory.

=====
Building
=====

Clone the repository:

```

git clone <https://github.com/davemoore22/sorcery.git>
cd sorcery

```

Configure the project:

```

cmake -S . -B build

```

Compile the project:

```

cmake --build build

```

A parallel build may be requested with:

```

cmake --build build --parallel

```

The executable and runtime files are placed in:

```

build/dist/

```

The resulting application can be launched from the command line with:

```

./build/dist/sorcery

```

=====
Post-build
=====

The build automatically creates the runtime distribution directory and copies
the required data directories into it.

The resulting layout is:

```

build/dist/
    sorcery
    cfg/
    dat/
    doc/
    gfx/
    sav/
    sfx/
    vfx/

```

The sav/characters and sav/states directories are included as part of the sav/
directory and do not require separate copy commands.

The build does not copy static .a archives into the distribution directory,
because they are not required at runtime.

=====
Debugging
=====

GDB, CMake Tools Debug and standard Visual Studio Code debugging are supported.

The Sorcery executable is built directly into:

```

build/dist/

```

This allows CMake Tools to identify and launch the correct target automatically.

The debugger working directory should also be:

```

build/dist/

```

This is necessary because Sorcery loads its runtime configuration, data,
graphics, save files, sound effects and video effects relative to the working
directory.

When using a custom .vscode/launch.json configuration, use settings equivalent
to:

```

{
    "name": "Sorcery",
    "type": "cppdbg",
    "request": "launch",
    "program": "${workspaceFolder}/build/dist/sorcery",
    "cwd": "${workspaceFolder}/build/dist",
    "MIMode": "gdb"
}

```

For CMake Tools Debug, no custom executable path should be necessary because
CMAKE_RUNTIME_OUTPUT_DIRECTORY points to build/dist.
