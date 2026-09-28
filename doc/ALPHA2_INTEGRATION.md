# Alpha 2 integration validation — 28 September 2026

Tested commit: `45ff45c1201bf4818756079eb0dc3a521d420686` on
`m-series_mac_support`. This merges the previously tested Mac support with
`Alpha_2_Spellcasting` at `ffd88d4f6592769b11af02b1e4411becf590e30d`.

## Integration

The merge retains Dear ImGui `v1.92.8-docking`, Dave's viewport/coordinate
changes, the new game/magic libraries, and the Alpha 2 gameplay changes.
The two textual conflicts were resolved in the README and `src/types/meta.cpp`.
The latter now includes the relocated spell and input-mode declarations while
retaining the Apple C++23 enum adapter and GCC C++26 reflection implementation.
Enum tests were extended for spell identifiers, the negative no-spell value,
field/combat/trap cast contexts, and the engine input mode.

## Native build results

Fresh, separate build directories were used, preserving the earlier builds
and their saves. All three machines built the same source commit:

| Platform | Compiler | Enum implementation | Debug build | CTest |
| --- | --- | --- | --- | --- |
| Apple M2 Pro, macOS 27.2 | Apple Clang 21.0.0 | Portable C++23 adapter | Passed | 2/2 passed |
| Windows 11 x64, MSYS2 UCRT64 | GCC 16.2.0 | C++26 reflection | Passed | 2/2 passed |
| Ubuntu 26.04.1 x86_64 | GCC 16.0.1 experimental, 20260322 | C++26 reflection | Passed | 2/2 passed |

The checks are `enum_resource_compatibility` and `preserve_saves_on_rebuild`.
They do not establish correctness of every new spell or UI feature. Ubuntu's
compiler is a distribution snapshot, not a released GCC 16.1/16.2 toolchain.

Build commands follow [MACOS.md](MACOS.md) and
[CROSS_PLATFORM_VALIDATION.md](CROSS_PLATFORM_VALIDATION.md), with the Mac
build directory changed to `build/macos-alpha2`. Windows and Linux explicitly
used `SORCERY_PORTABLE_ENUMS=OFF`; all enabled `SORCERY_BUILD_TESTS`.

## Runtime observations and remaining checks

On the Mac, observed the title screen, mouse menu selection, options, fullscreen
entry and return to windowed mode, F1 menu/dungeon help, F2 cheat tools, a new
game, castle, edge of town, and dungeon movement. A Dear ImGui metrics window
was dragged outside the main window and rendered as a separate native window.
The game also rendered on the external display after the initial Retina test.
A quicksave file was written in the separate Alpha 2 save directory.

Concurrent desktop interaction interrupted the controlled save/load and
gameplay checks. Do not treat quickload restoration, spell effects, buff
icons, darkness extinguishing light, sustained texture unloading, or the
claimed GPU-memory reduction as verified by this session. Repeated resizing,
detached-window lifecycle across displays, and audible music quality also
need further checks.

The Ubuntu executable launched in the existing desktop session and remained
running. Its Alpha 2 UI/gameplay was not visually verified. The Windows
executable and a separate desktop launcher were prepared; its Alpha 2
runtime checks remain pending. Earlier runtime results in
[CROSS_PLATFORM_VALIDATION.md](CROSS_PLATFORM_VALIDATION.md) apply to the older
source revision, not this combined build.

Before requesting merge, complete the remaining runtime checks on the same
commit, particularly fullscreen/input alignment on Windows and Ubuntu, the
implemented field spells and their targeting/cancellation paths, save/load,
and repeated screen transitions. Retest affected paths if further Alpha 2
work is incorporated.

## Observations to discuss separately

The Mac music-volume slider changed from 33% to 54% in the running UI, but
`music_volume` remained 33 in the runtime configuration after Save. The slider
updates the audio player without updating the configuration value. This UI
code is unchanged from Dave's branch; no volume-persistence fix is included.

Existing build warnings remain: GCC-only warning pragmas ignored by Clang,
unused variables in the docking changes, duplicate static libraries, a stale
dependency SDK path, and Windows runtime-dependency-copy warnings. These did
not prevent the builds. A clean-machine redistribution package is unverified.

## Artifact provenance

The transferred archive contained all 357 tracked files, with the 14 Git LFS
artwork files materialized and checked against their pointer hashes. Both
remote machines verified the archive before extraction.

SHA-256 values:

- Source archive: `a59f788904c351d3ed59cf38b45494fe8e9010630d5bb05d0c0972bb5f39a121`
- Mac executable: `f4def630c84739c25fcb9541ee8090189b5f1ccc07cbd82469babfb73ddb4977`
- Windows executable: `9d62cb0055f92b369920d162f48f59478d4ad01f2f26804a13a30c5f3b40e62d`
- Ubuntu executable: `b3d6b26c90609eb4d2f95d68def4d5d171786ff7c27e8ce7f29724f089c192e2`

Local logs, the transfer manifest, and runtime evidence are retained under
`build/alpha2-evidence/` (ignored by Git). The Mac app wrapper is a development
test artifact with an identical executable and links to this build's runtime
resources, not a distribution bundle.
