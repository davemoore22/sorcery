# Alpha 2 integration validation — 28 September 2026

Latest tested source commit: `f359467` on `m-series_mac_support`.
The initial integration commit `45ff45c1201bf4818756079eb0dc3a521d420686`
merges the previously tested Mac support with
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
and their saves. All three machines built the initial integration and then
rebuilt successfully at `f359467`; the results below apply to the latest source:

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

At the initial integration commit on Mac, observed the title screen, mouse
menus, options, fullscreen entry and return to windowed mode, F1 help, F2
cheat tools, a new game, castle, edge of town, and dungeon movement. A Dear
ImGui metrics window was dragged outside the main window and rendered as a
separate native window. Rendering was also observed on an external display.
A quicksave was written. Concurrent desktop interaction interrupted controlled
quickload checks. The latest two fixes have build/CTest coverage on Mac but
have not been manually retested there; the existing development app wrapper
still contains the initial integration executable.

On Windows, observed the title screen, menus, castle, dungeon movement, F1
help, F2 cheat tools, and the corrected full-window tiled background. Cheat
controls enabled the LOMILWA, LATUMAPIC and MAPORFIC buff icons, with improved
lighting and the displayed armour class changing from 10 to 8. This does not
validate casting those spells through the normal spell interface. F9 followed
by rotation and F10 restored the saved orientation. With `f359467`, quickload
and character inspection also completed without crashing. Fullscreen rendered,
but subsequent input/return to windowed mode was inconclusive through the
remote session and needs a controlled repeat.

On Ubuntu, observed the title/menu flow, dungeon movement, corrected tiled
background, and character inspection. Quickload followed by character
inspection initially crashed; GDB identified a null Context pointer in
`Character::race_to_str`. After `f359467`, the same quicksave restored party
orientation and opened the priest's details without crashing. The spell
prompt opened, but a successful normal spell cast was not established.

Before requesting merge, complete the remaining runtime checks on the same
commit: Mac regression checks for the latest fixes, fullscreen/input alignment
on Windows and Ubuntu, the implemented field spells and targeting/cancellation
paths, darkness extinguishing light, repeated screen transitions, and detached
window lifecycle across displays. Audible music quality, sustained texture
unloading and the claimed GPU-memory reduction are not verified. Retest
changed paths if further Alpha 2 work is incorporated.

## Runtime fixes found during integration testing

Both faults were present in the imported Alpha 2 code:

- `1cfc308`: use the viewport's screen position when drawing and clipping the
  tiled background. Previously, a window away from the screen origin could
  have a partially missing background. Verified visually on Ubuntu and Windows.
- `f359467`: restore runtime Context links for the deserialized state,
  characters and creation candidate after quickload. This reuses the existing
  `post_construct` lifecycle and does not change the save format. The crashing
  quickload/inspection path now passes manually on Ubuntu and Windows. The
  automated tests do not cover this runtime regression.

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
remote machines verified the archive before extraction. Subsequent fixes were
transferred as the three changed source files; their SHA-256 values matched
the local checkout on both remote machines before the final build.

SHA-256 values (archive at `45ff45c`; executables at `f359467`):

- Source archive: `a59f788904c351d3ed59cf38b45494fe8e9010630d5bb05d0c0972bb5f39a121`
- Mac executable: `d39e0cf252be735a61893b10d33f954118a35f185a6773d40787f6993193dc7f`
- Windows executable: `4ad2af6dfa16e2eb0215d5daa418fa6e09cb34b8860eaa69556c193e74528ef1`
- Ubuntu executable: `7b08820720c993c7dceae9628062b9271c585c653e5eb1d415a584f8a3d80eb1`

Local logs, the transfer manifest, and runtime evidence are retained under
`build/alpha2-evidence/` (ignored by Git). The Mac app wrapper is a development
test artifact with links to the build's runtime resources, not a distribution
bundle. Its older executable is explicitly distinguished above.
